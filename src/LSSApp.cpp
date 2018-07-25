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
#include "lss/game/terrain.hpp"
#include "lss/utils.hpp"

std::string VERSION = "0.1.0 by Averrin";

int HOffset = 24;
int VOffset = 24;

void LSSApp::setup() {
  srand(time(NULL));
  // pango::Surface::setTextRenderer(pango::TextRenderer::FREETYPE);

  int rc = SDL_Init(SDL_INIT_VIDEO);
  assert(rc >= 0);
  (void) rc;

  if (SDL_GetDesktopDisplayMode(0, &dm))
  {
      printf("Error getting desktop display mode\n");
      return;
  }
  window = SDL_CreateWindow("Long Story Short", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  dm.w, dm.h, SDL_WINDOW_OPENGL);
  assert(window);
  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
  SDL_ShowCursor(false);
  SDL_ShowWindow(window);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_Log("Vendor     : %s\n", GL_NO_CHECK(GetString(GL_VENDOR)));
  SDL_Log("Renderer   : %s\n", GL_NO_CHECK(GetString(GL_RENDERER)));
  SDL_Log("Version    : %s\n", GL_NO_CHECK(GetString(GL_VERSION)));
  // SDL_Log("Extensions : %s\n", GL_NO_CHECK(GetString(GL_EXTENSIONS)));
  SDL_Log("Width      : %d\n", dm.w);
  SDL_Log("Height     : %d\n", dm.h);
  // SDL_Log("\n");


#ifdef _WIN32
        // Initialize GLES2 function table
        glFuncTable.initialize();
#endif

  /* Frames */
  gameFrame = pango::Surface::create();
  gameFrame->setMinSize(getWindowWidth(),
                        getWindowHeight() - StatusLine::HEIGHT);
  gameFrame->setMaxSize(getWindowWidth(),
                        getWindowHeight() - StatusLine::HEIGHT);
  gameFrame->disableWrap();

  statusFrame = pango::Surface::create();
  statusFrame->setMinSize(getWindowWidth(), StatusLine::HEIGHT);
  statusFrame->setMaxSize(getWindowWidth(), StatusLine::HEIGHT);

  heroFrame = pango::Surface::create();
  heroFrame->setMinSize(getWindowWidth(), HeroLine::HEIGHT);
  heroFrame->setMaxSize(getWindowWidth(), HeroLine::HEIGHT);

  inspectFrame = pango::Surface::create();
  inspectFrame->setMinSize(getWindowWidth() / 4.f - 12, getWindowHeight());
  inspectFrame->setMaxSize(getWindowWidth() / 4.f - 12, getWindowHeight());

  logFrame = pango::Surface::create();
  logFrame->setMinSize(getWindowWidth() / 4.f - 12, getWindowHeight());
  logFrame->setMaxSize(getWindowWidth() / 4.f - 12, getWindowHeight());

  initModes();
  startGame();
  setListeners();

  statusLine->setContent(State::normal_mode);

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

void LSSApp::initModes() {
  normalMode = std::make_shared<NormalMode>(this);
  directionMode = std::make_shared<DirectionMode>(this);
  insertMode = std::make_shared<InsertMode>(this);
  objectSelectMode = std::make_shared<ObjectSelectMode>(this);
  helpMode = std::make_shared<HelpMode>(this);
  gameOverMode = std::make_shared<GameOverMode>(this);
  inspectMode = std::make_shared<InspectMode>(this);
  inventoryMode = std::make_shared<InventoryMode>(this);
}

void LSSApp::initStates() {
  state = std::make_shared<State>();
  objectSelectState = std::make_shared<State>();
  helpState = std::make_shared<State>();
  gameOverState = std::make_shared<State>();
  statusState = std::make_shared<State>();
  heroState = std::make_shared<State>();
  inspectState = std::make_shared<State>();
  inventoryState = std::make_shared<State>();
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
}

void LSSApp::startGame() {
  initStates();

  hero = std::make_shared<Player>();
  heroLine = std::make_shared<HeroLine>(heroState, hero);
  logPanel = std::make_shared<LogPanel>(logState, hero);
  statusLine = std::make_shared<StatusLine>(statusState);

  auto l = generator->getRandomLocation(hero);
  l->depth = 0;
  locations[0] = l;

  for (auto n = 1; n < 26; n++) {
    threads.push_back(std::thread(
        [&](int n) {
          auto l = generator->getRandomLocation(hero, n);
          locations[n] = l;
          // MessageEvent me(nullptr, fmt::format("Generated location count:
          // {}", locations.size()));
          // eb::EventBus::FireEvent(me);
        },
        n));
  }

  state->clear();
  state->fragments.assign(
      l->cells.size() * (l->cells.front().size() + 1),
      std::make_shared<CellSign>(std::make_shared<Cell>(CellType::UNKNOWN)));

  hero->currentLocation = locations[0];
  hero->currentLocation->enter(hero, locations[0]->enterCell);

  invalidate("init");
  gameFrame->setTextAlignment(pango::TextAlignment::LEFT);
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
            [](auto a, auto b) { return a->zIndex < b->zIndex; });

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
    } else if (auto t = std::dynamic_pointer_cast<Terrain>(o);
               t && hero->canSee(ec)) {
      state->fragments[index] = std::make_shared<TerrainSign>(t->type);
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

void LSSApp::repeatTimer() {
  if (lastKeyEvent != std::nullopt) {
    std::thread([&]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      if (lastKeyEvent != std::nullopt) {
        repeatKeyEvent = true;
        repeatTimer();
      }
    }).detach();
  }
}

