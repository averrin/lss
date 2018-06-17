#include <chrono>
#include <cmath>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <utility>

#include "cinder/Rand.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderPango.h"
#include "lss/command.hpp"
#include "lss/modes.hpp"
#include "lss/state.hpp"

#include "lss/game/door.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/events.hpp"
#include "lss/game/player.hpp"

#include "format.h"
#include "lss/ui/statusLine.hpp"
#include "rang.hpp"

#include "EventBus.hpp"
#include "EventHandler.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

int HOffset = 24;
int VOffset = 24;
// std::string DEFAULT_FONT = "Iosevka 14";
std::string DEFAULT_FONT = "FiraCode 12";
auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

QuitCommandEvent::QuitCommandEvent() : CommandEvent(nullptr) {}
std::shared_ptr<CommandEvent> QuitCommand::getEvent(std::string s) {
  return std::make_shared<QuitCommandEvent>();
}

class LSSApp : public App,
               public eb::EventHandler<eb::Event>,
               public eb::EventHandler<QuitCommandEvent> {
public:
  void setup() override;
  void mouseDown(MouseEvent event) override;
  void update() override;
  void keyDown(KeyEvent event) override;
  void draw() override;
  void invalidate();
  bool processCommand(std::string);
  void setListeners();
  void loadMap();

  kp::pango::CinderPangoRef mPango;
  kp::pango::CinderPangoRef statusFrame;
  ModeManager modeManager = ModeManager();
  std::shared_ptr<HintsMode> hints;
  std::shared_ptr<StatusLine> statusLine;
  std::shared_ptr<State> state;
  std::shared_ptr<Player> hero;

  std::vector<std::shared_ptr<Command>> commands;

  std::string typedCommand;

  virtual void onEvent(eb::Event &e) override;
  virtual void onEvent(QuitCommandEvent &e) override;
};

void LSSApp::onEvent(eb::Event &e) { invalidate(); }
void LSSApp::onEvent(QuitCommandEvent &e) { exit(0); }

void LSSApp::setup() {
  kp::pango::CinderPango::setTextRenderer(kp::pango::TextRenderer::FREETYPE);
  mPango = kp::pango::CinderPango::create();
  mPango->setMinSize(800, 600);
  mPango->setMaxSize(getWindowWidth(), getWindowHeight());
  mPango->setSpacing(mPango->getSpacing() - 4.0);

  statusFrame = kp::pango::CinderPango::create();
  statusFrame->setMinSize(getWindowWidth(), StatusLine::HEIGHT);
  statusFrame->setMaxSize(getWindowWidth(), StatusLine::HEIGHT);

  state = std::make_shared<State>();

  statusLine = std::make_shared<StatusLine>(state);
  statusLine->setContent(State::normal_mode);

  loadMap();
  invalidate();

  setListeners();

  commands.push_back(std::make_shared<MoveCommand>());
  commands.push_back(std::make_shared<QuitCommand>());
  commands.push_back(std::make_shared<PickCommand>());
  commands.push_back(std::make_shared<DigCommand>());
}

void LSSApp::loadMap() {
  hero = std::make_shared<Player>();
  hero->currentLocation = std::make_shared<Location>();

  std::string line;
  std::ifstream dungeon_file("./dungeon.ascii");
  int n, i;
  if (dungeon_file.is_open()) {
    n = 0;
    while (getline(dungeon_file, line)) {
      hero->currentLocation->cells.push_back({});
      i = 0;
      for (auto ch : line) {
        auto c = std::make_shared<Cell>(i, n, CellType::WALL);
        c->visibilityState = VisibilityState::UNKNOWN;
        c->type = CellType::UNKNOWN_CELL;
        c->passThrough = false;
        c->seeThrough = false;

        switch (ch) {
        case '.':
          c->type = CellType::FLOOR;
            c->passThrough = true;
            c->seeThrough = true;
          break;
        case '#':
          c->type = CellType::WALL;
          c->passThrough = false;
          c->seeThrough = false;
          break;
        case '+': {
          auto door = std::make_shared<Door>();
          door->currentCell = c;
          c->type = CellType::FLOOR;
          c->passThrough = true;
          hero->currentLocation->objects.push_back(door);
        } break;
        case 'e':
          auto enemy = std::make_shared<Enemy>();
          enemy->currentCell = c;
          c->type = CellType::FLOOR;
          c->passThrough = true;
          hero->currentLocation->objects.push_back(enemy);
          break;
        }
        hero->currentLocation->cells[n].push_back(c);
        i++;
      }
      n++;
    }
    dungeon_file.close();
  }
  state->currentPalette = palettes::DARK;
  hero->currentCell = hero->currentLocation->cells[15][30];
  hero->calcViewField();

  auto axe = std::make_shared<Item>(ItemType::PICK_AXE, "pick axe");
  axe->currentCell = hero->currentLocation->cells[16][31];
hero->currentLocation->objects.push_back(axe);

  state->fragments.assign(n * (i + 1), std::make_shared<Unknown>());
}

