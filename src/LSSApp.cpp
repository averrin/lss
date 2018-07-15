#include <chrono>
#include <cmath>
#include <iostream>
#include <iterator>
#include <ostream>
#include <stdio.h>
#include <string>
#include <utility>

#include "lss/LSSApp.hpp"
#include "lss/eventReactor.hpp"
#include "lss/utils.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

std::string VERSION = "0.0.8 by Averrin";

int HOffset = 24;
int VOffset = 24;

void LSSApp::setup() {
  srand(time(NULL));
  kp::pango::CinderPango::setTextRenderer(kp::pango::TextRenderer::FREETYPE);

  /* Frames */
  gameFrame = kp::pango::CinderPango::create();
  gameFrame->setMinSize(800, 600);
  gameFrame->setMaxSize(getWindowWidth(),
                        getWindowHeight() - StatusLine::HEIGHT);
  gameFrame->disableWrap();

  statusFrame = kp::pango::CinderPango::create();
  statusFrame->setMinSize(getWindowWidth(), StatusLine::HEIGHT);
  statusFrame->setMaxSize(getWindowWidth(), StatusLine::HEIGHT);

  heroFrame = kp::pango::CinderPango::create();
  heroFrame->setMinSize(getWindowWidth(), HeroLine::HEIGHT);
  heroFrame->setMaxSize(getWindowWidth(), HeroLine::HEIGHT);

  inspectFrame = kp::pango::CinderPango::create();
  inspectFrame->setMinSize(getWindowWidth() / 4.f - 12, getWindowHeight());
  inspectFrame->setMaxSize(getWindowWidth() / 4.f - 12, getWindowHeight());

  logFrame = kp::pango::CinderPango::create();
  logFrame->setMinSize(getWindowWidth() / 4.f - 12, getWindowHeight());
  logFrame->setMaxSize(getWindowWidth() / 4.f - 12, getWindowHeight());

  /* Modes && States */
  normalMode = std::make_shared<NormalMode>(this);
  state = std::make_shared<State>();

  directionMode = std::make_shared<DirectionMode>(this);
  insertMode = std::make_shared<InsertMode>(this);

  objectSelectMode = std::make_shared<ObjectSelectMode>(this);
  objectSelectState = std::make_shared<State>();

  helpMode = std::make_shared<HelpMode>(this);
  helpState = std::make_shared<State>();

  inventoryMode = std::make_shared<InventoryMode>(this);
  inventoryState = std::make_shared<State>();

  gameOverMode = std::make_shared<GameOverMode>(this);
  gameOverState = std::make_shared<State>();

  statusState = std::make_shared<State>();
  statusLine = std::make_shared<StatusLine>(statusState);

  heroState = std::make_shared<State>();

  inspectState = std::make_shared<State>();
  inspectMode = std::make_shared<InspectMode>(this);

  logState = std::make_shared<State>();

  state->currentPalette = palettes::DARK;
  statusState->currentPalette = palettes::DARK;
  objectSelectState->currentPalette = palettes::DARK;
  helpState->currentPalette = palettes::DARK;
  inventoryState->currentPalette = palettes::DARK;
  gameOverState->currentPalette = palettes::DARK;
  heroState->currentPalette = palettes::DARK;
  inspectState->currentPalette = palettes::DARK;
  logState->currentPalette = palettes::DARK;

  statusLine->setContent(State::normal_mode);

  hero = std::make_shared<Player>();
  heroLine = std::make_shared<HeroLine>(heroState, hero);
  logPanel = std::make_shared<LogPanel>(logState, hero);

  auto l = generator->getRandomLocation(hero);
  l->depth = 0;
  locations.push_back(l);

  hero->currentLocation = locations.front();
  hero->currentLocation->enter(hero, locations.front()->enterCell);

  state->fragments.assign(
      hero->currentLocation->cells.size() *
          (hero->currentLocation->cells.front().size() + 1),
      std::make_shared<CellSign>(std::make_shared<Cell>(CellType::UNKNOWN)));

  // hero->commit("init", 0);

  invalidate("init");
  setListeners();

  /* Commands */
  commands.push_back(std::make_shared<MoveCommand>());
  commands.push_back(std::make_shared<QuitCommand>());
  commands.push_back(std::make_shared<PickCommand>());
  commands.push_back(std::make_shared<DigCommand>());
  commands.push_back(std::make_shared<WalkCommand>());
  commands.push_back(std::make_shared<AttackCommand>());
  commands.push_back(std::make_shared<EquipCommand>());
  commands.push_back(std::make_shared<HelpCommand>());
  commands.push_back(std::make_shared<InventoryCommand>());
  commands.push_back(std::make_shared<DropCommand>());
  commands.push_back(std::make_shared<WaitCommand>());
  commands.push_back(std::make_shared<ZapCommand>());
  commands.push_back(std::make_shared<UpCommand>());
  commands.push_back(std::make_shared<DownCommand>());
  commands.push_back(std::make_shared<UseCommand>());
}