void LSSApp::keyUp(KeyEvent event) {
  repeatKeyEvent = false;
  lastKeyEvent = std::nullopt;
}

void LSSApp::keyDown(KeyEvent event) {
  lastKeyEvent = event;
  std::thread([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    if (lastKeyEvent != std::nullopt) {
      repeatTimer();
    }
  }).detach();

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
  if (auto e = std::dynamic_pointer_cast<MoveCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<QuitCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<PickCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<DigCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<WalkCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<AttackCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<EquipCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<HelpCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e =
                 std::dynamic_pointer_cast<InventoryCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<DropCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<WaitCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<ZapCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<StairEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<UseCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  }
  return true;
}

void LSSApp::update() {

  // gl::clear(state->currentPalette.bgColor);
  auto s = state;

  // gl::enableAlphaBlendingPremult();
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

  if (repeatKeyEvent && lastKeyEvent) {
    repeatKeyEvent = false;
    keyDown(*lastKeyEvent);
  }

  if (lastMode == modeManager.modeFlags->currentMode && !s->damaged) {
    return;
  }

  s->render(gameFrame);

  lastMode = modeManager.modeFlags->currentMode;
  needRedraw = true;
}

void LSSApp::draw() {

  std::cout << "draw" << std::endl;
  background = SDL_CreateTextureFromSurface(renderer, gameFrame->getTexture());
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, background, NULL, NULL);
  SDL_RenderPresent(renderer);
  // // if (!needRedraw) return;
  // // fmt::print(".");

  // gl::color(state->currentPalette.bgColor);
  // gl::drawSolidRect(
  //     Rectf(0, 0, getWindowWidth(), getWindowHeight() - StatusLine::HEIGHT));
  // gl::color(ColorA(1, 1, 1, 1));

  // switch (modeManager.modeFlags->currentMode) {
  // case Modes::INSPECT:
  // case Modes::NORMAL:
  // case Modes::INSERT:
  // case Modes::DIRECTION:
  // case Modes::HINTS:
  // case Modes::LEADER:
  // case Modes::HELP:
  // case Modes::OBJECTSELECT:
  // case Modes::INVENTORY:
  //   gameFrame->setTextAlignment(kp::pango::TextAlignment::LEFT);
  //   gl::draw(gameFrame->getTexture(), vec2(HOffset, VOffset));
  //   break;
  // case Modes::GAMEOVER:
  //   gameFrame->setTextAlignment(kp::pango::TextAlignment::CENTER);
  //   gl::draw(gameFrame->getTexture(), vec2(HOffset, VOffset));
  //   break;
  // }

  // if (statusFrame != nullptr) {
  //   gl::color(state->currentPalette.bgColorAlt);
  //   gl::drawSolidRect(Rectf(0, getWindowHeight() - StatusLine::HEIGHT,
  //                           getWindowWidth(), getWindowHeight()));
  //   gl::color(ColorA(1, 1, 1, 1));
  //   statusFrame->setBackgroundColor(ColorA(0, 0, 0, 0));
  //   gl::draw(statusFrame->getTexture(),
  //            vec2(6, getWindowHeight() - StatusLine::HEIGHT + 6));
  // }

  // if (logFrame != nullptr &&
  //     modeManager.modeFlags->currentMode != Modes::INSPECT) {
  //   gl::color(state->currentPalette.bgColorAlt);
  //   gl::drawSolidRect(Rectf(getWindowWidth() * 3.f / 4.f, 0,
  //   getWindowWidth(),
  //                           getWindowHeight()));
  //   gl::color(ColorA(1, 1, 1, 1));
  //   logFrame->setBackgroundColor(ColorA(0, 0, 0, 0));
  //   gl::draw(logFrame->getTexture(),
  //            vec2(getWindowWidth() * 3 / 4 + HOffset, VOffset));
  // }

  // if (inspectFrame != nullptr &&
  //     modeManager.modeFlags->currentMode == Modes::INSPECT) {
  //   gl::color(state->currentPalette.bgColorAlt);
  //   gl::drawSolidRect(Rectf(getWindowWidth() * 3.f / 4.f, 0,
  //   getWindowWidth(),
  //                           getWindowHeight()));
  //   gl::color(ColorA(1, 1, 1, 1));
  //   inspectFrame->setBackgroundColor(ColorA(0, 0, 0, 0));
  //   gl::draw(inspectFrame->getTexture(),
  //            vec2(getWindowWidth() * 3 / 4 + HOffset, VOffset));
  // }

  // if (heroFrame != nullptr) {
  //   gl::color(state->currentPalette.bgColor);
  //   gl::drawSolidRect(
  //       Rectf(0, getWindowHeight() - StatusLine::HEIGHT - HeroLine::HEIGHT,
  //             getWindowWidth(), getWindowHeight() - StatusLine::HEIGHT));
  //   gl::color(ColorA(1, 1, 1, 1));
  //   heroFrame->setBackgroundColor(ColorA(0, 0, 0, 0));
  //   gl::draw(
  //       heroFrame->getTexture(),
  //       vec2(6, getWindowHeight() - StatusLine::HEIGHT + 6 -
  //       HeroLine::HEIGHT));
  // }

  // gl::drawString(VERSION, vec2(getWindowWidth() - 120,
  //                              getWindowHeight() - StatusLine::HEIGHT + 12));
  // needRedraw = false;
}

