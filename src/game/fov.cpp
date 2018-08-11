#include <list>
#include <vector>

#include "lss/game/fov.hpp"

// Huge thanks to LordKelvin

namespace fov {
class ShadowLine {
public:
  std::vector<Shadow> _shadows;

  bool isInShadow(Shadow projection) {
    // Check the shadow list.
    for (auto shadow = _shadows.begin(); shadow != _shadows.end(); shadow++)
      if (shadow->contains(projection))
        return true;

    return false;
  }

  /// Add [shadow] to the list of non-overlapping shadows. May merge one or
  /// more shadows.
  void add(Shadow shadow) {
    // Figure out where to slot the new shadow in the sorted list.
    auto index = 0;
    for (; index < _shadows.size(); index++) {
      // Stop when we hit the insertion point.
      if (_shadows[index].start >= shadow.start)
        break;
    }
    auto overlappingPreviousSet =
        (index > 0 && _shadows[index - 1].end > shadow.start);
    auto overlappingNextSet =
        (index < _shadows.size() && _shadows[index].start < shadow.end);

    // Insert and unify with overlapping shadows.
    if (overlappingNextSet) {
      if (overlappingPreviousSet) {
        // Overlaps both, so unify one and delete the other.
        _shadows[index - 1].end = _shadows[index].end;
        _shadows.erase(_shadows.begin() + index);
      } else {
        // Only overlaps the next shadow, so unify it with that.
        _shadows[index].start = shadow.start;
      }
    } else {
      if (overlappingPreviousSet) {
        // Only overlaps the previous shadow, so unify it with that.
        _shadows[index - 1].end = shadow.end;
      } else {
        // Does not overlap anything, so insert.
        _shadows.insert(_shadows.begin() + index, shadow);
      }
    }
  }

  bool isFullShadow() {
    return _shadows.size() == 1 && _shadows[0].start == 0 &&
           _shadows[0].end == 1;
  }
};

Vec transformOctant(int row, int col, int octant) {
  switch (octant) {
  case 0:
    return Vec(col, -row);
  case 1:
    return Vec(row, -col);
  case 2:
    return Vec(row, col);
  case 3:
    return Vec(col, row);
  case 4:
    return Vec(-col, row);
  case 5:
    return Vec(-row, col);
  case 6:
    return Vec(-row, -col);
  case 7:
    return Vec(-col, -row);
  }

  throw "unreachable";
}

/// Creates a [Shadow] that corresponds to the projected silhouette of the
/// given tile. This is used both to determine visibility(if any of the
/// projection is visible, the tile is) and to add the tile to the shadow map.
///
/// The maximal projection of a square is always from the two opposing
/// corners. From the perspective of octant zero, we know the square is
/// above and to the right of the viewpoint, so it will be the top left and
/// bottom right corners.
Shadow _projectTile(int row, int col) {
  // The top edge of row 0 is 2 wide.
  auto topLeft = (double)col / (row + 2);

  // The bottom edge of row 0 is 1 wide.
  auto bottomRight = (double)(col + 1) / (row + 1);

  return Shadow(topLeft, bottomRight);
}

std::list<Vec> refreshOctant(Vec start, int octant, Vec bounds, Cells cells,
                             int maxRows = 999) {
  auto line = ShadowLine();
  auto fullShadow = false;
  std::list<Vec> result;

  // Sweep through the rows ('rows' may be vertical or horizontal based on
  // the incrementors). Start at row 1 to skip the center position.
  for (auto row = 1; row < maxRows; row++) {
    // If we've gone out of bounds, bail.
    auto pos = start + transformOctant(row, 0, octant);
    if (pos.x < 0 || pos.x >= bounds.x || pos.y < 0 || pos.y >= bounds.y)
      break;

    for (auto col = 0; col <= row; col++) {
      auto pos = start + transformOctant(row, col, octant);

      // If we've traversed out of bounds, bail on this row.
      // note: this improves performance, but works on the assumption that
      // the starting tile of the FOV is in bounds.
      if (pos.x < 0 || pos.x >= bounds.x || pos.y < 0 || pos.y >= bounds.y)
        break;

      // If we know the entire row is in shadow, we don't need to be more
      // specific.
      if (!fullShadow) {
        auto projection = _projectTile(row, col);

        // Set the visibility of this tile.
        auto visible = !line.isInShadow(projection);
        if (visible)
          result.push_back(pos);
        // else
        //    result.remove(pos);

        // Add any opaque tiles to the shadow map.
        if (visible && cells[pos.y][pos.x]->seeThrough == false) {
          line.add(projection);
          fullShadow = line.isFullShadow();
        }
      }
    }
  }

  return result;
}

/// Updates the visible flags in [stage] given the [Hero]'s [pos].
std::list<Vec> refresh(Vec pos, Vec bounds, Cells cells) {
  std::list<Vec> result;
  // Sweep through the octants.
  for (auto octant = 0; octant < 8; octant++) {
    std::list<Vec> oct = refreshOctant(pos, octant, bounds, cells);
    result.splice(result.end(), oct);
  }

  // The starting position is always visible.
  result.push_back(pos);

  result.sort([](const Vec a, const Vec b) {
    return (a.y < b.y || a.y == b.y) && a.x < b.x;
  });
  result.unique(
      [](const Vec a, const Vec b) { return a.x == b.x && a.y == b.y; });

  return result;
}
} // namespace fov
