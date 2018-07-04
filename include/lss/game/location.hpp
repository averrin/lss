#ifndef __LOCATION_H_
#define __LOCATION_H_
#include "lss/game/cell.hpp"
#include "lss/game/events.hpp"
#include "lss/game/fov.hpp"
#include "lss/game/object.hpp"
#include "micropather/micropather.h"
#include <cmath>

#include "EventHandler.hpp"
#include "EventRegisttration.hpp"

class Player;
class Room;
class Location : public micropather::Graph,
                 public eb::EventHandler<EnemyDiedEvent>,
                 public eb::EventHandler<ItemTakenEvent>,
                 public eb::EventHandler<DigEvent>,
                 public eb::EventHandler<DropEvent>,
                 public eb::EventHandler<CommitEvent>,
                 public eb::EventHandler<EnterCellEvent> {
public:
  Location();
  Cells cells;
  Objects objects;
  std::shared_ptr<Player> player;
  int depth = 0;

  std::shared_ptr<Cell> enterCell;
  std::shared_ptr<Cell> exitCell;
  std::vector<std::shared_ptr<Room>> rooms;

  void updateView(std::shared_ptr<Player>);
  void updateLight(std::shared_ptr<Player>);
  void reveal();
  void enter(std::shared_ptr<Player>, std::shared_ptr<Cell>);
  void leave(std::shared_ptr<Player>);
  Objects getObjects(std::shared_ptr<Cell>);
  std::vector<eb::HandlerRegistrationPtr> handlers;

  std::vector<std::shared_ptr<Cell>> getNeighbors(std::shared_ptr<Cell> cell) {
    std::vector<std::shared_ptr<Cell>> nbrs;
    if (cell->x > 0) {
      if (cell->y > 0) {
        nbrs.push_back(cells[cell->y - 1][cell->x - 1]);
        nbrs.push_back(cells[cell->y - 1][cell->x]);
        nbrs.push_back(cells[cell->y][cell->x - 1]);

        if (cell->x < cells.front().size() - 1) {
          nbrs.push_back(cells[cell->y - 1][cell->x + 1]);
          nbrs.push_back(cells[cell->y][cell->x + 1]);
        }
      }
      if (cell->y < cells.size() - 1) {
        if (cell->x < cells.front().size() - 1) {
          nbrs.push_back(cells[cell->y + 1][cell->x + 1]);
          nbrs.push_back(cells[cell->y + 1][cell->x - 1]);
        }
        nbrs.push_back(cells[cell->y + 1][cell->x]);
      }
    }
    return nbrs;
  }

  virtual void onEvent(EnemyDiedEvent &e) override;
  virtual void onEvent(ItemTakenEvent &e) override;
  virtual void onEvent(EnterCellEvent &e) override;
  virtual void onEvent(DigEvent &e) override;
  virtual void onEvent(DropEvent &e) override;
  virtual void onEvent(CommitEvent &e) override;

  float LeastCostEstimate(void *stateStart, void *stateEnd) override;
  void AdjacentCost(void *state,
                    MP_VECTOR<micropather::StateCost> *adjacent) override;
  void PrintStateInfo(void *state) override;

  std::vector<std::shared_ptr<Cell>> getVisible(std::shared_ptr<Cell> start,
                                                float distance) {
    std::vector<std::shared_ptr<Cell>> temp;
    std::vector<std::shared_ptr<Cell>> result;
    fov::Vec heroPoint{start->x, start->y};
    fov::Vec bounds{(int)cells.front().size(), (int)cells.size()};
    auto field = fov::refresh(heroPoint, bounds, cells);
    for (auto v : field) {
      auto c = cells[v.y][v.x];
      temp.push_back(c);
    }
    for (auto c : temp) {
      auto d = sqrt(pow(start->x - c->x, 2) + pow(start->y - c->y, 2));
      if (c->illuminated || d <= distance) {
        result.push_back(c);
      }
    }
    return result;
  }
};

#endif // __LOCATION_H_
