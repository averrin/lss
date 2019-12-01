#include <chrono>
#include <cmath>
#include <iostream>
#include <iterator>
#include <ostream>
#include <stdio.h>
#include <string>
#include <utility>
#include <cstdlib>

#include "lss/LSSApp.hpp"
#include "lss/eventReactor.hpp"
#include "lss/game/terrain.hpp"
#include "lss/keyEvent.hpp"
#include "lss/utils.hpp"

std::string VERSION = "0.1.2 by Averrin";

int HOffset = 24;
int VOffset = 24;

void LSSApp::setup(int s) {
  seed = s;
  fmt::print("Seed: {}\n", seed);
  srand(seed);
  pango::Surface::setTextRenderer(pango::TextRenderer::FREETYPE);

  int rc = SDL_Init(SDL_INIT_VIDEO);
  assert(rc >= 0);
  (void)rc;

  if (SDL_GetDesktopDisplayMode(0, &dm)) {
    printf("Error getting desktop display mode\n");
    return;
  }
  gameWidth = dm.w * 3 / 4 - VOffset;
  gameHeight = dm.h - StatusLine::HEIGHT - HeroLine::HEIGHT - HOffset;

  window =
      SDL_CreateWindow("Long Story Short", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, dm.w, dm.h, SDL_WINDOW_OPENGL);
  assert(window);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
  SDL_ShowCursor(false);
  SDL_ShowWindow(window);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(renderer, dm.w, dm.h);

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
  gameFrame = pango::Surface::create(renderer);
  gameFrame->setMinSize(gameWidth, gameHeight);
  gameFrame->setMaxSize(gameWidth, gameHeight);
  gameFrame->disableWrap();

  gameGrid = std::make_shared<TextGrid>(renderer);
  gameGrid->setSize(gameWidth, gameHeight);

  statusFrame = pango::Surface::create(renderer);
  statusFrame->setMinSize(getWindowWidth(), StatusLine::HEIGHT);
  statusFrame->setMaxSize(getWindowWidth(), StatusLine::HEIGHT);

  heroFrame = pango::Surface::create(renderer);
  heroFrame->setMinSize(getWindowWidth(), HeroLine::HEIGHT);
  heroFrame->setMaxSize(getWindowWidth(), HeroLine::HEIGHT);

  inspectFrame = pango::Surface::create(renderer);
  inspectFrame->setMinSize(getWindowWidth() / 4.f - 12, getWindowHeight());
  inspectFrame->setMaxSize(getWindowWidth() / 4.f - 12, getWindowHeight());

  logFrame = pango::Surface::create(renderer);
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
  commands.push_back(std::make_shared<HeroCommand>());
  commands.push_back(std::make_shared<ThrowCommand>());
  commands.push_back(std::make_shared<LightCommand>());
}

void LSSApp::initModes() {
  normalMode = std::make_shared<NormalMode>(shared_from_this());
  directionMode = std::make_shared<DirectionMode>(shared_from_this());
  insertMode = std::make_shared<InsertMode>(shared_from_this());
  objectSelectMode = std::make_shared<ObjectSelectMode>(shared_from_this());
  helpMode = std::make_shared<HelpMode>(shared_from_this());
  gameOverMode = std::make_shared<GameOverMode>(shared_from_this());
  inspectMode = std::make_shared<InspectMode>(shared_from_this());
  inventoryMode = std::make_shared<InventoryMode>(shared_from_this());
  pauseMode = std::make_shared<PauseMode>(shared_from_this());
  targetMode = std::make_shared<TargetMode>(shared_from_this());
  heroMode = std::make_shared<HeroMode>(shared_from_this());
}

void LSSApp::initStates() {
  state = std::make_shared<State>();
  objectSelectState = std::make_shared<State>();
  helpState = std::make_shared<State>();
  heroLineState = std::make_shared<State>();
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
  heroLineState->currentPalette = palettes::DARK;
  inventoryState->currentPalette = palettes::DARK;
  gameOverState->currentPalette = palettes::DARK;
  heroState->currentPalette = palettes::DARK;
  inspectState->currentPalette = palettes::DARK;
  logState->currentPalette = palettes::DARK;
}

