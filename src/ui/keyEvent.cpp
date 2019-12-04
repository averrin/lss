#include "ui/keyEvent.hpp"

SDL_Scancode KeyEvent::getCode() { return event.keysym.scancode; }
char KeyEvent::getChar() { return event.keysym.sym; }
bool KeyEvent::isShiftDown() { return mods & KMOD_SHIFT; }
