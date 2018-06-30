#ifndef __CELL_H_
#define __CELL_H_
#include <memory>
#include <vector>

enum CellType {
  UNKNOWN_CELL,
  FLOOR,
  FLOOR_BLOOD,
  WALL,
  DOWNSTAIRS,
  UPSTAIRS,
};

enum VisibilityState { UNKNOWN, SEEN, VISIBLE };

class Cell {
public:
  Cell(int, int, CellType);
  CellType type;
  VisibilityState visibilityState;
  bool illuminated = false;

  int x;
  int y;

  bool passThrough = true;
  bool seeThrough = true;
};

typedef std::vector<std::shared_ptr<Cell>> CellRow;
typedef std::vector<CellRow> Cells;

#endif // __CELL_H_