void LSSApp::startGame() {
  initStates();
  gameGrid->clear();

  bgRunning = true;
  hero = std::make_shared<Player>();
  magic = std::make_shared<Magic>(hero);
  heroLine = std::make_shared<HeroLine>(heroLineState, hero);
  logPanel = std::make_shared<LogPanel>(logState, hero);
  statusLine = std::make_shared<StatusLine>(statusState);

  auto l = generator->getRandomLocation(hero);
  l->depth = 0;
  locations[0] = l;
  l->enterCell->type = CellType::FLOOR;

  threads.push_back(std::thread(
      [&](std::shared_ptr<Location> l) {
        for (auto n = 1; n < MAX_LEVELS; n++) {
          l = generator->getRandomLocation(hero, n, l->exitCell);
          locations[n] = l;
          // MessageEvent me(nullptr, fmt::format("Generated location count:
          // {}", locations.size()));
          // eb::EventBus::FireEvent(me);
        }
        log.info("MAIN", lu::green("locations done"));
      },
      l));

  state->clear();
  state->fragments.assign(
      l->cells.size() * (l->cells.front().size() + 1),
      std::make_shared<CellSign>(std::make_shared<Cell>(CellType::UNKNOWN)));

  hero->currentLocation = locations[0];
  hero->currentLocation->enter(hero, locations[0]->enterCell);

  invalidate("init");
  gameFrame->setTextAlignment(pango::TextAlignment::LEFT);

  updateMap();
}

// TODO: fix crashes
void LSSApp::startBg() {
  bgRunning = false;
  if (bgThread.joinable()) {
    bgThread.join();
  }
  bgRunning = true;
  bgThread = std::thread([&]() { serveBg(); });
}

void LSSApp::frameBg() {
  // std::lock_guard<std::mutex> lock(exec_mutex);
  auto lightMap = hero->getLightMap();

  if (lightMap.size() > 0) {
    std::map<std::shared_ptr<Object>, int> ld;
    for (auto [ls, cells] : lightMap) {
      auto glow = ls->getGlow();
      if (!glow || glow->stable)
        continue;
      if (ld.find(ls) == ld.end()) {
        ld[ls] = R::N(0, 2);
      }
      auto d = ld[ls];
      for (auto c : cells) {
        auto cd = R::N(0, 1);
        auto f = state->fragments
                     [c->y * (hero->currentLocation->cells.front().size() + 1) +
                      c->x];
        auto a = f->alpha + d + cd;
        if (c->hasFeature(CellFeature::FROST)) {
          a = 100 - R::N(0, 5);
        }
        auto ml = 25 + 5 * c->lightSources.size();
        if (a < ml)
          a = ml;
        if (a > 100)
          a = 100;
        f->setAlpha(a);
      }
    }
    state->invalidate(); // illumination
  }

  playAnimations();
}

void LSSApp::serveBg() {
  while (bgRunning) {
    frameBg();
    SDL_Delay(32);
  }
}

void LSSApp::playAnimations() {
  std::vector<std::shared_ptr<Cell>> changed;
  auto _anims = animations;
  for (auto a : _anims) {
    if (a->stopped) {
      animations.erase(std::remove(animations.begin(), animations.end(), a));
      continue;
    }
    if (auto ca = std::dynamic_pointer_cast<ColorAnimation>(a)) {
      if (ca->cell == nullptr || (ca->object != nullptr && ca->object->currentCell == nullptr)) {
        animations.erase(std::remove(animations.begin(), animations.end(), a));
        continue;
      };
      auto c = ca->cell;
      auto f =
          state->fragments[c->x +
                           c->y * (hero->currentLocation->cells.front().size() +
                                   1)];
      if (ca->fragment == nullptr) {
        if (ca->bg) {
          ca->initColor = Color::fromString(f->bgColor);
        } else {
          ca->initColor = Color::fromString(f->fgColor);
        }
      }
      ca->fragment = f;
    } else if (auto ma = std::dynamic_pointer_cast<MoveAnimation>(a)) {
      hero->currentLocation->invalidate("move animation");
    }
    auto cells = a->tick();
    changed.insert(changed.end(), cells.begin(), cells.end());
  }
  if (changed.size()) {
    for (auto c : changed) {
      updateCell(c);
    }
    state->invalidate("animations");
  }
}

