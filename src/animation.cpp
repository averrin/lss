#include "lss/animation.hpp"
//TODO: move animations to ui
#include "ui/fragment.hpp"
#include <fmt/format.h>

std::vector<std::shared_ptr<Cell>> MoveAnimation::tick() {
  std::vector<std::shared_ptr<Cell>> ret = {object->currentCell};
  counter++;
  // fmt::print("{}\n", counter);
  auto d = steps / path.size();
  if (counter < steps) {
    object->setCurrentCell(path.at(counter / d));
    ret.push_back(object->currentCell);
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
  return ret;
};

std::vector<std::shared_ptr<Cell>> ColorAnimation::tick() {
  std::vector<std::shared_ptr<Cell>> ret = {};
  if (object != nullptr) {
    ret.push_back(object->currentCell);
  } else {
    ret.push_back(cell);
  }
  counter++;
  if (counter < steps) {
    auto c = initColor;
    c.blend(targetColor, (1.f / steps) * counter);

    if (bg) {
      fragment->setBgColor(c.hex());
    } else {
      fragment->setFgColor(c.hex());
    }
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
      return ret;
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
  return ret;
}