void LSSApp::setListeners() {
  eb::EventBus::AddHandler<EnemyDiedEvent>(*hero->currentLocation);
  eb::EventBus::AddHandler<ItemTakenEvent>(*hero->currentLocation);
  eb::EventBus::AddHandler<EnterCellEvent>(*hero->currentLocation, hero);
  eb::EventBus::AddHandler<DigEvent>(*hero->currentLocation, hero);
  eb::EventBus::AddHandler<ItemTakenEvent>(*statusLine);
  eb::EventBus::AddHandler<ItemsFoundEvent>(*statusLine);
  eb::EventBus::AddHandler<MessageEvent>(*statusLine);

  eb::EventBus::AddHandler<eb::Event>(*this);
  eb::EventBus::AddHandler<QuitCommandEvent>(*this);
  eb::EventBus::AddHandler<PickCommandEvent>(*hero);
  eb::EventBus::AddHandler<DigCommandEvent>(*hero);

  eb::EventBus::AddHandler<DoorOpenedEvent>(*statusLine);
  eb::EventBus::AddHandler<EnemyDiedEvent>(*statusLine);
  eb::EventBus::AddHandler<EnemyTakeDamageEvent>(*statusLine);
}

void LSSApp::invalidate() {
  auto t0 = std::chrono::system_clock::now();
  auto row = 0;
  auto index = 0;
  for (auto r : hero->currentLocation->cells) {
    auto column = 0;
    for (auto c : r) {
      auto cc = hero->currentCell;
      if (hero->canSee(c)) {
        c->visibilityState = VisibilityState::VISIBLE;
      } else if (c->visibilityState == VisibilityState::VISIBLE) {
        c->visibilityState = VisibilityState::SEEN;
      }

      auto f = state->fragments[index];
      if (c->visibilityState == VisibilityState::UNKNOWN) {
        if (!std::dynamic_pointer_cast<Unknown>(f)) {
          state->fragments[index] = std::make_shared<Unknown>();
        }
      } else if (c->visibilityState == VisibilityState::SEEN) {
        switch (c->type) {
        case CellType::UNKNOWN_CELL:
          if (!std::dynamic_pointer_cast<Unknown>(f)) {
            state->fragments[index] = std::make_shared<Unknown>();
          }
          break;
        case CellType::WALL:
          if (!std::dynamic_pointer_cast<WallSeen>(f)) {
            state->fragments[index] = std::make_shared<WallSeen>();
          }
          break;
        case CellType::FLOOR:
          if (!std::dynamic_pointer_cast<FloorSeen>(f)) {
            state->fragments[index] = std::make_shared<FloorSeen>();
          }
          break;
        }
      } else {
        switch (c->type) {
        case CellType::UNKNOWN_CELL:
          if (!std::dynamic_pointer_cast<Unknown>(f)) {
            state->fragments[index] = std::make_shared<Unknown>();
          }
          break;
        case CellType::WALL:
          if (!std::dynamic_pointer_cast<Wall>(f)) {
            state->fragments[index] = std::make_shared<Wall>();
          }
          break;
        case CellType::FLOOR:
          if (!std::dynamic_pointer_cast<Floor>(f)) {
            state->fragments[index] = std::make_shared<Floor>();
          }
          break;
        }
      }
      column++;
      index++;
    }
    auto f = state->fragments[index];
    if (f != State::END_LINE[0]) {
      state->fragments[index] = State::END_LINE[0];
    }
    index++;
    row++;
  }

  for (auto o : hero->currentLocation->objects) {
    auto ec = o->currentCell;
    if (!hero->canSee(ec))
      continue;
    auto index =
        ec->y * (hero->currentLocation->cells.front().size() + 1) + ec->x;

    if (auto e = std::dynamic_pointer_cast<Enemy>(o)) {
      state->fragments[index] = std::make_shared<EnemySign>();
    } else if (auto d = std::dynamic_pointer_cast<Door>(o)) {
      state->fragments[index] = std::make_shared<DoorSign>(d->opened);
    } else if (auto i = std::dynamic_pointer_cast<Item>(o)) {
      state->fragments[index] = std::make_shared<ItemSign>(i->type);
    }
  }

  auto hc = hero->currentCell;
  index = hc->y * (hero->currentLocation->cells.front().size() + 1) + hc->x;
  if (!std::dynamic_pointer_cast<HeroSign>(state->fragments[index])) {
    state->fragments[index] = std::make_shared<HeroSign>();
  }

  state->invalidate();
}

void LSSApp::mouseDown(MouseEvent event) {}

