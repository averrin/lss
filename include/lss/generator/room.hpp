#ifndef __ROOM_H_
#define __ROOM_H_
#include "lss/game/cell.hpp"

enum class RoomType {
  HALL,
  PASSAGE,
  CAVERN,
  CAVE,
};

enum class RoomFeature { CAVE, DUNGEON };

class Room {
public:
  Room(RoomType t, Cells c) : type(t), cells(c) {
    height = cells.size();
    width = cells.front().size();
  }
  Cells cells;
  RoomType type;
  int threat = 0;
  std::vector<RoomFeature> features;

  int height;
  int width;

  int x;
  int y;
};

#endif // __ROOM_H_
