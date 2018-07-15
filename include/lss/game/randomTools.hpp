#ifndef __RANDOMTOOLS_H_
#define __RANDOMTOOLS_H_
#include <cstdlib>
#include <memory>
#include <variant>

namespace R {
float R(float min, float max);
float R();
int Z(int min, int max);
int Z();

struct Float {
  float min;
  float max;
  float value;
  bool fixed = false;

  float roll() {
    value = R(min, max);
    fixed = true;
    return value;
  }

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
    value = Z(min, max);
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
