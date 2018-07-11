#ifndef __RANDOMTOOLS_H_
#define __RANDOMTOOLS_H_
#include "fmt/format.h"
#include "rang.hpp"
#include <cstdlib>
#include <variant>

namespace R {
struct Float {
  float min;
  float max;
  float value;
  bool fixed = false;

  float roll() {
    value = rand() % int(max - min + 1) + min;
    fixed = true;
    return value;
  }

  // NOTE: i know, itn not real float
  operator float() {
    if (!fixed) {
      roll();
    }
    return value;
  }
};

struct Int {
  int min;
  int max;
  int value;
  bool fixed = false;

  int roll() {
    value = rand() % int(max - min + 1) + min;
    fixed = true;
    return value;
  }

  operator int() {
    if (!fixed) {
      roll();
    }
    return value;
  }
};

typedef std::variant<float, std::shared_ptr<R::Float>> rndFloat;
typedef std::variant<int, std::shared_ptr<R::Int>> rndInt;

rndFloat F(float min, float max);
rndFloat F(rndFloat);
rndInt I(int min, int max);
rndInt I(rndInt);
float get(rndFloat);
int get(rndInt);

} // namespace R

#endif // __RANDOMTOOLS_H_
