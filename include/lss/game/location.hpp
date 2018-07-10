#ifndef __LOCATION_H_
#define __LOCATION_H_

#include "lss/game/cell.hpp"
#include "lss/game/events.hpp"
#include "lss/game/fov.hpp"
#include "lss/game/object.hpp"
#include "micropather/micropather.h"
#include <cmath>
#include <map>

#include "EventHandler.hpp"
#include "EventRegisttration.hpp"
#include "fmt/format.h"

const float TORCH_DISTANCE = 4.5f;

enum LocationFeature {
  CAVE_PASSAGE,
  RIVER,
  TORCHES,
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
class Location : public micropather::Graph,
                 public eb::EventHandler<EnemyDiedEvent>,
                 public eb::EventHandler<ItemTakenEvent>,
                 public eb::EventHandler<DigEvent>,
                 public eb::EventHandler<DropEvent>,
                 public eb::EventHandler<CommitEvent>,
                 public eb::EventHandler<DoorOpenedEvent>,
                 public eb::EventHandler<LeaveCellEvent>,
                 public eb::EventHandler<EnterCellEvent> {
public:
  Location(LocationSpec t) : type(t), features(t.features) {}
  LocationSpec type;
  Cells cells;
  Objects objects;
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
  std::vector<eb::HandlerRegistrationPtr> handlers;

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
