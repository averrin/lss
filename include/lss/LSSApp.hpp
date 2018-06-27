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
#include <utility>
#include <any>

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
#include "lss/game/player.hpp"

#include "fmt/format.h"
#include "lss/ui/heroLine.hpp"
#include "lss/ui/statusLine.hpp"
#include "rang.hpp"

#include "EventBus.hpp"
#include "EventHandler.hpp"

using namespace ci;
using namespace ci::app;

class EventReactor;
class LSSApp : public App {
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

  kp::pango::CinderPangoRef gameFrame;
  kp::pango::CinderPangoRef statusFrame;
  kp::pango::CinderPangoRef objectSelectFrame;
  kp::pango::CinderPangoRef helpFrame;
  kp::pango::CinderPangoRef inventoryFrame;
  kp::pango::CinderPangoRef heroFrame;

  ModeManager modeManager = ModeManager();
  std::shared_ptr<EventReactor> reactor;

  std::shared_ptr<NormalMode> normalMode;
  std::shared_ptr<DirectionMode> directionMode;
  std::shared_ptr<InsertMode> insertMode;
  std::shared_ptr<ObjectSelectMode> objectSelectMode;
  std::shared_ptr<HelpMode> helpMode;
  std::shared_ptr<InventoryMode> inventoryMode;

  std::shared_ptr<StatusLine> statusLine;
  std::shared_ptr<HeroLine> heroLine;

  std::shared_ptr<State> state;
  std::shared_ptr<State> statusState;
  std::shared_ptr<State> objectSelectState;
  std::shared_ptr<State> helpState;
  std::shared_ptr<State> inventoryState;
  std::shared_ptr<State> heroState;

  std::shared_ptr<Player> hero;

  std::vector<std::any> commands;

  std::string typedCommand;
  std::string pendingCommand;
};

#endif // __LSSAPP_H_