void LSSApp::setListeners() { reactor = std::make_shared<EventReactor>(this); }

void LSSApp::updateCell(std::shared_ptr<Cell> c) {
  if (!c->damaged && !state->select)
    return;
  auto objects = hero->currentLocation->getObjects(c);
  auto index = c->y * (hero->currentLocation->cells.front().size() + 1) + c->x;
  auto currentFragment = state->fragments[index];
  if (objects.size() == 0) {
    auto f = std::make_shared<CellSign>(c);
    switch (c->visibilityState) {
    case VisibilityState::UNKNOWN:
      if (hero->monsterSense &&
          !std::dynamic_pointer_cast<CellSign>(currentFragment)) {
        state->setFragment(index, f);
      }
      if (state->select) {
        if (debug) {
          state->setFragment(index, f);
        } else {
          state->setFragment(index, std::make_shared<CellSign>(std::make_shared<Cell>(CellType::UNKNOWN)));
        }
      } else {
        state->setFragment(index, state->US);
      }
      break;
    case VisibilityState::SEEN:
      state->setFragment(index, f);
      f->setAlpha(Cell::DEFAULT_LIGHT);
      break;
    case VisibilityState::VISIBLE:
      state->setFragment(index, f);
      f->setAlpha(c->illumination);
      break;
    }
  } else {
    std::sort(objects.begin(), objects.end(),
              [](auto a, auto b) { return a->zIndex < b->zIndex; });
    for (auto o : objects) {
      if (auto e = std::dynamic_pointer_cast<Enemy>(o)) {
        if (debug) {
          unsigned size = e->path.size();
          for (int k = 0; k < size; ++k) {
            auto dot = e->path[k];
            auto i =
                dot->y * (hero->currentLocation->cells.front().size() + 1) +
                dot->x;
            state->setFragment(i, std::make_shared<ItemSign>(c, ItemType::ROCK));
          }
        }

        if (!hero->canSee(c) && !hero->monsterSense)
          continue;

        auto f = std::make_shared<EnemySign>(c, e->type);
        state->setFragment(index, f);
        f->setAlpha(c->illumination);

      } else if (auto d = std::dynamic_pointer_cast<Door>(o);
                 d && (hero->canSee(c) ||
                       c->visibilityState == VisibilityState::SEEN)) {
        std::shared_ptr<Fragment> f = std::make_shared<DoorSign>(d->opened);
        if (!d->opened && d->hidden) {
          auto fake_wall = std::make_shared<Cell>(CellType::WALL);
          fake_wall->visibilityState = c->visibilityState;
          fake_wall->lightSources = c->lightSources;
          fake_wall->features = c->features;
          fake_wall->illuminated = c->illuminated;
          fake_wall->illumination = c->illumination;
          f = std::make_shared<CellSign>(fake_wall);
        }
        state->setFragment(index, f);
        f->setAlpha(c->illumination);
      } else if (auto i = std::dynamic_pointer_cast<Item>(o);
                 i && hero->canSee(c)) {
        auto f = std::make_shared<ItemSign>(c, i->type);
        state->setFragment(index, f);
        f->setAlpha(c->illumination);
      } else if (auto t = std::dynamic_pointer_cast<Terrain>(o);
                 t && hero->canSee(c)) {
        auto f = std::make_shared<TerrainSign>(c, t->type);
        state->setFragment(index, f);
        f->setAlpha(c->illumination);
        if (t->getGlow()) {
          f->setAlpha(100);
        }
      }
    }
  }

  if (c == hero->currentCell) {
    state->setFragment(index, std::make_shared<HeroSign>(c,
                                  !debug ? palettes::DARK.hero_color : "red"));
  }

  c->damaged = false;
}

