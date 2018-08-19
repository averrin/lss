#ifndef __LSSAPP_H_
#define __LSSAPP_H_
#include <config.hpp>

#include <chrono>
#include <cmath>
#include <iostream>
#include <iterator>
#include <mutex>
#include <ostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <thread>
#include <utility>

#include "gl2/Context.h"
#include <SDL.h>
#include <SDLPango.hpp>
#include <assert.h>
#include <fmt/format.h>
#include <rang.hpp>

#include "lss/animation.hpp"
#include "lss/commands.hpp"
#include "lss/keyEvent.hpp"
#include "lss/modes.hpp"
#include "lss/state.hpp"

#include "lss/game/door.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/magic.hpp"
#include "lss/game/player.hpp"

#include "lss/ui/heroLine.hpp"
#include "lss/ui/inspectMode.hpp"
#include "lss/ui/logPanel.hpp"
#include "lss/ui/normalMode.hpp"
#include "lss/ui/statusLine.hpp"

#include "lss/generator/generator.hpp"

#include "EventBus.hpp"
#include "EventHandler.hpp"

class EventReactor;
class LSSApp {
public:
  static const int MAX_LEVELS = 10;

  ~LSSApp();
  SDL_DisplayMode dm;
  int getWindowWidth() { return dm.w; }
  int getWindowHeight() { return dm.h; }
  void setup();
  void update();
  void keyDown(KeyEvent event);
  void keyUp(KeyEvent event);
  void draw();
  bool running = true;
  bool bgRunning = true;
  void startBg();
  void serveBg();
  void frameBg();
  void playAnimations();

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  int gameWidth, gameHeight;
  std::mutex exec_mutex;

  pango::SurfaceRef gameFrame;
  pango::SurfaceRef statusFrame;
  pango::SurfaceRef heroFrame;
  pango::SurfaceRef inspectFrame;
  pango::SurfaceRef logFrame;

  void drawFrame(pango::SurfaceRef, SDL_Rect);

  void updateMap();
  void invalidate() { damaged = true; }
  void invalidate(std::string reason) {
    // fmt::print("Invalidate reason: {}\n", reason);
    auto t0 = std::chrono::system_clock::now();
    invalidate();
    auto t1 = std::chrono::system_clock::now();
    using milliseconds = std::chrono::duration<double, std::milli>;
    milliseconds ms = t1 - t0;
    // std::cout << "invalidate: " << rang::fg::green << ms.count()
    // << rang::style::reset << '\n';
  }
  bool processCommand(std::string);
  void setListeners();
  void startGame();
  void initModes();
  void initStates();

  std::optional<KeyEvent> lastKeyEvent;
  bool repeatKeyEvent = false;
  void repeatTimer();

  // std::map<std::string, std::shared_ptr<Location>> locations;
  std::shared_ptr<Location> loadMap(std::string);
  std::map<int, std::shared_ptr<Location>> locations;
  std::vector<std::thread> threads;
  std::thread bgThread;
  int currentLevel = 0;
  bool debug = false;

  ModeManager modeManager = ModeManager();
  std::shared_ptr<EventReactor> reactor;

  Modes::ModeName lastMode;

  std::shared_ptr<NormalMode> normalMode;
  std::shared_ptr<DirectionMode> directionMode;
  std::shared_ptr<InsertMode> insertMode;
  std::shared_ptr<ObjectSelectMode> objectSelectMode;
  std::shared_ptr<HelpMode> helpMode;
  std::shared_ptr<InventoryMode> inventoryMode;
  std::shared_ptr<GameOverMode> gameOverMode;
  std::shared_ptr<InspectMode> inspectMode;
  std::shared_ptr<PauseMode> pauseMode;
  std::shared_ptr<TargetMode> targetMode;
  std::shared_ptr<HeroMode> heroMode;

  std::shared_ptr<StatusLine> statusLine;
  std::shared_ptr<HeroLine> heroLine;
  std::shared_ptr<LogPanel> logPanel;

  std::shared_ptr<Generator> generator;

  std::shared_ptr<State> state;
  std::shared_ptr<State> statusState;
  std::shared_ptr<State> objectSelectState;
  std::shared_ptr<State> helpState;
  std::shared_ptr<State> heroLineState;
  std::shared_ptr<State> inventoryState;
  std::shared_ptr<State> heroState;
  std::shared_ptr<State> gameOverState;
  std::shared_ptr<State> inspectState;
  std::shared_ptr<State> logState;

  std::shared_ptr<Player> hero;
  std::shared_ptr<Magic> magic;

  std::vector<std::shared_ptr<Command>> commands;
  std::vector<std::shared_ptr<Animation>> animations;

  std::string typedCommand;
  bool needRedraw = true;
  bool damaged = false;
};

#endif // __LSSAPP_H_
