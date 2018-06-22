#ifndef __MODES_H_
#define __MODES_H_
#include <memory>
#include <functional>

#include "cinder/app/App.h"

using namespace ci;
using namespace ci::app;

#include <boost/sml.hpp>
namespace sml = boost::sml;

#include "lss/actions.hpp"
#include "lss/state.hpp"

class LSSApp;
struct modes {
  auto operator()() const noexcept {
    using namespace sml;
    auto is_hints = [](KeyPressedEvent e) {
      return e.key.getCode() == KeyEvent::KEY_f;
    };
    auto is_leader = [](KeyPressedEvent e) {
      return e.key.getCode() == KeyEvent::KEY_SPACE;
    };
    auto is_esc = [](KeyPressedEvent e) {
      return e.key.getCode() == KeyEvent::KEY_ESCAPE;
    };
    auto is_insert = [](KeyPressedEvent e) {
      return e.key.getCode() == KeyEvent::KEY_SLASH;
    };
    auto is_direction_event = [](EnableModeEvent e) {
      return e.mode == Modes::DIRECTION;
    };
    auto is_object_select_event = [](EnableModeEvent e) {
      return e.mode == Modes::OBJECTSELECT;
    };

    auto set_hints = [](std::shared_ptr<Modes> m) {
      std::cout << "Set HINTS mode" << std::endl;
      m->currentMode = Modes::HINTS;
    };
    auto set_leader = [](std::shared_ptr<Modes> m) {
      std::cout << "Set LEADER mode" << std::endl;
      m->currentMode = Modes::LEADER;
    };
    auto set_normal = [](std::shared_ptr<Modes> m) {
      std::cout << "Set NORMAL mode" << std::endl;
      m->currentMode = Modes::NORMAL;
    };
    auto set_insert = [](std::shared_ptr<Modes> m) {
      std::cout << "Set INSERT mode" << std::endl;
      m->currentMode = Modes::INSERT;
    };
    auto set_direction = [](std::shared_ptr<Modes> m) {
      std::cout << "Set DIRECTION mode" << std::endl;
      m->currentMode = Modes::DIRECTION;
    };
    auto set_object_select = [](std::shared_ptr<Modes> m) {
      std::cout << "Set OBJECTSELECT mode" << std::endl;
      m->currentMode = Modes::OBJECTSELECT;
    };

    // clang-format off
        return make_transition_table(
            * "normal"_s + event<KeyPressedEvent> [is_hints] / set_hints  = "hints"_s
            , "normal"_s + event<KeyPressedEvent> [is_leader] / set_leader  = "leader"_s
            , "normal"_s + event<KeyPressedEvent> [is_insert] / set_insert  = "insert"_s
            , "normal"_s + event<EnableModeEvent> [is_direction_event] / set_direction  = "direction"_s
            , "normal"_s + event<EnableModeEvent> [is_object_select_event] / set_object_select  = "object_select"_s

            , "hints"_s + event<KeyPressedEvent> [is_esc] / set_normal = "normal"_s
            , "leader"_s + event<KeyPressedEvent> [is_esc] / set_normal  = "normal"_s
            , "insert"_s + event<KeyPressedEvent> [is_esc] / set_normal  = "normal"_s
            , "direction"_s + event<KeyPressedEvent> [is_esc] / set_normal  = "normal"_s
            , "object_select"_s + event<KeyPressedEvent> [is_esc] / set_normal  = "normal"_s

            , "insert"_s + event<KeyPressedEvent> [is_insert] / set_normal  = "normal"_s

            , "hints"_s + event<ModeExitedEvent> / set_normal = "normal"_s
            , "leader"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
            , "insert"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
            , "direction"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
            , "object_select"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
        );
    // clang-format on
  }
};

class ModeManager {
public:
  ModeManager();
  void processKey(KeyEvent e);
  void processEvent(std::shared_ptr<LssEvent> e);
  void toNormal();
  void toDirection();
  void toObjectSelect();
  std::shared_ptr<Modes> modeFlags = std::make_shared<Modes>();

private:
  sml::sm<modes> state_machine = sml::sm<modes>(modeFlags);
};

class Mode {
public:
  Mode(LSSApp *);
  bool processKey(KeyEvent e);
  bool activated = false;

protected:
  LSSApp *app;
};

class HintsMode : public Mode {
public:
  HintsMode(LSSApp *app) : Mode(app){};
  bool processKey(KeyEvent e);
  void processEvent(std::shared_ptr<LssEvent> e);
};

class NormalMode : public Mode {
public:
  NormalMode(LSSApp *app) : Mode(app){};
  bool processKey(KeyEvent e);
};

class LeaderMode : public Mode {
  LeaderMode(LSSApp *app) : Mode(app){};
  bool processKey(KeyEvent e);
};

class DirectionMode : public Mode {
public:
  DirectionMode(LSSApp *app) : Mode(app){};
  bool processKey(KeyEvent e);
};

class InsertMode : public Mode {
public:
  InsertMode(LSSApp *app) : Mode(app){};
  bool processKey(KeyEvent e);
};

typedef std::function<std::string(std::shared_ptr<Object>)> Formatter;

class ObjectSelectMode : public Mode {
public:
  ObjectSelectMode(LSSApp *app) : Mode(app){};
  bool processKey(KeyEvent e);
  void setHeader(std::shared_ptr<Fragment> h) {
    header = h;
  }
  void setObjects(Objects o) {
    objects = o;
  }
  void setFormatter(Formatter f) {
    formatter = f;
  };

  void render(std::shared_ptr<State>);
    
  std::shared_ptr<Fragment> header;
  Objects objects;
  Formatter formatter;
};

#endif // __MODES_H_