void LSSApp::setListeners() { reactor = std::make_shared<EventReactor>(this); }

void LSSApp::invalidate() {
  auto row = 0;
  auto index = 0;
  auto cc = hero->currentCell;
  for (auto r : hero->currentLocation->cells) {
    auto column = 0;
    for (auto c : r) {
      // if (debug) {
      //   c->features.clear();
      // }
      auto f = state->fragments[index];
      // fmt::print("{}", c->type);
      switch (c->visibilityState) {
      case VisibilityState::UNKNOWN:
        if (hero->monsterSense &&
            !std::dynamic_pointer_cast<CellSign>(state->fragments[index])) {
          state->fragments[index] = std::make_shared<CellSign>(c);
        }
        break;
      case VisibilityState::SEEN:
        state->fragments[index] = std::make_shared<CellSign>(c);
        break;
      case VisibilityState::VISIBLE:
        // if (!c->seeThrough) c->features = {CellFeature::MARK1};
        state->fragments[index] = std::make_shared<CellSign>(c);
        break;
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
    // fmt::print("\n");
  }

  auto objects = hero->currentLocation->objects;
  std::sort(objects.begin(), objects.end(),
            [](std::shared_ptr<Object> a, std::shared_ptr<Object> b) {
              return std::dynamic_pointer_cast<Creature>(b) &&
                     !std::dynamic_pointer_cast<Creature>(a);
            });

  for (auto o : objects) {
    auto ec = o->currentCell;
    auto index =
        ec->y * (hero->currentLocation->cells.front().size() + 1) + ec->x;

    if (auto e = std::dynamic_pointer_cast<Enemy>(o)) {
      if (debug) {
        // for (auto dot : e->viewField) {
        //   if (dot == e->currentCell)
        //     continue;
        //   auto i = dot->y * (hero->currentLocation->cells.front().size() + 1)
        //   +
        //            dot->x;
        //   dot->visibilityState = VisibilityState::VISIBLE;
        //   dot->features = {CellFeature::BLOOD};
        //   state->fragments[i] = std::make_shared<CellSign>(dot);
        // }
        unsigned size = e->path.size();
        for (int k = 0; k < size; ++k) {
          auto ptr = e->path[k];
          auto dot = static_cast<Cell *>(ptr);
          auto i = dot->y * (hero->currentLocation->cells.front().size() + 1) +
                   dot->x;
          state->fragments[i] = std::make_shared<ItemSign>(ItemType::ROCK);
        }
      }

      if (!hero->canSee(ec) && !hero->monsterSense)
        continue;

      state->fragments[index] = std::make_shared<EnemySign>(e->type);

    } else if (auto d = std::dynamic_pointer_cast<Door>(o);
               d && hero->canSee(ec)) {
      state->fragments[index] = std::make_shared<DoorSign>(d->opened);
    } else if (auto i = std::dynamic_pointer_cast<Item>(o);
               i && hero->canSee(ec)) {
      state->fragments[index] = std::make_shared<ItemSign>(i->type);
    } else if (auto t = std::dynamic_pointer_cast<TorchStand>(o);
               t && hero->canSee(ec)) {
      state->fragments[index] = std::make_shared<ItemSign>(t->type);
    }
  }

  auto hc = hero->currentCell;
  index = hc->y * (hero->currentLocation->cells.front().size() + 1) + hc->x;
  if (!std::dynamic_pointer_cast<HeroSign>(state->fragments[index])) {
    state->fragments[index] =
        std::make_shared<HeroSign>(!debug ? palettes::DARK.hero_color : "red");
  }

  state->width = hero->currentLocation->cells.front().size();
  state->height = hero->currentLocation->cells.size();
  state->invalidate();
}

void LSSApp::mouseDown(MouseEvent event) {}

void LSSApp::keyDown(KeyEvent event) {
  auto prevMode = modeManager.modeFlags->currentMode;
  modeManager.processKey(event);
  if (modeManager.modeFlags->currentMode != Modes::INSERT) {
    typedCommand = "";
  }
  if (modeManager.modeFlags->currentMode != prevMode) {
    statusLine->setModeLine(modeManager.modeFlags->currentMode);
    return;
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
  case Modes::OBJECTSELECT:
    objectSelectMode->processKey(event);
    break;
  case Modes::GAMEOVER:
    gameOverMode->processKey(event);
    break;
  case Modes::INSPECT:
    inspectMode->processKey(event);
    break;
  }
}

bool LSSApp::processCommand(std::string cmd) {
  // fmt::print("Command: {}\n", cmd);
  auto s = utils::split(cmd, ' ').front();
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
  if (event == std::nullopt)
    return false;

  // TODO: do it automagicaly
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
  } else if (auto e = dynamic_pointer_cast<EquipCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<HelpCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<InventoryCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<DropCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<WaitCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<ZapCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<StairEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<UseCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  }
  return true;
}

void LSSApp::update() {

  // gl::clear(state->currentPalette.bgColor);
  auto s = state;

  gl::enableAlphaBlendingPremult();
  gameFrame->setSpacing(0);

  switch (modeManager.modeFlags->currentMode) {
  case Modes::NORMAL:
    state->setSelect(false);
  case Modes::INSPECT:
  case Modes::INSERT:
  case Modes::DIRECTION:
  case Modes::HINTS:
  case Modes::LEADER:
    gameFrame->setSpacing(-5.0);
    break;
  case Modes::OBJECTSELECT:
    s = objectSelectState;
    break;
  case Modes::HELP:
    s = helpState;
    break;
  case Modes::INVENTORY:
    s = inventoryState;
    break;
  case Modes::GAMEOVER:
    s = gameOverState;
    break;
  }

  if (statusFrame != nullptr) {
    statusState->render(statusFrame);
  }
  if (heroFrame != nullptr) {
    heroState->render(heroFrame);
  }
  if (inspectFrame != nullptr) {
    inspectState->render(inspectFrame);
  }
  if (logFrame != nullptr) {
    logState->render(logFrame);
  }

  if (lastMode == modeManager.modeFlags->currentMode && !s->damaged) {
    return;
  }

  s->render(gameFrame);

  lastMode = modeManager.modeFlags->currentMode;
  needRedraw = true;
}

void LSSApp::draw() {
  // if (!needRedraw) return;
  // fmt::print(".");

  gl::color(state->currentPalette.bgColor);
  gl::drawSolidRect(
      Rectf(0, 0, getWindowWidth(), getWindowHeight() - StatusLine::HEIGHT));
  gl::color(ColorA(1, 1, 1, 1));

  switch (modeManager.modeFlags->currentMode) {
  case Modes::INSPECT:
  case Modes::NORMAL:
  case Modes::INSERT:
  case Modes::DIRECTION:
  case Modes::HINTS:
  case Modes::LEADER:
  case Modes::HELP:
  case Modes::OBJECTSELECT:
  case Modes::INVENTORY:
    gl::draw(gameFrame->getTexture(), vec2(HOffset, VOffset));
    break;
  case Modes::GAMEOVER:
    gameFrame->setTextAlignment(kp::pango::TextAlignment::CENTER);
    gl::draw(gameFrame->getTexture(), vec2(HOffset, VOffset));
    break;
  }

  if (statusFrame != nullptr) {
    gl::color(state->currentPalette.bgColorAlt);
    gl::drawSolidRect(Rectf(0, getWindowHeight() - StatusLine::HEIGHT,
                            getWindowWidth(), getWindowHeight()));
    gl::color(ColorA(1, 1, 1, 1));
    statusFrame->setBackgroundColor(ColorA(0, 0, 0, 0));
    gl::draw(statusFrame->getTexture(),
             vec2(6, getWindowHeight() - StatusLine::HEIGHT + 6));
  }

  if (logFrame != nullptr &&
      modeManager.modeFlags->currentMode != Modes::INSPECT) {
    gl::color(state->currentPalette.bgColorAlt);
    gl::drawSolidRect(Rectf(getWindowWidth() * 3.f / 4.f, 0, getWindowWidth(),
                            getWindowHeight()));
    gl::color(ColorA(1, 1, 1, 1));
    logFrame->setBackgroundColor(ColorA(0, 0, 0, 0));
    gl::draw(logFrame->getTexture(),
             vec2(getWindowWidth() * 3 / 4 + HOffset, VOffset));
  }

  if (inspectFrame != nullptr &&
      modeManager.modeFlags->currentMode == Modes::INSPECT) {
    gl::color(state->currentPalette.bgColorAlt);
    gl::drawSolidRect(Rectf(getWindowWidth() * 3.f / 4.f, 0, getWindowWidth(),
                            getWindowHeight()));
    gl::color(ColorA(1, 1, 1, 1));
    inspectFrame->setBackgroundColor(ColorA(0, 0, 0, 0));
    gl::draw(inspectFrame->getTexture(),
             vec2(getWindowWidth() * 3 / 4 + HOffset, VOffset));
  }

  if (heroFrame != nullptr) {
    gl::color(state->currentPalette.bgColor);
    gl::drawSolidRect(
        Rectf(0, getWindowHeight() - StatusLine::HEIGHT - HeroLine::HEIGHT,
              getWindowWidth(), getWindowHeight() - StatusLine::HEIGHT));
    gl::color(ColorA(1, 1, 1, 1));
    heroFrame->setBackgroundColor(ColorA(0, 0, 0, 0));
    gl::draw(
        heroFrame->getTexture(),
        vec2(6, getWindowHeight() - StatusLine::HEIGHT + 6 - HeroLine::HEIGHT));
  }

  gl::drawString(VERSION, vec2(getWindowWidth() - 120,
                               getWindowHeight() - StatusLine::HEIGHT + 12));
  needRedraw = false;
}

CINDER_APP(LSSApp, RendererGl)
