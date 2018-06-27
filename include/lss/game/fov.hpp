#ifndef __FOV_H_
#define __FOV_H_
#include <list>
#include <vector>
#include "lss/game/cell.hpp"

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

  Vec startPos, endPos;

  Shadow() {}

  Shadow(double in_start, double in_end, Vec in_startPos, Vec in_endPos) {
    start = in_start;
    end = in_end;
    startPos = in_startPos;
    endPos = in_endPos;
  }

  /// Returns `true` if [other] is completely covered by this shadow.
  bool contains(Shadow other) {
    return start <= other.start && end >= other.end;
  }
};
} // namespace fov
#endif // __FOV_H_
