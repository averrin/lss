#include "lss/animation.hpp"
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

