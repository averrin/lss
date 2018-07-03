#ifndef __CELL_H_
#define __CELL_H_
#include <algorithm>
#include <memory>
#include <vector>

struct CellSpec {
  std::string name;

  bool passThrough = false;
  bool seeThrough = false;

  friend bool operator==(CellSpec &t1, const CellSpec &t2) {
    return t1.name == t2.name;
  }
  friend bool operator!=(CellSpec &t1, const CellSpec &t2) {
    return t1.name != t2.name;
  }
  friend bool operator<(const CellSpec &t1, const CellSpec &t2) {
    return t1.name < t2.name;
  }
};

namespace CellType {
const CellSpec UNKNOWN_CELL = CellSpec{"unknown", false, false};
const CellSpec FLOOR = CellSpec{"floor", true, true};
const CellSpec WALL = CellSpec{"wall", false, false};
const CellSpec DOWNSTAIRS = CellSpec{"downstairs", true, true};
const CellSpec UPSTAIRS = CellSpec{"upstairs", true, false};
};

enum VisibilityState { UNKNOWN, SEEN, VISIBLE };
enum CellFeature { BLOOD, CAVE };

class Cell {
public:
  Cell(CellSpec t) : type(t) {}
  Cell(int _x, int _y, CellSpec t)
      : x(_x), y(_y), type(t), passThrough(type.passThrough),
        seeThrough(type.seeThrough) {}
  Cell(int _x, int _y, CellSpec t, std::vector<CellFeature> f)
      : x(_x), y(_y), type(t), features(f), passThrough(type.passThrough),
        seeThrough(type.seeThrough) {}
  CellSpec type;
  VisibilityState visibilityState = VisibilityState::UNKNOWN;
  bool illuminated = false;
  std::vector<CellFeature> features;

  int x;
  int y;

  bool passThrough = false;
  bool seeThrough = false;

  bool hasFeature(CellFeature f) {
    return std::find(features.begin(), features.end(), f) != features.end();
  }
};

typedef std::vector<std::shared_ptr<Cell>> CellRow;
typedef std::vector<CellRow> Cells;

#endif // __CELL_H_
