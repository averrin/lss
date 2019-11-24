#ifndef __ROOM_H_
#define __ROOM_H_
#include "lss/game/cell.hpp"
#include "lss/game/terrain.hpp"
#include "lss/generator/mapUtils.hpp"
#include "lss/game/item.hpp"
#include "lss/game/trigger.hpp"
#include "lss/game/content/enemies.hpp"

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

  std::shared_ptr<Cell> getCell(int x, int y) {
    return cells[x + y * width];
  }

  static std::shared_ptr<Room> makeRoom(int hMax = 11, int hMin = 3, int wMax = 15,
                                int wMin = 3, CellSpec type = CellType::FLOOR) {
    auto rh = R::Z(hMin, hMax);
    auto rw = R::Z(wMin, wMax);

    auto cells = mapUtils::fill(rh, rw, type);
    auto room = std::make_shared<Room>(RoomType::HALL, cells);
    return room;
  }
};


typedef std::function<std::shared_ptr<Room>(std::shared_ptr<Location>)> RoomGenerator;
class RoomTemplate {
  RoomGenerator generator;
  public:
    RoomTemplate(RoomGenerator g) : generator(g) {}
    std::shared_ptr<Room> generate(std::shared_ptr<Location> location) {
      return generator(location);
    }
};

namespace RoomTemplates {
  const auto ALTAR_ROOM = std::make_shared<RoomTemplate>(
    /*
            .....
            .&.&.
            .*_*.
            .....
    */
    [](std::shared_ptr<Location> location) {
      auto room = Room::makeRoom(4, 4, 5, 5, CellType::FLOOR);
      auto cell = room->getCell(1, 1);
      auto s = std::make_shared<Terrain>(TerrainType::STATUE);
      s->setCurrentCell(cell);
      location->addObject<Terrain>(s);

      cell = room->getCell(3, 1);
      s = std::make_shared<Terrain>(TerrainType::STATUE);
      s->setCurrentCell(cell);
      location->addObject<Terrain>(s);
      room->features.push_back(RoomFeature::STATUE);

      cell = room->getCell(2, 2);
      s = std::make_shared<Terrain>(TerrainType::ALTAR);
      s->setCurrentCell(cell);
      location->addObject<Terrain>(s);
      room->features.push_back(RoomFeature::ALTAR);

      for (auto n : location->getNeighbors(cell)) {
        if (R::R() < 0.3) {
          n->features.push_back(CellFeature::BLOOD);
        }
      }

      if (R::R() < 0.3) {
        cell = room->getCell(2, 3);
        auto enemy = mapUtils::makeEnemy(location, cell, EnemyType::CULTIST);
        location->addObject<Enemy>(enemy);
      }

      cell->triggers.push_back(std::make_shared<UseTrigger>(Prototype::QUEST_ITEM->type, [=]{
        return Triggers::ALTAR_TRIGGER(cell, location);
      }));

      cell = room->getCell(1, 2);
      s = std::make_shared<Terrain>(TerrainType::TORCH_STAND);
      s->setCurrentCell(cell);
      location->addObject<Terrain>(s);

      cell = room->getCell(3, 2);
      s = std::make_shared<Terrain>(TerrainType::TORCH_STAND);
      s->setCurrentCell(cell);
      location->addObject<Terrain>(s);

      return room;
    });
  const auto STATUE_ROOM = std::make_shared<RoomTemplate>(
    /*
            ...
            .&.
            ...
    */
    [](std::shared_ptr<Location> location) {
      auto room = Room::makeRoom(3, 3, 3, 3, CellType::FLOOR);
      auto cell = room->getCell(1, 1);
      auto s = std::make_shared<Terrain>(TerrainType::STATUE);
      s->setCurrentCell(cell);
      location->addObject<Terrain>(s);
      room->features.push_back(RoomFeature::STATUE);

      for (auto n : location->getNeighbors(cell)) {
        n->features.push_back(CellFeature::BLOOD);
        if (R::R() < 0.2 && n->type == CellType::FLOOR) {
          auto bones = std::make_shared<Item>(ItemType::BONES, 1);
          bones->setCurrentCell(n);
          location->addObject<Item>(bones);
        }
      }
      return room;
    });
};

#endif // __ROOM_H_
