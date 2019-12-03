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
        // if (cell->type == CellType::UNKNOWN)
        //   continue;
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

  void rotate() {
    std::vector<std::shared_ptr<Cell>> result(cells.size());
    auto n = 0;
    for (auto c : cells) {
      auto cx = n % width;
      auto cy = n / width;
      auto nx = height - (cy + 1);
      auto ny = cx;
      auto nn = nx + height*ny;
      result[nn] = c;
      c->x = nx;
      c->y = ny;
      n++;
    }
    cells = result;
    auto w = width;
    auto h = height;
    width = h;
    height = w;
  }

std::optional<std::shared_ptr<Cell>> getRandomCell(CellSpec type) {
  std::vector<std::shared_ptr<Cell>> result(cells.size());
  auto it = std::copy_if(cells.begin(), cells.end(), result.begin(),
                         [type](auto cell) { return cell->type == type; });
  result.resize(std::distance(result.begin(), it));
  if (result.size() == 0) {
    // std::cout << rang::fg::red << "FATAL: " << "cannot get random cell with this type" << rang::style::reset << std::endl;
    return std::nullopt;
  }

  auto cell = result[rand() % result.size()];
  while (cell->type != type) {
    cell = result[rand() % result.size()];
  }
  return cell;
}


  static std::shared_ptr<Room> makeRoom(int hMax = 11, int hMin = 3, int wMax = 15,
                                int wMin = 3, CellSpec type = CellType::FLOOR) {
    auto rh = R::Z(hMin, hMax);
    auto rw = R::Z(wMin, wMax);

    auto cells = mapUtils::fill(rh, rw, type);
    auto room = std::make_shared<Room>(RoomType::HALL, cells);
    for (auto c: room->cells) {
      c->room = room;
    }
    return room;
  }

  static void paste(std::shared_ptr<Room> inner,
            std::shared_ptr<Room> outer, int x, int y) {
    for (auto cell : inner->cells) {
      if (cell->type == CellType::UNKNOWN)
        continue;
      auto r = y + cell->y;
      auto c = x + cell->x;
      outer->cells.at(r*outer->width + c) = cell;
      cell->x = c;
      cell->y = r;
    }
  }

  std::vector<std::shared_ptr<Cell>> getNeighbors(std::shared_ptr<Cell> cell) {
    std::vector<std::shared_ptr<Cell>> nbrs;
    if (cell->x > 0) {
      if (cell->y > 0) {
        nbrs.push_back(cells[(cell->y - 1)*width+(cell->x - 1)]);
        nbrs.push_back(cells[(cell->y - 1)*width+(cell->x)]);
        nbrs.push_back(cells[(cell->y)*width+(cell->x - 1)]);

        if (cell->x < int(width - 1)) {
          nbrs.push_back(cells[(cell->y - 1)*width+(cell->x + 1)]);
          nbrs.push_back(cells[(cell->y)*width+(cell->x + 1)]);
        }
      }
      if (cell->y < int(height-1)) {
        if (cell->x < int(width-1)) {
          nbrs.push_back(cells[(cell->y + 1)*width+(cell->x + 1)]);
          nbrs.push_back(cells[(cell->y + 1)*width+(cell->x - 1)]);
        }
        nbrs.push_back(cells[(cell->y + 1)*width+(cell->x)]);
      }
    }
    return nbrs;

  }



  static std::shared_ptr<Room> makeBlob(std::shared_ptr<Location> location, int hMax = 11, int hMin = 3, int wMax = 15,
                                int wMin = 3, CellSpec type = CellType::FLOOR, CellSpec outerType = CellType::UNKNOWN, bool smooth = true) {
    auto room = Room::makeRoom(hMax, hMin, wMax, wMin, outerType);
    auto outerRoom = Room::makeRoom(room->height+2, room->height+2, room->width+2, room->width+2, outerType);

    for (auto c : room->cells) {
      if (R::R() > 0.35) {
        c->type = type;
        c->passThrough = type.passThrough;
        c->seeThrough = type.seeThrough;
      }
    }

    Room::paste(room, outerRoom, 1, 1);

    for (auto c : room->cells) {
      auto n = outerRoom->getNeighbors(c);
      auto vn =
          std::count_if(n.begin(), n.end(), [type](std::shared_ptr<Cell> nc) {
            return nc->type == type;
          });
      vn += 8 - n.size();

      if (c->type == type) {
        if (vn < 4) {
          c->type = outerType;
          c->passThrough = outerType.passThrough;
          c->seeThrough = outerType.seeThrough;
        } else if (c->type != type) {
          if (vn >= 6) {
            c->type = type;
            c->passThrough = type.passThrough;
            c->seeThrough = type.seeThrough;
          }
        }
      }
    }

    if (smooth) {
      for (auto n = 0; n < 5; n++) {
        for (auto c : room->cells) {
          if (c->type == outerType) {
            auto n = outerRoom->getNeighbors(c);
            auto fn =
                std::count_if(n.begin(), n.end(), [=](std::shared_ptr<Cell> nc) {
                  return nc->type == type;
                });
            if (fn == 0) {
              c->type = CellType::UNKNOWN;
            } else if (fn >= 3) {
              c->type = type;
            }
          }
        }
      }

    }

    // room->print();
    return room;
  }


    static void printRoom(std::shared_ptr<Location> location, std::shared_ptr<Room> room) {
      auto n = 0;

      fmt::print("Room stats: {}x{} = {} @ {}.{}\n", room->width, room->height, room->cells.size(), room->x, room->y);
      for (auto c : room->cells) {
        if (n % room->width == 0 && n > 0) {
          fmt::print("\n");
        }
        auto o = location->getObjects(c);
        if (o.size() > 0) {
          fmt::print("o");
          n++;
          continue;
        }
        if (c->type == CellType::WALL) {
          fmt::print("#");
        } else if (c->type == CellType::FLOOR) {
          fmt::print(".");
        } else if (c->type == CellType::UNKNOWN) {
          fmt::print(" ");
        } else if (c->type == CellType::VOID) {
          fmt::print("⌄");
        } else if (c->type == CellType::WATER) {
          fmt::print("=");
        } else {
          fmt::print("?");
        }
        n++;
      }
      fmt::print("\n");
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
      s->setName("statue");
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

      if (R::R() < 0.5) {
        cell = room->getCell(2, 3);
        auto enemy = mapUtils::makeEnemy(location, cell, EnemyType::CULTIST);
        location->addObject<Enemy>(enemy);
      }

      cell->triggers.push_back(std::make_shared<UseItemTrigger>(Prototype::QUEST_ITEM->type, [=]{
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

      auto n = rand() % 4;
      for (auto i = 0; i < n; i++) {
        room->rotate();
      }

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

  const auto TREASURE_ROOM = std::make_shared<RoomTemplate>(
    /*
            ###
            #(#
            ###
    */
    [](std::shared_ptr<Location> location) {
      auto innerRoom = Room::makeRoom(3, 1, 3, 1, CellType::FLOOR);
      auto room = Room::makeRoom(innerRoom->height+2, innerRoom->height+2, innerRoom->width+2, innerRoom->width+2, CellType::WALL);

      Room::paste(innerRoom, room, 1, 1);
      // room->print();

      auto cell = room->getRandomCell(CellType::FLOOR);

      auto box = LootBoxes::LOOT_TIER_3;
      for (auto item : box.open(true)) {
        item->setCurrentCell(*cell);
        location->addObject<Item>(item);
      }

      // for (auto c: innerRoom->cells) {
      //   mapUtils::updateCell(c, CellType::FLOOR, {CellFeature::BLOOD});
      // }

      return room;
    });

  const auto HEAL_STAND_ROOM = std::make_shared<RoomTemplate>(
    /*
            .*.
            .&.
            ...
    */
    [](std::shared_ptr<Location> location) {
      auto room = Room::makeRoom(3, 3, 3, 3, CellType::FLOOR);
      auto cell = room->getCell(1, 1);
      auto s = std::make_shared<UsableTerrain>(TerrainType::ALTAR);
      s->setCurrentCell(cell);
      location->addObject<Terrain>(s);

      cell = room->getCell(1, 0);
      auto fb = std::make_shared<Terrain>(TerrainType::ACID_LIGHT_FOREVER, -1);
      fb->setCurrentCell(cell);
      fb->setName("heal_light");
      location->addObject(fb);

      s->triggers.push_back(std::make_shared<UseTrigger>([=](std::shared_ptr<Player> hero){
        return Triggers::HEAL_STAND_TRIGGER(hero, s);
      }));

      auto n = rand() % 4;
      for (auto i = 0; i < n; i++) {
        room->rotate();
      }
      return room;
    });

  const auto MANA_STAND_ROOM = std::make_shared<RoomTemplate>(
    /*
            .*.
            .&.
            ...
    */
    [](std::shared_ptr<Location> location) {
      auto room = Room::makeRoom(3, 3, 3, 3, CellType::FLOOR);
      auto cell = room->getCell(1, 1);
      auto s = std::make_shared<UsableTerrain>(TerrainType::ALTAR);
      s->setCurrentCell(cell);
      location->addObject<Terrain>(s);

      cell = room->getCell(1, 0);
      auto fb = std::make_shared<Terrain>(TerrainType::MAGIC_LIGHT_FOREVER, -1);
      fb->setCurrentCell(cell);
      fb->setName("mana_light");
      location->addObject(fb);

      s->triggers.push_back(std::make_shared<UseTrigger>([=](std::shared_ptr<Player> hero){
        return Triggers::MANA_STAND_TRIGGER(hero, s);
      }));

      auto n = rand() % 4;
      for (auto i = 0; i < n; i++) {
        room->rotate();
      }
      return room;
    });

  const auto BONES = std::make_shared<RoomTemplate>(
    /*
             %%
            %%%%
             %%%
    */
    [](std::shared_ptr<Location> location) {
      auto room = Room::makeBlob(location, 7, 2, 7, 2, CellType::FLOOR, CellType::UNKNOWN, false);
      for (auto c : room->cells) {
        if (c->type != CellType::FLOOR) {
          continue;
        }
        auto bones = std::make_shared<Item>(ItemType::BONES, rand() % 3 + 1);
        bones->setCurrentCell(c);
        location->addObject<Item>(bones);
      }
      return room;
    });

  const auto ICE = std::make_shared<RoomTemplate>(
    /*
             %%
            %%%%
             %%%
    */
    [](std::shared_ptr<Location> location) {
      auto room = Room::makeBlob(location, 8, 3, 8, 3, CellType::FLOOR, CellType::UNKNOWN, true);
      for (auto c : room->cells) {
        if (c->type != CellType::FLOOR) {
          continue;
        }
        c->addFeature(CellFeature::FROST);
      }
      return room;
    });
};

#endif // __ROOM_H_
