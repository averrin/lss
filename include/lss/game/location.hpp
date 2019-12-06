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
#include <optional>

enum LocationFeature {
  CAVE_PASSAGE,
  RIVER,
  TORCHES,
  STATUE,
  ALTAR,
  LAKE,
  VOID,
  ICE,
  HEAL,
  MANA,
  TREASURE_SMALL,
  CORRUPT,
  BONES_FIELD,
};

enum LocationType { DUNGEON, CAVERN };

struct LocationSpec {
  std::string name;
  int threat = 0;
  LocationType type = LocationType::DUNGEON;
  std::vector<LocationFeature> features;
  std::vector<CellFeature> cellFeatures;
  std::shared_ptr<Cell> enterCell;
};

class Player;
class Room;
class AiManager;
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
  LibLog::Logger &log = LibLog::Logger::getInstance();
  Location(LocationSpec t) : Object(), type(t), features(t.features) {}
  ~Location();
  LocationSpec type;
  Cells cells;
  Objects objects{};
    std::map<std::shared_ptr<Cell>, Objects> cellObjects = std::map<std::shared_ptr<Cell>, Objects>{};
  std::shared_ptr<Player> player;
  int depth = 0;
  std::shared_ptr<AiManager> aiManager;

  template <typename T>
  std::optional<std::shared_ptr<T>> getObject(std::string name) {
    for (auto o : objects) {
      if (auto casted_object = std::dynamic_pointer_cast<T>(o)) {
        if (casted_object->name == name) {
          return casted_object;
        }
      }
    }
    return std::nullopt;
  }


  template <typename T>
  void addObject(std::shared_ptr<T> o, std::shared_ptr<T> cc) {
    o->setCurrentCell(cc);
    addObject<T>(o);
  }

  template <typename T> void addObject(std::shared_ptr<T> o) {
    if (o->currentCell == nullptr) {
      throw std::runtime_error("you cannot add object without currentCell");
      return;
    }
    // o->currentLocation = shared_from_this();
    objects.push_back(o);
    // if (cellObjects.find(o->currentCell) == cellObjects.end()) {
    //   cellObjects[o->currentCell] = {};
    // }
    // cellObjects[o->currentCell].push_back(o);
  }

  void removeObject(std::shared_ptr<Object> o) {
    objects.erase(std::remove(objects.begin(), objects.end(), o));
    // auto co = cellObjects[o->currentCell];
    // co.erase(std::remove(co.begin(), co.end(), o));
    o->removeCell();
  }

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
    std::map<LocationFeature, std::string> featureMap = {
    {LocationFeature::TORCHES, "T"},
    {LocationFeature::CAVE_PASSAGE, "C"},
    {LocationFeature::RIVER, "R"},
    {LocationFeature::STATUE, "S"},
    {LocationFeature::ALTAR, "A"},
    {LocationFeature::VOID, "V"},
    {LocationFeature::LAKE, "L"},
    {LocationFeature::ICE, "I"},
    {LocationFeature::HEAL, "H"},
    {LocationFeature::MANA, "M"},
    {LocationFeature::TREASURE_SMALL, "t"},
    {LocationFeature::CORRUPT, "c"},
    {LocationFeature::BONES_FIELD, "B"},
  };
    std::string locationFeatures = "";
    for (auto [f, l] : featureMap) {
      if (hasFeature(f)) {
        locationFeatures += l;
      }
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

        if (cell->x < int(cells.front().size() - 1)) {
          nbrs.push_back(cells[cell->y - 1][cell->x + 1]);
          nbrs.push_back(cells[cell->y][cell->x + 1]);
        }
      }
      if (cell->y < int(cells.size() - 1)) {
        if (cell->x < int(cells.front().size() - 1)) {
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

  void invalidate(std::string reason);
  void invalidate() { needUpdateLight = true; }

  std::vector<std::shared_ptr<Cell>> getLine(std::shared_ptr<Cell> c1,
                                             std::shared_ptr<Cell> c2);

  std::optional<std::shared_ptr<Cell>> getCell(std::shared_ptr<Cell> cc,
                                               Direction d) {
    std::optional<std::shared_ptr<Cell>> cell;
    switch (d) {
    case N:
      if (cc->y == 0)
        break;
      cell = cells[cc->y - 1][cc->x];
      break;
    case E:
      if (cc->x == cells.front().size())
        break;
      cell = cells[cc->y][cc->x + 1];
      break;
    case S:
      if (cc->y == cells.size())
        break;
      cell = cells[cc->y + 1][cc->x];
      break;
    case W:
      if (cc->x == 0)
        break;
      cell = cells[cc->y][cc->x - 1];
      break;
    case NW:
      if (cc->y == 0)
        break;
      if (cc->x == 0)
        break;
      cell = cells[cc->y - 1][cc->x - 1];
      break;
    case NE:
      if (cc->y == 0)
        break;
      if (cc->x == cells.front().size())
        break;
      cell = cells[cc->y - 1][cc->x + 1];
      break;
    case SW:
      if (cc->y == cells.size())
        break;
      if (cc->x == 0)
        break;
      cell = cells[cc->y + 1][cc->x - 1];
      break;
    case SE:
      if (cc->y == cells.size())
        break;
      if (cc->x == cells.front().size())
        break;
      cell = cells[cc->y + 1][cc->x + 1];
      break;
    }
    return cell;
  }

  std::optional<Direction> getDirFromCell(std::shared_ptr<Cell> start,
                                          std::shared_ptr<Cell> end) {
    std::optional<Direction> dir = std::nullopt;
    if (start == nullptr || end == nullptr) {
      return dir;
    }
    if (start->x == end->x && start->y < end->y) {
      dir = Direction::N;
    } else if (start->x == end->x && start->y > end->y) {
      dir = Direction::S;
    } else if (start->x < end->x && start->y == end->y) {
      dir = Direction::W;
    } else if (start->x > end->x && start->y == end->y) {
      dir = Direction::E;
    } else if (start->x < end->x && start->y < end->y) {
      dir = Direction::NW;
    } else if (start->x > end->x && start->y < end->y) {
      dir = Direction::NE;
    } else if (start->x < end->x && start->y > end->y) {
      dir = Direction::SW;
    } else if (start->x > end->x && start->y > end->y) {
      dir = Direction::SE;
    }
    return dir;
  }

  float getDistance(std::shared_ptr<Cell> c, std::shared_ptr<Cell> cc) {
    return sqrt(pow(cc->x - c->x, 2) + pow(cc->y - c->y, 2));
  }
  bool needUpdateLight = true;
  int apAccomulator = 0;

private:
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