void LSSApp::updateMap() {
  std::string label = "update map";
  log.start("MAIN", label);
  auto index = 0;
  auto cc = hero->currentCell;
  for (auto r : hero->currentLocation->cells) {
    for (auto c : r) {
      updateCell(c);
      index = c->y * (hero->currentLocation->cells.front().size() + 1) + c->x;
    }
    index++;
    auto f = state->fragments[index];
    if (f != State::END_LINE.front()) {
      state->setFragment(index, State::END_LINE.front());
    }
  }

  state->width = hero->currentLocation->cells.front().size();
  state->height = hero->currentLocation->cells.size();
  startBg();
  log.stop(label);
}

void LSSApp::repeatTimer() {
  if (lastKeyEvent != std::nullopt) {
    std::thread([&]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      if (lastKeyEvent != std::nullopt) {
        repeatKeyEvent = true;
        repeatTimer();
      }
    })
        .detach();
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
  })
      .detach();

  auto prevMode = modeManager.modeFlags->currentMode;
  modeManager.processKey(event);
  if (modeManager.modeFlags->currentMode != Modes::INSERT) {
    typedCommand = "";
  }
  if (modeManager.modeFlags->currentMode != prevMode) {
    state->invalidateSelection("mode exited");
    objectSelectMode->currentPage = 0;
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
  case Modes::PAUSE:
    pauseMode->processKey(event);
    break;
  case Modes::TARGET:
    targetMode->processKey(event);
    break;
  case Modes::HERO:
    heroMode->processKey(event);
    break;
  }

  if (modeManager.modeFlags->currentMode != prevMode) {
    statusLine->setModeLine(modeManager.modeFlags->currentMode);
  }
}

bool LSSApp::processCommand(std::string cmd) {

  // fmt::print("Command: {}\n", cmd);
  auto s = lu::split(cmd, ' ').front();
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

  auto label = fmt::format("cmd {}", lu::magenta(cmd));
  log.start("MAIN", label);

  std::lock_guard<std::mutex> lock(exec_mutex);
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
  } else if (auto e = std::dynamic_pointer_cast<HeroCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<ThrowCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = std::dynamic_pointer_cast<LightCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  }
  log.stop(label);

  state->invalidate("process command");
  return true;
}