void LSSApp::keyDown(KeyEvent event) {

  modeManager.processKey(event);
  if (!modeManager.modeFlags->isInsert) {
    typedCommand = "";
  }

  if (modeManager.modeFlags->isNormal) {
    statusLine->setContent(State::normal_mode);
  } else if (modeManager.modeFlags->isHints) {
    hints->activated = true;
    statusLine->setContent(State::hints_mode);
  } else if (modeManager.modeFlags->isLeader) {
    statusLine->setContent(State::leader_mode);
  } else if (modeManager.modeFlags->isInsert) {
    if (event.getCode() != KeyEvent::KEY_SLASH &&
        event.getCode() != KeyEvent::KEY_RETURN &&
        event.getCode() != KeyEvent::KEY_BACKSPACE
    ) {
      typedCommand += event.getChar();
    } else if (event.getCode() == KeyEvent::KEY_BACKSPACE) {
      std::cout << typedCommand.length() << std::endl;
      if (typedCommand.length() > 0) {
        typedCommand.erase(typedCommand.length() - 1, typedCommand.length());
      } else {
        modeManager.toNormal();
        statusLine->setContent(State::normal_mode);
        typedCommand = "";
        return;
      }
    } else if (event.getCode() == KeyEvent::KEY_RETURN) {
      modeManager.toNormal();
      statusLine->setContent(State::normal_mode);
      processCommand(typedCommand);
      typedCommand = "";
      return;
    }
    if (typedCommand.length() == 0) {
      statusLine->setContent(State::insert_mode);
    } else {
      statusLine->setContent({State::insert_mode.front(), F(typedCommand)});
    }
    return;
  }

  switch (event.getCode()) {
  case KeyEvent::KEY_k:
    processCommand("move n");
    break;
  case KeyEvent::KEY_l:
    processCommand("e");
    break;
  case KeyEvent::KEY_j:
    processCommand("m s");
    break;
  case KeyEvent::KEY_h:
    processCommand("w");
    break;
  case KeyEvent::KEY_q:
    processCommand("q");
    break;
  case KeyEvent::KEY_p:
    processCommand("p");
    break;
  default:
    break;
  }
}

// TODO: to utils
std::vector<std::string> split_command(std::string strToSplit, char delimeter) {
  std::stringstream ss(strToSplit);
  std::string item;
  std::vector<std::string> splittedStrings;
  while (std::getline(ss, item, delimeter)) {
    splittedStrings.push_back(item);
  }
  return splittedStrings;
}

bool LSSApp::processCommand(std::string cmd) {
  // TODO: split, find command, create event, emit it and handle
  auto s = split_command(cmd, ' ').front();
  auto c = std::find_if(commands.begin(), commands.end(),
                        [s](std::shared_ptr<Command> c) {
                          return std::find(c->aliases.begin(), c->aliases.end(),
                                           s) != c->aliases.end();
                        });
  if (c == commands.end()) {
    statusLine->setContent(State::unknown_command);
    return false;
  }
  auto command = *c;
  if (auto e = dynamic_pointer_cast<MoveCommandEvent>(command->getEvent(cmd))) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<QuitCommandEvent>(
                 command->getEvent(cmd))) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<PickCommandEvent>(
                 command->getEvent(cmd))) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<DigCommandEvent>(
                 command->getEvent(cmd))) {
    eb::EventBus::FireEvent(*e);
  }
  invalidate();
  return true;
}

void LSSApp::update() {
  gl::clear(state->currentPalette.bgColor);
  gl::enableAlphaBlendingPremult();

  if (mPango != nullptr) {
    state->render(mPango);
  }

  if (statusFrame != nullptr) {

    statusFrame->setText(state->renderStatus());

    statusFrame->setDefaultTextFont(DEFAULT_FONT);
    statusFrame->render();
  }
}

void LSSApp::draw() {
  if (mPango != nullptr) {

    mPango->setDefaultTextColor(state->currentPalette.fgColor);
    mPango->setBackgroundColor(ColorA(0, 0, 0, 0));
    gl::draw(mPango->getTexture(), vec2(HOffset, VOffset));
  }
  if (statusFrame != nullptr) {
    gl::color(state->currentPalette.bgColorAlt);
    gl::drawSolidRect(Rectf(0, getWindowHeight() - StatusLine::HEIGHT,
                            getWindowWidth(), getWindowHeight()));
    gl::color(ColorA(1, 1, 1, 1));
    statusFrame->setDefaultTextColor(Color(state->currentPalette.fgColor));
    statusFrame->setBackgroundColor(ColorA(0, 0, 0, 0));
    gl::draw(statusFrame->getTexture(),
             vec2(6, getWindowHeight() - StatusLine::HEIGHT + 6));
  }
}

CINDER_APP(LSSApp, RendererGl)
