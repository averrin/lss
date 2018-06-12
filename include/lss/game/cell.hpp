#ifndef __CELL_H_
#define __CELL_H_
#include <vector>
#include <memory>

enum CellType {
    FLOOR,
    WALL
};

enum VisibilityState {
    UNKNOWN,
    SEEN,
    VISIBLE
};

class Cell {
public:
    Cell(int, int, CellType);
    CellType type;
    VisibilityState visibilityState;

    int x;
    int y;

    bool passThrough = true;
    bool seeThrough = true;
};

typedef std::vector<std::shared_ptr<Cell>> CellRow;
typedef std::vector<CellRow> Cells;

#endif // __CELL_H_
