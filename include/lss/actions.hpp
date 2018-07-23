#ifndef __ACTIONS_H_
#define __ACTIONS_H_
#include "cinder/app/App.h"
#include "lss/fragment.hpp"
#include "lss/palette.hpp"
#include <string>

using namespace ci;
using namespace ci::app;

struct Modes {
  enum ModeName {
    NORMAL,
    HINTS,
    LEADER,
    INSERT,
    DIRECTION,
    OBJECTSELECT,
    INVENTORY,
    HELP,
    GAMEOVER,
    INSPECT,
  };
  ModeName currentMode = ModeName::NORMAL;
};

struct SetContentEvent : LssEvent {
  Fragments content;
  SetContentEvent(Fragments c) : LssEvent(), content(c){};
};

struct AddContentEvent : LssEvent {
  Fragments content;
  AddContentEvent(Fragments c) : LssEvent(), content(c){};
};

struct KeyPressedEvent : LssEvent {
  KeyEvent key;
  KeyPressedEvent(KeyEvent k) : LssEvent(), key(k){};
};

struct ChangePaletteEvent : LssEvent {
  Palette palette;
  ChangePaletteEvent(Palette p) : LssEvent(), palette(p){};
};

struct ModeExitedEvent : LssEvent {
  ModeExitedEvent() : LssEvent(){};
};

struct EnableModeEvent : LssEvent {
  Modes::ModeName mode;
  EnableModeEvent(Modes::ModeName m) : LssEvent(), mode(m){};
};

#endif // __ACTIONS_H_
