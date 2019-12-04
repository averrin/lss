#ifndef __UI_UTILS_H_
#define __UI_UTILS_H_

#include <optional>
#include <SDL.h>

class ui_utils {
public:
  static std::optional<std::string> getDir(int code) {
    switch (code) {
    case SDL_SCANCODE_J:
      return "s"s;
    case SDL_SCANCODE_H:
      return "w"s;
    case SDL_SCANCODE_L:
      return "e"s;
    case SDL_SCANCODE_K:
      return "n"s;
    case SDL_SCANCODE_Y:
      return "nw"s;
    case SDL_SCANCODE_U:
      return "ne"s;
    case SDL_SCANCODE_B:
      return "sw"s;
    case SDL_SCANCODE_N:
      return "se"s;
    }
    return std::nullopt;
  }


};


#endif // __UI_UTILS_H_
