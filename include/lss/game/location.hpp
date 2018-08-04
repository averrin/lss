#ifndef __LOCATION_H_
#define __LOCATION_H_

#include "lss/game/cell.hpp"
#include "lss/game/events.hpp"
#include "lss/game/fov.hpp"
#include "lss/game/object.hpp"
#include "micropather/micropather.h"
#include <cmath>
#include <iostream>
#include <map>

enum LocationFeature {
  CAVE_PASSAGE,
  RIVER,
  TORCHES,
  STATUE,
  ALTAR,
  LAKE,
  VOID
};

enum LocationType { DUNGEON, CAVERN };

struct LocationSpec {
  std::string name;
  int threat = 0;
  LocationType type = LocationType::DUNGEON;
  std::vector<LocationFeature> features;
  std::vector<CellFeature> cellFeatures;
};

class Player;
class Room;
class Location : public Object,
                 public micropather::Graph,
                 public eb::EventHandler<EnemyDiedEvent>,
                 public eb::EventHandler<ItemTakenEvent>,
                 public eb::EventHandler<DigEvent>,
                 public eb::EventHandler<DropEvent>,
                 public eb::EventHandler<CommitEvent>,
                 public eb::EventHandler<DoorOpenedEvent>,
                 public eb::EventHandler<LeaveCellEvent>,
                 public eb::EventHandler<EnterCellEvent> {
public:
  Location(LocationSpec t) : Object(), type(t), features(t.features) {}
  ~Location();
  LocationSpec type;
  Cells cells;
  Objects objects{};
  std::shared_ptr<Player> player;
  int depth = 0;

  std::shared_ptr<Cell> enterCell;
  std::shared_ptr<Cell> exitCell;
  std::vector<std::shared_ptr<Room>> rooms;

  std::vector<LocationFeature> features;
  bool hasFeature(LocationFeature f) {
    return std::find(features.begin(), features.end(), f) != features.end();
  }

  void updateView(std::shared_ptr<Player>);
  void updateLight(std::shared_ptr<Player>);
  void reveal();
  void enter(std::shared_ptr<Player>, std::shared_ptr<Cell>);
  void leave(std::shared_ptr<Player>);
  Objects getObjects(std::shared_ptr<Cell>);
  std::string getFeaturesTag() {
    std::string locationFeatures = "_______";
    if (hasFeature(LocationFeature::TORCHES)) {
      locationFeatures[0] = 'T';
    }
    if (hasFeature(LocationFeature::CAVE_PASSAGE)) {
      locationFeatures[1] = 'C';
    }
    if (hasFeature(LocationFeature::RIVER)) {
      locationFeatures[2] = 'R';
    }
    if (hasFeature(LocationFeature::STATUE)) {
      locationFeatures[3] = 'S';
    }
    if (hasFeature(LocationFeature::ALTAR)) {
      locationFeatures[4] = 'A';
    }
    if (hasFeature(LocationFeature::VOID)) {
      locationFeatures[5] = 'V';
    }
    if (hasFeature(LocationFeature::LAKE)) {
      locationFeatures[6] = 'L';
    }
    return locationFeatures;
  }

  std::vector<std::shared_ptr<Cell>> getNeighbors(std::shared_ptr<Cell> cell) {
    return getNeighbors(cell.get());
  }
  std::vector<std::shared_ptr<Cell>> getNeighbors(Cell *cell) {
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
  std::vector<std::shared_ptr<Cell>> getVisible(std::shared_ptr<Cell> start,
                                                float distance);

  void invalidateVisibilityCache(std::shared_ptr<Cell> cell);

  std::map<std::pair<std::shared_ptr<Cell>, float>,
           std::vector<std::shared_ptr<Cell>>>
      visibilityCache;

  void dump() {

    for (auto r : cells) {
      for (auto c : r) {
        if (c == enterCell || c == exitCell) {
          std::cout << "*";
          continue;
        }
        std::cout << c->type.name.front();
      }
      std::cout << std::endl;
    }
  }

  void invalidate() { needUpdateLight = true; }

  std::vector<std::shared_ptr<Cell>> getLine(std::shared_ptr<Cell> c1,
                                             std::shared_ptr<Cell> c2);

  std::shared_ptr<Cell> getCell(std::shared_ptr<Cell> cc, Direction d) {
    std::shared_ptr<Cell> cell;
    switch (d) {
    case N:
      cell = cells[cc->y - 1][cc->x];
      break;
    case E:
      cell = cells[cc->y][cc->x + 1];
      break;
    case S:
      cell = cells[cc->y + 1][cc->x];
      break;
    case W:
      cell = cells[cc->y][cc->x - 1];
      break;
    case NW:
      cell = cells[cc->y - 1][cc->x - 1];
      break;
    case NE:
      cell = cells[cc->y - 1][cc->x + 1];
      break;
    case SW:
      cell = cells[cc->y + 1][cc->x - 1];
      break;
    case SE:
      cell = cells[cc->y + 1][cc->x + 1];
      break;
    }
    return cell;
  }

private:
  bool needUpdateLight = true;

  virtual void onEvent(EnemyDiedEvent &e) override;
  virtual void onEvent(ItemTakenEvent &e) override;
  virtual void onEvent(EnterCellEvent &e) override;
  virtual void onEvent(LeaveCellEvent &e) override;
  virtual void onEvent(DigEvent &e) override;
  virtual void onEvent(DropEvent &e) override;
  virtual void onEvent(CommitEvent &e) override;
  virtual void onEvent(DoorOpenedEvent &e) override;

  float LeastCostEstimate(void *stateStart, void *stateEnd) override;
  void AdjacentCost(void *state,
                    MP_VECTOR<micropather::StateCost> *adjacent) override;
  void PrintStateInfo(void *state) override;
};

#endif // __LOCATION_H_
