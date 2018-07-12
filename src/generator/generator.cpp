#include <chrono>
#include <iostream>
#include <set>

#include "fmt/format.h"
#include "lss/game/door.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/item.hpp"
#include "lss/game/player.hpp"
#include "lss/generator/generator.hpp"
#include "lss/generator/room.hpp"
#include "rang.hpp"

Generator::Generator() {}

int WIDTH = 120;
int HEIGHT = 60;

namespace P {
int DOOR = 40;
int CAVE_PASSAGE = 40;
int RIVER = 10;
int TORCHES = 40;
int ENEMY = 1;
int CAVE_ROCK = 10;
int CAVE_GRASS = 5;
int CAVERN_WALL = 35;
} // namespace P

Cells fill(int h, int w, CellSpec type) {
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

void paste(Cells room, std::shared_ptr<Location> location, int x, int y) {
  auto bh = location->cells.size();
  auto bw = location->cells.front().size();
  for (auto r = y; r < room.size() + y && r < bh; r++) {
    for (auto c = x; c < room.front().size() + x && c < bw; c++) {
      auto cell = room[r - y][c - x];
      if (cell->type == CellType::UNKNOWN_CELL)
        continue;
      location->cells[r][c] = cell;
      location->cells[r][c]->x = c;
      location->cells[r][c]->y = r;
    }
  }
}

void placeWalls(std::shared_ptr<Location> location) {
  for (auto r : location->cells) {
    for (auto cell : r) {
      if (cell->type == CellType::UNKNOWN_CELL) {
        auto n = location->getNeighbors(cell);
        if (auto nc = std::find_if(n.begin(), n.end(),
                                   [](std::shared_ptr<Cell> c) {
                                     return c->type == CellType::FLOOR ||
                                            c->type == CellType::DOWNSTAIRS ||
                                            c->type == CellType::WATER ||
                                            c->type == CellType::UPSTAIRS;
                                   });
            nc != n.end()) {
          cell->type = CellType::WALL;
          cell->seeThrough = false;
          cell->passThrough = false;
          cell->features = (*nc)->features;
        }
      }
      if (cell == r.front() || cell == r.back()) {
        if (cell->type != CellType::UNKNOWN_CELL) {
          cell->type = CellType::WALL;
          cell->seeThrough = false;
          cell->passThrough = false;
        }
      }
    }
  }
  for (auto cell : location->cells.front()) {
    if (cell->type != CellType::UNKNOWN_CELL) {
      cell->type = CellType::WALL;
      cell->seeThrough = false;
      cell->passThrough = false;
    }
  }
  for (auto cell : location->cells.back()) {
    if (cell->type != CellType::UNKNOWN_CELL) {
      cell->type = CellType::WALL;
      cell->seeThrough = false;
      cell->passThrough = false;
    }
  }
}

std::shared_ptr<Room> getRoom(int hMax = 11, int hMin = 3, int wMax = 15,
                              int wMin = 3, CellSpec type = CellType::FLOOR) {
  auto rh = rand() % (hMax - hMin) + hMin;
  auto rw = rand() % (wMax - wMin) + wMin;

  auto cells = fill(rh, rw, type);
  auto room = std::make_shared<Room>(RoomType::HALL, cells);
  return room;
}

std::shared_ptr<Room> makePassage(std::shared_ptr<Cell> start, Direction dir,
                                  int length) {
  auto rh = 1;
  auto rw = 1;
  switch (dir) {
  case Direction::N:
  case Direction::S:
    rh = length;
    break;
  case Direction::W:
  case Direction::E:
    rw = length;
    break;
  }
  auto cells = fill(rh, rw, CellType::FLOOR);
  auto room = std::make_shared<Room>(RoomType::PASSAGE, cells);
  return room;
}

void makePassageBetweenRooms(std::shared_ptr<Location> location,
                             std::shared_ptr<Room> room1,
                             std::shared_ptr<Room> room2) {
  auto sc = room1->cells[rand() % room1->height][rand() % room1->width];
  auto ec = room2->cells[rand() % room2->height][rand() % room2->width];
  while (sc == ec) {
    ec = room2->cells[rand() % room2->height][rand() % room2->width];
  }

  auto l = 0;
  if (sc->y > ec->y) {
    std::swap(sc, ec);
  }

  auto passage = makePassage(sc, S, ec->y - sc->y + 1);
  paste(passage->cells, location, sc->x, sc->y);
  passage->x = sc->x;
  passage->y = sc->y;
  location->rooms.push_back(passage);
  sc = passage->cells.back().back();
  if (sc != ec) {
    if (sc->x > ec->x) {
      std::swap(sc, ec);
    }
    passage = makePassage(sc, E, ec->x - sc->x + 1);
    paste(passage->cells, location, sc->x, sc->y);
    passage->x = sc->x;
    passage->y = sc->y;
    location->rooms.push_back(passage);
  }
}

void makeFloor(std::shared_ptr<Cell> cell) {
  cell->type = CellType::FLOOR;
  cell->seeThrough = true;
  cell->passThrough = true;
}

void makeFloor(std::shared_ptr<Cell> cell, std::vector<CellFeature> features) {
  cell->features = features;
  makeFloor(cell);
}

void updateCell(std::shared_ptr<Cell> cell, CellSpec type,
                std::vector<CellFeature> features) {
  cell->type = type;
  cell->seeThrough = type.seeThrough;
  cell->passThrough = type.passThrough;
  cell->features = features;
}

std::pair<std::shared_ptr<Cell>, Cells>
dig(std::shared_ptr<Cell> start, Direction dir, int length, int minWidth,
    int jWidth, int wind = 0, CellSpec type = CellType::FLOOR) {
  auto cell = start;
  std::vector<CellFeature> f = {CellFeature::CAVE};
  Cells dCells = fill(HEIGHT, WIDTH, CellType::UNKNOWN_CELL);
  for (auto step = 0; step < length; step++) {
    auto width = (jWidth > 1 ? rand() % jWidth : 0) + minWidth;
    // TODO: make wind configurable without crazy magic
    auto w = wind == -1 ? rand() % (width >= 3 ? width * 3 / 4 : 2) : wind;
    if (w == 1) {
      w = rand() % 2 ? 0 : 1;
    }
    auto offset = rand() % 2 ? -1 : 1;
    auto ox = cell->x;
    auto oy = cell->y;
    auto nx = ox;
    auto ny = oy;
    switch (dir) {
    case Direction::N:
      ox -= width / 2;
      for (auto n = 0; n < width; n++) {
        ox++;
        if (oy < 0 || ox < 0 || oy > HEIGHT - 1 || ox > WIDTH - 1)
          continue;
        dCells[oy][ox] = std::make_shared<Cell>(ox, oy, type, f);
      }
      ny -= 1;
      nx += offset * w;
      break;
    case Direction::S:
      ox -= width / 2;
      for (auto n = 0; n < width; n++) {
        ox++;
        if (oy < 0 || ox < 0 || oy > HEIGHT - 1 || ox > WIDTH - 1)
          continue;
        dCells[oy][ox] = std::make_shared<Cell>(ox, oy, type, f);
      }
      ny += 1;
      nx += offset * w;
      break;
    case Direction::W:
      oy -= width / 2;
      for (auto n = 0; n < width; n++) {
        oy++;
        if (oy < 0 || ox < 0 || oy > HEIGHT - 1 || ox > WIDTH - 1)
          continue;
        dCells[oy][ox] = std::make_shared<Cell>(ox, oy, type, f);
      }
      nx -= 1;
      ny += offset * w;
      break;
    case Direction::E:
      oy -= width / 2;
      for (auto n = 0; n < width; n++) {
        oy++;
        if (oy < 0 || ox < 0 || oy > HEIGHT - 1 || ox > WIDTH - 1)
          continue;
        dCells[oy][ox] = std::make_shared<Cell>(ox, oy, type, f);
      }
      nx += 1;
      ny += offset * w;
      break;
    }
    if (ny < 0 || nx < 0 || ny > HEIGHT - 1 || nx > WIDTH - 1) {
      break;
    };
    cell = dCells[ny][nx];
  }
  return {cell, dCells};
}

std::pair<std::shared_ptr<Cell>, Cells> randomDig(std::shared_ptr<Cell> start,
                                                  Direction dir, int length) {
  int minWidth = 1;
  int jWidth = 6;
  bool wind = -1;
  return dig(start, dir, length, minWidth, jWidth, wind);
}

// TODO: sort rooms for minimize passage cells
void fixOverlapped(std::shared_ptr<Location> location) {
  auto bh = location->cells.size();
  auto bw = location->cells.front().size();
  for (auto room : location->rooms) {
    for (auto r : room->cells) {
      auto row = r;
      for (auto c : row) {
        if (c->y >= bh || c->x >= bw) {
          r.erase(std::remove(r.begin(), r.end(), c));
        } else {
          room->cells[c->y - room->y][c->x - room->x] =
              location->cells[c->y][c->x];
          room->cells[c->y - room->y][c->x - room->x]->room = room;
        }
      }
    }
  }
}

void placeDoors(std::shared_ptr<Location> location) {
  auto d = 0;
  for (auto r : location->cells) {
    for (auto c : r) {
      // fmt::print("{}", c->type);
      if (c->type == CellType::FLOOR) {
        if ((location->cells[c->y - 1][c->x]->type == CellType::WALL &&
             location->cells[c->y + 1][c->x]->type == CellType::WALL) ||
            (location->cells[c->y][c->x - 1]->type == CellType::WALL &&
             location->cells[c->y][c->x + 1]->type == CellType::WALL)) {
          auto n = location->getNeighbors(c);
          if (std::count_if(n.begin(), n.end(), [](std::shared_ptr<Cell> nc) {
                return nc->type == CellType::WALL;
              }) < 6) {
            auto o = location->getObjects(c);
            if (rand() % 100 > P::DOOR || c->hasFeature(CellFeature::CAVE) ||
                o.size() > 0)
              continue;
            auto nd = false;
            for (auto nc : n) {
              auto no = location->getObjects(nc);
              if (std::find_if(no.begin(), no.end(),
                               [](std::shared_ptr<Object> ob) {
                                 return std::dynamic_pointer_cast<Door>(ob);
                               }) != no.end()) {
                nd = true;
                break;
              }
            }
            if (nd)
              continue;
            auto door = std::make_shared<Door>();
            door->currentCell = c;
            c->seeThrough = false;
            location->objects.push_back(door);
            d++;
          }
        }
      }
    }
  }
  fmt::print("Doors: {}\n", d);
}

std::shared_ptr<Enemy> makeEnemy(std::shared_ptr<Location> location,
                                 std::shared_ptr<Cell> c, EnemySpec type) {
  auto enemy = std::make_shared<Enemy>(type);
  enemy->currentCell = c;
  enemy->currentLocation = location;
  return enemy;
}

// TODO: use room threat and hero level. Add respawn
void placeEnemies(std::shared_ptr<Location> location, int threat) {
  fmt::print("Threat level: {}\n", threat);
  std::vector<EnemySpec> ets{EnemyType::RAT, EnemyType::BAT,  EnemyType::GOBLIN,
                             EnemyType::ORK, EnemyType::PIXI, EnemyType::OGRE};
  std::vector<EnemySpec> ess(ets.size());
  auto it = std::copy_if(ets.begin(), ets.end(), ess.begin(),
                         [threat](EnemySpec es) { return es.level <= threat; });
  ess.resize(std::distance(ess.begin(), it));

  auto e = 0;
  for (auto r : location->cells) {
    for (auto c : r) {
      if (c->type == CellType::FLOOR) {
        if (rand() % 100 > P::ENEMY || location->getObjects(c).size() > 0)
          continue;
        auto et = ets[rand() % ess.size()];
        auto enemy = makeEnemy(location, c, et);
        location->objects.push_back(enemy);
        e++;
      }
    }
  }
  fmt::print("Enemies: {}\n", e);
}

void makeRiver(std::shared_ptr<Location> location) {
  auto river =
      dig(location->cells[0][location->cells.front().size() / 3 +
                             rand() % location->cells.front().size() / 3],
          S, location->cells.size() - 2, 2, 2, 1, CellType::WATER);
  paste(river.second, location, 0, 0);
}

std::shared_ptr<Location>
Generator::getRandomLocation(std::shared_ptr<Player> hero) {
  auto spec = LocationSpec{"Dungeon"};
  if (rand() % 100 < 30) {
    spec.type = LocationType::CAVERN;
    spec.cellFeatures.push_back(CellFeature::CAVE);
    spec.name = "Cavern";
  }
  if (rand() % 100 < P::CAVE_PASSAGE) {
    spec.features.push_back(LocationFeature::CAVE_PASSAGE);
  }
  if (rand() % 100 < P::RIVER) {
    spec.features.push_back(LocationFeature::RIVER);
  }
  if (rand() % 100 < P::TORCHES) {
    spec.features.push_back(LocationFeature::TORCHES);
  }
  // if (rand() % 100 < 10) {
  //   spec.cellFeatures.push_back(CellFeature::BLOOD);
  //   spec.name = "Temple of blood";
  // }
  int depth = 0;
  if (hero->currentLocation != nullptr) {
    depth = hero->currentLocation->depth + 1;
  }
  spec.threat = depth;
  auto l = getLocation(spec);
  l->depth = depth;
  return l;
}

void placeTorches(std::shared_ptr<Location> location) {
  auto tc = rand() % 7 + 3;
  int n;
  for (n = 0; n < tc;) {
    auto room = location->rooms[rand() % location->rooms.size()];
    auto cell = room->cells[rand() % room->height][rand() % room->width];
    if (cell->type != CellType::FLOOR || location->getObjects(cell).size() > 0)
      continue;
    auto ngs = location->getNeighbors(cell);
    if (std::find_if(ngs.begin(), ngs.end(), [](std::shared_ptr<Cell> nc) {
          return nc->type == CellType::WALL;
        }) == ngs.end())
      continue;
    auto torch = std::make_shared<TorchStand>();
    torch->currentCell = cell;
    location->objects.push_back(torch);
    n++;
  }
  auto nbrs = location->getNeighbors(location->exitCell);
  auto torch = std::make_shared<TorchStand>();
  torch->currentCell = nbrs[rand() % nbrs.size()];
  location->objects.push_back(torch);

  nbrs = location->getNeighbors(location->enterCell);
  torch = std::make_shared<TorchStand>();
  torch->currentCell = nbrs[rand() % nbrs.size()];
  location->objects.push_back(torch);

  fmt::print("Torches: {}\n", n);
}

auto getRandomCell(std::shared_ptr<Location> location, CellSpec type) {
  auto room = location->rooms.size() > 1
                  ? location->rooms[rand() % location->rooms.size()]
                  : location->rooms.front();
  auto cell = room->cells[rand() % room->height][rand() % room->width];
  while (cell->type != type) {
    cell = room->cells[rand() % room->height][rand() % room->width];
  }
  return cell;
}

void placeStairs(std::shared_ptr<Location> location) {
  location->enterCell = getRandomCell(location, CellType::FLOOR);
  location->exitCell = getRandomCell(location, CellType::FLOOR);

  auto pather = new micropather::MicroPather(location.get());
  micropather::MPVector<void *> path;
  float totalCost = 0;
  pather->Reset();
  int result = pather->Solve(location->enterCell.get(),
                             location->exitCell.get(), &path, &totalCost);

  // for (auto r : location->cells) {
  //   for (auto c : r) {
  //     if (c == location->enterCell || c == location->exitCell) {
  //       fmt::print(" ");
  //       continue;
  //     }
  //     fmt::print("{}", c->type.name.front());
  //   }
  //   fmt::print("\n");
  // }

  while (result != micropather::MicroPather::SOLVED || totalCost < 20) {
    // return;
    location->enterCell = getRandomCell(location, CellType::FLOOR);
    location->exitCell = getRandomCell(location, CellType::FLOOR);
    pather->Reset();
    result = pather->Solve(location->enterCell.get(), location->exitCell.get(),
                           &path, &totalCost);
  }
  fmt::print("{}.{} -> {}.{} = {}\n", location->enterCell->x,
             location->enterCell->y, location->exitCell->x,
             location->exitCell->y, totalCost);
  fmt::print("\n");
  delete pather;

  location->exitCell->type = CellType::DOWNSTAIRS;
  auto n = location->getNeighbors(location->exitCell);
  std::for_each(n.begin(), n.end(), [](auto c) { c->type = CellType::FLOOR; });
  location->enterCell->type = CellType::UPSTAIRS;
  n = location->getNeighbors(location->enterCell);
  std::for_each(n.begin(), n.end(), [](auto c) { c->type = CellType::FLOOR; });
}

void makePassages(std::shared_ptr<Location> location) {
  auto pc = rand() % 10;
  auto n = 0;
  std::set<std::shared_ptr<Room>> rooms;
  std::vector<std::shared_ptr<Room>> halls = location->rooms;

  while (n < pc) {
    auto room1 = location->rooms[rand() % location->rooms.size()];
    auto room2 = location->rooms[rand() % location->rooms.size()];
    if (room1 == room2)
      continue;
    if (std::find(halls.begin(), halls.end(), room1) != halls.end())
      rooms.insert(room1);
    if (std::find(halls.begin(), halls.end(), room2) != halls.end())
      rooms.insert(room2);
    n++;
    makePassageBetweenRooms(location, room1, room2);
  }
  for (auto r : halls) {
    if (std::find(rooms.begin(), rooms.end(), r) == rooms.end()) {
      auto room2 = location->rooms[rand() % location->rooms.size()];
      while (room2 == r) {
        room2 = location->rooms[rand() % location->rooms.size()];
      }
      makePassageBetweenRooms(location, r, room2);
    }
  }
}

void placeRooms(std::shared_ptr<Location> location) {
  auto rc = rand() % 12 + 7;

  location->cells = fill(HEIGHT, WIDTH, CellType::UNKNOWN_CELL);
  for (auto n = 0; n < rc; n++) {
    auto room = getRoom();
    auto ry = rand() % (location->cells.size() - room->height - 2) + 2;
    auto rx = rand() % (location->cells.front().size() - room->width - 2) + 2;
    paste(room->cells, location, rx, ry);
    room->x = rx;
    room->y = ry;
    room->threat = rand() % 4;
    location->rooms.push_back(room);
  }
}

void placeCaves(std::shared_ptr<Location> location) {
  auto rc = rand() % 12 + 7;

  location->cells = fill(HEIGHT, WIDTH, CellType::UNKNOWN_CELL);
  for (auto n = 0; n < rc; n++) {
    auto room = getRoom(44, 12, 44, 12, CellType::WALL);

    for (auto r : room->cells) {
      for (auto c : r) {
        if (rand() % 100 > P::CAVERN_WALL) {
          c->type = CellType::FLOOR;
          c->passThrough = true;
        }
      }
    }
    auto ry = rand() % (location->cells.size() - room->height - 2) + 2;
    auto rx = rand() % (location->cells.front().size() - room->width - 2) + 2;
    paste(room->cells, location, rx, ry);
    room->x = rx;
    room->y = ry;
    room->threat = rand() % 4;
    location->rooms.push_back(room);
  }
  fixOverlapped(location);

  for (auto i = 0; i < 5; i++) {
    for (auto room : location->rooms) {
      for (auto r : room->cells) {
        for (auto c : r) {
          auto n = location->getNeighbors(c);
          auto fn =
              std::count_if(n.begin(), n.end(), [](std::shared_ptr<Cell> nc) {
                return nc->type == CellType::FLOOR;
              });

          if (c->type == CellType::FLOOR) {
            if (fn < 4) {
              c->type = CellType::WALL;
              c->passThrough = false;
            }
          } else if (c->type == CellType::WALL) {
            if (fn >= 6) {
              c->type = CellType::FLOOR;
              c->passThrough = true;
            }
          }
        }
      }
    }
  }

  for (auto room : location->rooms) {
    for (auto r : room->cells) {
      for (auto c : r) {
        if (c->type == CellType::UNKNOWN_CELL)
          continue;
        auto n = location->getNeighbors(c);
        auto fn =
            std::count_if(n.begin(), n.end(), [](std::shared_ptr<Cell> nc) {
              return nc->type == CellType::FLOOR;
            });
        if (fn == 0) {
          c->type = CellType::UNKNOWN_CELL;
        } else if (fn != 8) {
          if (c->type == CellType::FLOOR && rand() % 100 < P::CAVE_ROCK) {
            auto rock = std::make_shared<Item>(ItemType::ROCK, 1);
            rock->currentCell = c;
            location->objects.push_back(rock);
            continue;
          }
        } else if (c->type == CellType::FLOOR && rand() % 100 < P::CAVE_GRASS) {
          auto grass = Prototype::GRASS->clone();
          grass->currentCell = c;
          location->objects.push_back(grass);
          continue;
        }
      }
    }
  }
}

void makeCavePassage(std::shared_ptr<Location> location) {
  std::vector<Direction> ds{N, E, S, W};
  auto room = location->rooms[rand() % location->rooms.size()];
  auto cell = room->cells[rand() % room->height][rand() % room->width];
  for (auto n = 0; n < rand() % 6 + 2; n++) {
    auto res = randomDig(cell, ds[rand() % ds.size()], rand() % 30);
    cell = res.first;
    auto newRoom = std::make_shared<Room>(RoomType::CAVE_ROOM, res.second);
    paste(newRoom->cells, location, 0, 0);
    newRoom->x = 0;
    newRoom->y = 0;
    newRoom->threat = rand() % 4;
    location->rooms.push_back(newRoom);
    for (auto r : newRoom->cells) {
      for (auto c : r) {
        if (c->type == CellType::FLOOR && rand() % 100 < P::CAVE_ROCK) {
          auto rock = std::make_shared<Item>(ItemType::ROCK, 1);
          rock->currentCell = c;
          location->objects.push_back(rock);
        } else if (c->type == CellType::FLOOR && rand() % 100 < P::CAVE_GRASS) {
          auto grass = Prototype::GRASS->clone();
          grass->currentCell = c;
          location->objects.push_back(grass);
          continue;
        }
      }
    }
  }
}

std::shared_ptr<Location> Generator::getLocation(LocationSpec spec) {
  auto t0 = std::chrono::system_clock::now();

  auto location = std::make_shared<Location>(spec);

  if (spec.type == LocationType::DUNGEON) {
    placeRooms(location);

    makePassages(location);
    fixOverlapped(location);

    if (location->hasFeature(LocationFeature::CAVE_PASSAGE)) {
      makeCavePassage(location);
    }
  } else if (spec.type == LocationType::CAVERN) {
    placeCaves(location);
  }

  if (location->hasFeature(LocationFeature::RIVER)) {
    makeRiver(location);
  }

  placeStairs(location);
  placeWalls(location);

  if (spec.type == LocationType::DUNGEON) {
    placeDoors(location);
  }
  placeEnemies(location, spec.threat);

  if (location->hasFeature(LocationFeature::TORCHES)) {
    placeTorches(location);
  }

  for (auto r : location->cells) {
    for (auto c : r) {
      for (auto cf : location->type.cellFeatures) {
        c->features.push_back(cf);
      }
    }
  }

  auto t1 = std::chrono::system_clock::now();
  using milliseconds = std::chrono::duration<double, std::milli>;
  milliseconds ms = t1 - t0;
  std::cout << "location gen time taken: " << rang::fg::green << ms.count()
            << rang::style::reset << '\n';
  return location;
}
