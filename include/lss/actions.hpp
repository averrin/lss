#ifndef __ACTIONS_H_
#define __ACTIONS_H_
#include <string>
#include "cinder/app/App.h"
#include "lss/palette.hpp"
#include "lss/fragment.hpp"

using namespace ci;
using namespace ci::app;

struct SetContentEvent : LssEvent {
    Fragments content;
    SetContentEvent(Fragments c) : LssEvent(), content(c) {};
};

struct AddContentEvent : LssEvent {
    Fragments content;
    AddContentEvent(Fragments c) : LssEvent(), content(c) {};
};


struct KeyPressedEvent : LssEvent {
    KeyEvent key;
    KeyPressedEvent(KeyEvent k) : LssEvent(), key(k) {};
};

struct ChangePaletteEvent : LssEvent {
    Palette palette;
    ChangePaletteEvent(Palette p) : LssEvent(), palette(p) {};
};

struct ModeExitedEvent : LssEvent {
    ModeExitedEvent() : LssEvent() {};
};

#endif // __ACTIONS_H_
