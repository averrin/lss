#ifndef __KEYEVENT_H_
#define __KEYEVENT_H_
#include <SDL.h>

// typedef SDL_KeyboardEvent KeyEvent;

class KeyEvent {
public:
KeyEvent(SDL_KeyboardEvent e) : event(e) {
    mods = SDL_GetModState();
}
  SDL_KeyboardEvent event;
  SDL_Keymod mods;

  SDL_Scancode getCode();
  char getChar();
  bool isShiftDown();
};

#endif // __KEYEVENT_H_
