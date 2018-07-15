#ifndef __ROOM_H_
#define __ROOM_H_
#include "lss/game/cell.hpp"

enum class RoomType {
  HALL,
  PASSAGE,
  CAVERN,
  CAVE,
};

enum class RoomFeature { CAVE, DUNGEON, STATUE, ALTAR };

class Room {
public:
  Room(RoomType t, Cells c) : type(t) {
    height = c.size();
    width = c.front().size();
    for (auto r : c) {
      for (auto cell : r) {
        if (cell->type == CellType::UNKNOWN)
          continue;
        cells.push_back(cell);
      }
    }
  }
  std::vector<std::shared_ptr<Cell>> cells;
  RoomType type;
  int threat = 0;
  std::vector<RoomFeature> features;

  int height;
  int width;

  int x;
  int y;
};

#endif // __ROOM_H_
