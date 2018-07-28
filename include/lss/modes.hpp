#ifndef __MODES_H_
#define __MODES_H_
#include <functional>
#include <memory>

#include <boost/sml.hpp>
namespace sml = boost::sml;

#include "lss/actions.hpp"
#include "lss/keyEvent.hpp"
#include "lss/state.hpp"

#include <SDL.h>

// TODO: add exit method for modes;
class LSSApp;
struct modes {
  auto operator()() const noexcept {
    using namespace sml;
    auto is_hints = [](KeyPressedEvent e) {
      return false;
      // return e.key.getCode() == SDL_SCANCODE_f;
    };
    auto is_leader = [](KeyPressedEvent e) {
      return false;
      // return e.key.getCode() == SDL_SCANCODE_SPACE;
    };
    auto is_esc = [](KeyPressedEvent e) {
      return e.key.getCode() == SDL_SCANCODE_ESCAPE;
    };
    auto is_esc_or_z = [](KeyPressedEvent e) {
      return e.key.getCode() == SDL_SCANCODE_ESCAPE ||
             e.key.getCode() == SDL_SCANCODE_Z;
    };
    auto is_insert = [](KeyPressedEvent e) {
      return e.key.getCode() == SDL_SCANCODE_SLASH && !e.key.isShiftDown();
    };
    auto is_direction_event = [](EnableModeEvent e) {
      return e.mode == Modes::DIRECTION;
    };
    auto is_object_select_event = [](EnableModeEvent e) {
      return e.mode == Modes::OBJECTSELECT;
    };
    auto is_inventory_event = [](EnableModeEvent e) {
      return e.mode == Modes::INVENTORY;
    };
    auto is_help_event = [](EnableModeEvent e) {
      return e.mode == Modes::HELP;
    };
    auto is_go_event = [](EnableModeEvent e) {
      return e.mode == Modes::GAMEOVER;
    };
    auto is_inspect_event = [](EnableModeEvent e) {
      return e.mode == Modes::INSPECT;
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
    auto set_inventory = [](std::shared_ptr<Modes> m) {
      std::cout << "Set INVENTORY mode" << std::endl;
      m->currentMode = Modes::INVENTORY;
    };
    auto set_help = [](std::shared_ptr<Modes> m) {
      std::cout << "Set HELP mode" << std::endl;
      m->currentMode = Modes::HELP;
    };
    auto set_go = [](std::shared_ptr<Modes> m) {
      std::cout << "Set GAMEOVER mode" << std::endl;
      m->currentMode = Modes::GAMEOVER;
    };
    auto set_inspect = [](std::shared_ptr<Modes> m) {
      std::cout << "Set INSPECT mode" << std::endl;
      m->currentMode = Modes::INSPECT;
    };

    // clang-format off
        return make_transition_table(
            * "normal"_s + event<KeyPressedEvent> [is_hints] / set_hints  = "hints"_s
            , "normal"_s + event<KeyPressedEvent> [is_leader] / set_leader  = "leader"_s
            , "normal"_s + event<KeyPressedEvent> [is_insert] / set_insert  = "insert"_s
            , "normal"_s + event<EnableModeEvent> [is_direction_event] / set_direction  = "direction"_s
            , "normal"_s + event<EnableModeEvent> [is_object_select_event] / set_object_select  = "object_select"_s
            , "normal"_s + event<EnableModeEvent> [is_inventory_event] / set_inventory  = "inventory"_s
            , "normal"_s + event<EnableModeEvent> [is_help_event] / set_help  = "inventory"_s
            , "normal"_s + event<EnableModeEvent> [is_go_event] / set_go  = "game_over"_s
            , "normal"_s + event<EnableModeEvent> [is_inspect_event] / set_inspect  = "inspect"_s

            , "hints"_s + event<KeyPressedEvent> [is_esc] / set_normal = "normal"_s
            , "leader"_s + event<KeyPressedEvent> [is_esc] / set_normal  = "normal"_s
            , "insert"_s + event<KeyPressedEvent> [is_esc] / set_normal  = "normal"_s
            , "direction"_s + event<KeyPressedEvent> [is_esc] / set_normal  = "normal"_s
            , "object_select"_s + event<KeyPressedEvent> [is_esc_or_z] / set_normal  = "normal"_s
            , "inventory"_s + event<KeyPressedEvent> [is_esc_or_z] / set_normal  = "normal"_s
            , "help"_s + event<KeyPressedEvent> [is_esc] / set_normal  = "normal"_s
            , "inspect"_s + event<KeyPressedEvent> [is_esc_or_z] / set_normal  = "normal"_s

            , "insert"_s + event<KeyPressedEvent> [is_insert] / set_normal  = "normal"_s

            , "hints"_s + event<ModeExitedEvent> / set_normal = "normal"_s
            , "leader"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
            , "insert"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
            , "direction"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
            , "object_select"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
            , "inventory"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
            , "help"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
            , "inspect"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
            , "game_over"_s + event<ModeExitedEvent> / set_normal  = "normal"_s
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
  void toHelp();
  void toInventory();
  void toDirection();
  void toGameOver();
  void toObjectSelect();
  void toInspect();
  std::shared_ptr<Modes> modeFlags = std::make_shared<Modes>();

private:
  sml::sm<modes> state_machine = sml::sm<modes>(modeFlags);
};

class Mode {
public:
  Mode(LSSApp *);
  bool processKey(KeyEvent e) { return false; };
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

class InspectMode : public Mode {
public:
  InspectMode(LSSApp *app) : Mode(app){};
  bool processKey(KeyEvent e);
  void render();
};

typedef std::function<std::string(std::shared_ptr<Object>, std::string)>
    Formatter;
typedef std::function<bool(std::shared_ptr<Object>)> SelectCallback;

class TextMode : public Mode {
public:
  TextMode(LSSApp *app) : Mode(app){};
  void setHeader(std::shared_ptr<Fragment> h) { header = h; }
  std::shared_ptr<Fragment> header;
  bool processKey(KeyEvent e);
  virtual void render(std::shared_ptr<State>) = 0;
};

class ObjectSelectMode : public TextMode {
public:
  ObjectSelectMode(LSSApp *app) : TextMode(app){};
  bool processKey(KeyEvent e);

  void setObjects(Objects o) { objects = o; }
  Objects objects;
  void setFormatter(Formatter f) { formatter = f; };
  Formatter formatter;

  void setCallback(SelectCallback c) { callback = c; };
  SelectCallback callback;

  void render(std::shared_ptr<State>);
};

class InventoryMode : public TextMode {
public:
  InventoryMode(LSSApp *app) : TextMode(app){};
  void render(std::shared_ptr<State>);

  void setObjects(Objects o) { objects = o; }
  Objects objects;
};

class HelpMode : public TextMode {
public:
  HelpMode(LSSApp *app) : TextMode(app){};
  void render(std::shared_ptr<State>);
};

class GameOverMode : public TextMode {
public:
  GameOverMode(LSSApp *app) : TextMode(app){};
  void render(std::shared_ptr<State>);
  bool processKey(KeyEvent e);
};
#endif // __MODES_H_
