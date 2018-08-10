#include "lss/game/randomTools.hpp"

std::random_device rd{};
std::mt19937 gen{rd()};

namespace R {
float R(float min, float max) {
  return min +
         static_cast<float>(rand()) /
             (static_cast<float>(RAND_MAX / (max - min)));
  ;
}
float R() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }

int Z(int min, int max) { return rand() % (max - min + 1) + min; }

int Z() { return rand() % 100; }

int N(int mean, int dev) {
  std::normal_distribution<> d{double(mean), double(dev)};

  return std::round(d(gen));
}

rndFloat F(float min, float max) {
  return std::make_shared<R::Float>(R::Float{min, max});
}

rndFloat F(rndFloat v) {
  if (std::holds_alternative<float>(v)) {
    return std::get<float>(v);
  }
  auto rv = std::get<std::shared_ptr<R::Float>>(v);
  return std::make_shared<R::Float>(R::Float{rv->min, rv->max});
}

rndInt I(int min, int max) {
  return std::make_shared<R::Int>(R::Int{min, max});
}

rndInt I(rndInt v) {
  if (std::holds_alternative<int>(v)) {
    return std::get<int>(v);
  }
  auto rv = std::get<std::shared_ptr<R::Int>>(v);
  return std::make_shared<R::Int>(R::Int{rv->min, rv->max});
}

float get(rndFloat v) {
  if (std::holds_alternative<float>(v)) {
    return std::get<float>(v);
  }
  return float(*std::get<std::shared_ptr<R::Float>>(v));
}

int get(rndInt v) {
  if (std::holds_alternative<int>(v)) {
    return std::get<int>(v);
  }
  return int(*std::get<std::shared_ptr<R::Int>>(v));
}

} // namespace R
