#include "lss/animation.hpp"
#include "lss/fragment.hpp"
#include <fmt/format.h>

int MoveAnimation::tick() {
  counter++;
  // fmt::print("{}\n", counter);
  auto d = steps / path.size();
  if (counter < steps) {
    object->currentCell = path.at(counter / d);
    if (frameCallback != nullptr) {
      frameCallback(counter);
    }
  } else if (counter == steps) {
    if (endless) {
      counter = 0;
    } else {
      stopped = true;
    }
    if (animationCallback != nullptr) {
      animationCallback();
    }
  }
  return counter;
};

int ColorAnimation::tick() {
  counter++;
  if (counter < steps) {
    auto c = initColor;
    c.blend(targetColor, (1.f / steps) * counter);
    fragment->setFgColor(c.getString());
    if (frameCallback != nullptr) {
      frameCallback(counter);
    }
  } else if (counter == steps) {
    if (pulse && !wayback) {
      counter = 0;
      auto t = targetColor;
      targetColor = initColor;
      initColor = t;
      wayback = true;
      return counter;
    }
    if (endless) {
      counter = 0;
    } else {
      stopped = true;
    }
    if (animationCallback != nullptr) {
      animationCallback();
    }
  }
  return counter;
}
