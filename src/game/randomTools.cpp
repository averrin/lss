#include "lss/game/randomTools.hpp"

namespace R {
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