void LSSApp::update() {
  // std::lock_guard<std::mutex> lock(exec_mutex);
  if (damaged) {
    bgRunning = false;
    bgThread.join();
    hero->currentLocation->invalidateVisibilityCache(hero->currentCell);
    if (hero->currentLocation->needUpdateLight) {
      hero->viewField = hero->calcViewField();
      hero->currentLocation->updateLight(hero);
      hero->currentLocation->updateView(hero);
    }
    updateMap();
    damaged = false;
  }

  // gl::clear(state->currentPalette.bgColor);
  auto s = state;
  // auto cc = hero->currentCell;
  // state->fragments[cc->y * (hero->currentLocation->cells.front().size() + 1)
  // +cc->x]->setAlpha(rand() % 101);

  // gl::enableAlphaBlendingPremult();
  gameFrame->setSpacing(0);

  switch (modeManager.modeFlags->currentMode) {
  case Modes::NORMAL:
    state->setSelect(false);
  case Modes::INSPECT:
  case Modes::PAUSE:
  case Modes::TARGET:
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
  case Modes::HERO:
    s = heroState;
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
    heroLineState->render(heroFrame);
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

  if (s != state) {
    s->render(gameFrame);
  } else {
    state->render(gameGrid);
  }

  lastMode = modeManager.modeFlags->currentMode;
  needRedraw = true;
}

void LSSApp::drawFrame(pango::SurfaceRef frame, SDL_Rect dst) {
  auto t = SDL_CreateTextureFromSurface(renderer, frame->getTexture());
  SDL_RenderCopyEx(renderer, t, NULL, &dst, 0, NULL, SDL_FLIP_VERTICAL);
  SDL_DestroyTexture(t);
  // frame->free();
}

void LSSApp::drawGrid(std::shared_ptr<TextGrid> frame, SDL_Rect dst) {
  auto t = SDL_CreateTextureFromSurface(renderer, frame->getTexture());
  SDL_RenderCopyEx(renderer, t, NULL, &dst, 0, NULL, SDL_FLIP_VERTICAL);
  // SDL_RenderCopy(renderer, t, NULL, &dst);
  SDL_DestroyTexture(t);
  // frame->free();
}

void LSSApp::draw() {
  if (!needRedraw) {
    return;
  }
  SDL_SetRenderDrawColor(renderer, 0x0d, 0x0f, 0x12, 255);
  SDL_RenderClear(renderer);

  // gl::color(state->currentPalette.bgColor);
  // gl::drawSolidRect(
  //     Rectf(0, 0, getWindowWidth(), getWindowHeight() - StatusLine::HEIGHT));
  // gl::color(ColorA(1, 1, 1, 1));
  SDL_Rect dstG = {HOffset, VOffset, gameWidth, gameHeight};
  switch (modeManager.modeFlags->currentMode) {
  case Modes::INSPECT:
  case Modes::PAUSE:
  case Modes::TARGET:
  case Modes::NORMAL:
  case Modes::INSERT:
  case Modes::DIRECTION:
    drawGrid(gameGrid, dstG);
    break;
  case Modes::HERO:
  case Modes::HELP:
  case Modes::OBJECTSELECT:
  case Modes::INVENTORY:
    gameFrame->setTextAlignment(pango::TextAlignment::LEFT);
    drawFrame(gameFrame, dstG);
    break;
  case Modes::GAMEOVER:
    gameFrame->setTextAlignment(pango::TextAlignment::CENTER);
    drawFrame(gameFrame, dstG);
    break;
  }

  if (statusFrame != nullptr) {
    drawFrame(statusFrame, {HOffset, getWindowHeight() - StatusLine::HEIGHT,
                            getWindowWidth() - HOffset, StatusLine::HEIGHT});
  }

  if (logFrame != nullptr &&
      modeManager.modeFlags->currentMode != Modes::INSPECT) {
    drawFrame(logFrame, {HOffset + gameWidth, 0, getWindowWidth() / 4,
                         getWindowHeight()});
  }

  if (inspectFrame != nullptr &&
      modeManager.modeFlags->currentMode == Modes::INSPECT) {
    drawFrame(inspectFrame, {HOffset + gameWidth, 0, getWindowWidth() / 4,
                             getWindowHeight()});
  }

  if (heroFrame != nullptr) {
    drawFrame(heroFrame,
              {HOffset,
               getWindowHeight() - StatusLine::HEIGHT - HeroLine::HEIGHT,
               getWindowWidth() - HOffset, HeroLine::HEIGHT});
  }

  needRedraw = false;
  SDL_RenderPresent(renderer);
  SDL_Delay(16);

  // gameFrame->free();
  statusFrame->free();
  logFrame->free();
  inspectFrame->free();
  heroFrame->free();
}

LSSApp::~LSSApp() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  bgRunning = false;
  bgThread.join();
  for (auto t = 0; t < threads.size(); t++) {
    threads[t].join();
  }
  gameGrid->free();
}

int main(int argc, char *argv[]) {
  auto app = std::make_shared<LSSApp>();
  auto seed = time(NULL);
  if (argc > 1) {
    seed = std::atoi(argv[1]);
  }
  app->setup(seed);
  Uint32 lastTick = SDL_GetTicks();
  while (app->running) {
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
      case SDL_QUIT:
        app->running = false;
        break;

      case SDL_KEYUP:
        app->keyUp(KeyEvent(event.key));
        break;
      case SDL_KEYDOWN:
        app->keyDown(KeyEvent(event.key));
        break;

      case SDL_WINDOWEVENT: {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
          app->running = false;
        }
        break;
      }
      }
      app->needRedraw = true;
    }

    // Update the view
    if (app->running) {

      auto lup = "update";
      auto ld = "draw";
      app->log.start("MAIN", lup, true);
      app->update();
      app->log.stop(lup, 60.f);
      app->log.start("MAIN", ld, true);
      app->draw();
      app->log.stop(ld, 50.f);
    }
  }

  return 0;
}
