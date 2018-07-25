#ifndef __LSSAPP_H_
#define __LSSAPP_H_

#include <chrono>
#include <cmath>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <thread>
#include <utility>

#include "lss/commands.hpp"
#include "lss/keyEvent.hpp"
#include "lss/modes.hpp"
#include "lss/pango.hpp"
#include "lss/state.hpp"

#include "lss/game/door.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/player.hpp"

#include "lss/ui/heroLine.hpp"
#include "lss/ui/logPanel.hpp"
#include "lss/ui/statusLine.hpp"
#include <fmt/format.h>

#include "lss/generator/generator.hpp"

#include "EventBus.hpp"
#include "EventHandler.hpp"
#include "rang.hpp"

class EventReactor;
class LSSApp {
public:
  float getWindowWidth() { return 800; }
  float getWindowHeight() { return 600; }
  void setup();
  void update();
  void keyDown(KeyEvent event);
  void keyUp(KeyEvent event);
  void draw();
  void invalidate();
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
  int currentLevel = 0;
  bool debug = false;

  pango::SurfaceRef gameFrame;
  pango::SurfaceRef statusFrame;
  pango::SurfaceRef heroFrame;
  pango::SurfaceRef inspectFrame;
  pango::SurfaceRef logFrame;

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

  std::shared_ptr<StatusLine> statusLine;
  std::shared_ptr<HeroLine> heroLine;
  std::shared_ptr<LogPanel> logPanel;

  std::shared_ptr<Generator> generator;

  std::shared_ptr<State> state;
  std::shared_ptr<State> statusState;
  std::shared_ptr<State> objectSelectState;
  std::shared_ptr<State> helpState;
  std::shared_ptr<State> inventoryState;
  std::shared_ptr<State> heroState;
  std::shared_ptr<State> gameOverState;
  std::shared_ptr<State> inspectState;
  std::shared_ptr<State> logState;

  std::shared_ptr<Player> hero;

  std::vector<std::shared_ptr<Command>> commands;

  std::string typedCommand;
  std::string pendingCommand;
  bool needRedraw = true;

  std::thread bg;
};

#endif // __LSSAPP_H_
