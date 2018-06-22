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

#include "fmt/format.h"
#include "lss/ui/statusLine.hpp"
#include "rang.hpp"

#include "EventBus.hpp"
#include "EventHandler.hpp"

using namespace ci;
using namespace ci::app;

class LSSApp : public App,
               public eb::EventHandler<eb::Event>,
               public eb::EventHandler<EquipCommandEvent>,
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

  kp::pango::CinderPangoRef gameFrame;
  kp::pango::CinderPangoRef statusFrame;
  kp::pango::CinderPangoRef objectSelectFrame;

  ModeManager modeManager = ModeManager();
  std::shared_ptr<NormalMode> normalMode;
  std::shared_ptr<DirectionMode> directionMode;
  std::shared_ptr<InsertMode> insertMode;
  std::shared_ptr<ObjectSelectMode> objectSelectMode;

  std::shared_ptr<StatusLine> statusLine;
  std::shared_ptr<State> state;
  std::shared_ptr<State> statusState;
  std::shared_ptr<State> objectSelectState;
  std::shared_ptr<Player> hero;

  std::vector<std::shared_ptr<Command>> commands;

  std::string typedCommand;
  std::string pendingCommand;

  virtual void onEvent(eb::Event &e) override;
  virtual void onEvent(QuitCommandEvent &e) override;
  virtual void onEvent(EquipCommandEvent &e) override;
};

#endif // __LSSAPP_H_
