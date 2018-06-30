#ifndef __FOV_H_
#define __FOV_H_
#include "lss/game/cell.hpp"
#include <list>
#include <vector>

namespace fov {

class Vec {
public:
  int x, y;

  Vec(int in_x = 0, int in_y = 0) {
    x = in_x;
    y = in_y;
  }

  Vec operator+(const Vec &a) const { return Vec(a.x + x, a.y + y); }

  bool operator==(const Vec &rhs) { return rhs.x == x && rhs.y == y; }
};

std::list<Vec> refresh(Vec, Vec, Cells);

class Shadow {
public:
  double start, end;

  Shadow() {}

  Shadow(double in_start, double in_end) {
    start = in_start;
    end = in_end;
  }

  /// Returns `true` if [other] is completely covered by this shadow.
  bool contains(Shadow other) {
    return start <= other.start && end >= other.end;
  }
};
} // namespace fov
#endif // __FOV_H_
