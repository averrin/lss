#ifndef __MAPUTILS_H_
#define __MAPUTILS_H_

#include "lss/game/location.hpp"
#include "lss/game/enemy.hpp"

class mapUtils {
public:
    static Cells fill(int h, int w, CellSpec type) {
        Cells cells;
        for (auto r = 0; r < h; r++) {
            std::vector<std::shared_ptr<Cell>> row;
            for (auto c = 0; c < w; c++) {
            auto cell = std::make_shared<Cell>(c, r, type);
            cell->visibilityState = VisibilityState::UNKNOWN;
            if (type == CellType::FLOOR) {
                cell->seeThrough = true;
                cell->passThrough = true;
            }

            row.push_back(cell);
            }

            cells.push_back(row);
        }
        return cells;
    }

//TODO: move to Room class
static void paste(std::vector<std::shared_ptr<Cell>> room,
           std::shared_ptr<Location> location, int x, int y) {
  for (auto cell : room) {
    if (cell->type == CellType::UNKNOWN)
      continue;
    auto r = y + cell->y;
    auto c = x + cell->x;
    location->cells.at(r).at(c) = cell;
    cell->x = c;
    cell->y = r;
  }
}

static void paste(Cells room, std::shared_ptr<Location> location, int x, int y) {
  int bh = location->cells.size();
  int bw = location->cells.front().size();
  for (auto r = y; r < int(room.size() + y) && r < bh; r++) {
    for (auto c = x; c < int(room.front().size() + x) && c < bw; c++) {
      auto cell = room[r - y][c - x];
      if (cell->type == CellType::UNKNOWN)
        continue;
      location->cells[r][c] = cell;
      location->cells[r][c]->x = c;
      location->cells[r][c]->y = r;
    }
  }
}

static std::shared_ptr<Enemy> makeEnemy(std::shared_ptr<Location> location,
                                 std::shared_ptr<Cell> c, EnemySpec type) {
  if (c == nullptr) {
    throw std::runtime_error("nullptr cell for enemy");
  }
  auto enemy = std::make_shared<Enemy>(type);
  enemy->setCurrentCell(c);
  enemy->currentLocation = location;
  return enemy;
}


};
#endif // __MAPUTILS_H_