LSSApp::~LSSApp() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}


int main(int argc, char *argv[]) {
  auto app = new LSSApp();
  app->setup();
    Uint32 lastTick = SDL_GetTicks();
    bool running = true;
    while (running) {
        Uint32 now = SDL_GetTicks();
        if (now - lastTick < 16) {
            SDL_Delay(16 - (now - lastTick));
            now = SDL_GetTicks();
        }
        lastTick = now;

        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                // case SDL_MOUSEBUTTONDOWN:
                // case SDL_MOUSEBUTTONUP: {
                //     if (event.button.button == SDL_BUTTON_LEFT) {
                //         view->m_rotationRateTarget = (event.type == SDL_MOUSEBUTTONDOWN) ? 0.f : 0.8f;
                //     }
                //     break;
                // }

                // case SDL_MOUSEMOTION: {
                //     if ((event.motion.state & SDL_BUTTON_LMASK) != 0) {
                //         view->m_yaw += event.motion.xrel * 0.01f;
                //         view->m_pitch += event.motion.yrel * 0.01f;
                //         view->m_pitch = std::max(std::min(view->m_pitch, Pi / 2), -Pi / 2);
                //     }
                //     break;
                // }

                case SDL_WINDOWEVENT: {
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        running = false;
                    }
                    break;
                }
            }
        }

        // Update the view
        if (running) {
            app->update();
            app->draw();
        }
    }

  delete app;
  return 0;
}
