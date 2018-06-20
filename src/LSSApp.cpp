#include <chrono>
#include <cmath>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <utility>

#include "lss/LSSApp.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

std::string VERSION = "0.0.4 by Averrin";

int HOffset = 24;
int VOffset = 24;
// std::string DEFAULT_FONT = "Iosevka 14";
std::string DEFAULT_FONT = "FiraCode 12";
auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

QuitCommandEvent::QuitCommandEvent() : CommandEvent(nullptr) {}
std::optional<std::shared_ptr<CommandEvent>> QuitCommand::getEvent(std::string s) {
  return std::make_shared<QuitCommandEvent>();
}

void LSSApp::onEvent(eb::Event &e) { invalidate(); }
void LSSApp::onEvent(QuitCommandEvent &e) { exit(0); }

void LSSApp::setup() {
  kp::pango::CinderPango::setTextRenderer(kp::pango::TextRenderer::FREETYPE);
  gameFrame = kp::pango::CinderPango::create();
  gameFrame->setMinSize(800, 600);
  gameFrame->setMaxSize(getWindowWidth(), getWindowHeight());
  gameFrame->setSpacing(gameFrame->getSpacing() - 4.0);

  statusFrame = kp::pango::CinderPango::create();
  statusFrame->setMinSize(getWindowWidth(), StatusLine::HEIGHT);
  statusFrame->setMaxSize(getWindowWidth(), StatusLine::HEIGHT);

  state = std::make_shared<State>();
  normalMode = std::make_shared<NormalMode>(this);
  directionMode = std::make_shared<DirectionMode>(this);
  insertMode = std::make_shared<InsertMode>(this);

  statusLine = std::make_shared<StatusLine>(state);
  statusLine->setContent(State::normal_mode);

  loadMap();
  invalidate();

  setListeners();

  commands.push_back(std::make_shared<MoveCommand>());
  commands.push_back(std::make_shared<QuitCommand>());
  commands.push_back(std::make_shared<PickCommand>());
  commands.push_back(std::make_shared<DigCommand>());
  commands.push_back(std::make_shared<WalkCommand>());
  commands.push_back(std::make_shared<AttackCommand>());
}

std::shared_ptr<Enemy> makeEnemy(std::shared_ptr<Cell> c, std::shared_ptr<Player> hero, EnemySpec type) {
    auto enemy = std::make_shared<Enemy>(type);
    enemy->currentCell = c;
    enemy->currentLocation = hero->currentLocation;
    enemy->registration = eb::EventBus::AddHandler<CommitEvent>(*enemy, hero);
    return enemy;
}

void LSSApp::loadMap() {
  hero = std::make_shared<Player>();
  hero->currentLocation = std::make_shared<Location>();
  hero->currentLocation->player = hero;

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
        case 'o':{
          c->type = CellType::FLOOR;
          c->passThrough = true;
          hero->currentLocation->objects.push_back(makeEnemy(c, hero, EnemyType::ORK));
          }break;
        case 'g':{
          c->type = CellType::FLOOR;
          c->passThrough = true;
          hero->currentLocation->objects.push_back(makeEnemy(c, hero, EnemyType::GOBLIN));
          }break;
        case 'p': {
          c->type = CellType::FLOOR;
          c->passThrough = true;
          hero->currentLocation->objects.push_back(makeEnemy(c, hero, EnemyType::PIXI));
          }break;
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
  hero->currentLocation->updateView(hero);

  auto axe = std::make_shared<Item>(ItemType::PICK_AXE);
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
  eb::EventBus::AddHandler<AttackCommandEvent>(*hero);

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
      state->fragments[index] = std::make_shared<EnemySign>(e->type);
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
  if (modeManager.modeFlags->currentMode != Modes::INSERT) {
    typedCommand = "";
  }
  
  switch (modeManager.modeFlags->currentMode) {
    case Modes::NORMAL:
      statusLine->setContent(State::normal_mode);
      normalMode->processKey(event);
      break;
    case Modes::DIRECTION:
      directionMode->processKey(event);
      break;
    case Modes::INSERT:
      insertMode->processKey(event);
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
  auto event = command->getEvent(cmd);
  if (event == std::nullopt) return false;

  //TODO: do it automagicaly
  if (auto e = dynamic_pointer_cast<MoveCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<QuitCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<PickCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<DigCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<WalkCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<AttackCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  }
  invalidate();
  return true;
}

void LSSApp::update() {
  gl::clear(state->currentPalette.bgColor);
  gl::enableAlphaBlendingPremult();

  if (gameFrame != nullptr) {
    state->render(gameFrame);
  }

  if (statusFrame != nullptr) {

    statusFrame->setText(state->renderStatus());

    statusFrame->setDefaultTextFont(DEFAULT_FONT);
    statusFrame->render();
  }
}

void LSSApp::draw() {
  if (gameFrame != nullptr) {

    gameFrame->setDefaultTextColor(state->currentPalette.fgColor);
    gameFrame->setBackgroundColor(ColorA(0, 0, 0, 0));
    gl::draw(gameFrame->getTexture(), vec2(HOffset, VOffset));
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
  gl::drawString(VERSION, vec2(getWindowWidth() - 120, getWindowHeight() - StatusLine::HEIGHT + 12));
}

CINDER_APP(LSSApp, RendererGl)
