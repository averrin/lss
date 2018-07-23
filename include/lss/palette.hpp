#pragma once
#ifndef __PALETTE_H_
#define __PALETTE_H_
#include <string>

struct Palette {
  std::string name;
  ci::Color fgColor;
  ci::Color bgColor;
  ci::Color bgColorAlt;

  std::string green;
  std::string red;
  std::string blue;
  std::string orange;

  std::string floor_color;
  std::string wall_color;
  std::string hero_color;

  std::string floor_color_seen;
  std::string wall_color_seen;
};

class palettes {
public:
  static const Palette DARK;
  static const Palette LIGHT;
};

#endif // __PALETTE_H_
