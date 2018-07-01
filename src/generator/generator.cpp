#include <chrono>
#include <iostream>

#include "lss/generator/generator.hpp"
#include "lss/generator/room.hpp"
#include "lss/game/item.hpp"
#include "rang.hpp"
#include "fmt/format.h"

Generator::Generator() {}

Cells fill(int h, int w, CellType type) {
  Cells cells;
  for (auto r = 0; r < h; r++) {
    std::vector<std::shared_ptr<Cell>> row;
    for (auto c = 0; c < w; c++) {
      auto cell = std::make_shared<Cell>(c, r, type);
      cell->visibilityState = VisibilityState::UNKNOWN;
      switch (type) {
        case CellType::FLOOR:
        case CellType::FLOOR_BLOOD:
          cell->seeThrough = true;
          cell->passThrough = true;
          break;
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
      location->cells[r][c] = room[r-y][c-x];
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
          if (std::find_if(n.begin(), n.end(), [](std::shared_ptr<Cell> c){
            return c->type == CellType::FLOOR || c->type == CellType::DOWNSTAIRS || c->type == CellType::UPSTAIRS;
          }) != n.end()) {
            cell->type = CellType::WALL;
            cell->seeThrough = false;
            cell->passThrough = false;
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

std::shared_ptr<Room> Generator::getRoom() {
  auto rh = rand() % 10 + 3;
  auto rw = rand() % 20 + 3;
  
  auto cells = fill(rh, rw, CellType::FLOOR);
  auto room = std::make_shared<Room>(RoomType::HALL, cells);
  return room;
}

std::shared_ptr<Room> Generator::makePassage(std::shared_ptr<Cell> start, Direction dir, int length) {
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

void fixOverlapped(std::shared_ptr<Location> location) {
  auto bh = location->cells.size();
  auto bw = location->cells.front().size();
  for (auto room : location->rooms) {
    for (auto r : room->cells) {
      for (auto c : r) {
        if (c->y >= bh || c->x >= bw) {
          r.erase(std::remove(r.begin(), r.end(), c));
        } else {
          room->cells[c->y-room->y][c->x-room->x] = location->cells[c->y][c->x];
        }
      }
    }
  }
}

std::shared_ptr<Location> Generator::getLocation() {
  auto t0 = std::chrono::system_clock::now();

  auto location = std::make_shared<Location>();
  auto WIDTH = 120;
  auto HEIGHT = 40;

  auto rc = rand() % 7 + 3;

  location->cells = fill(HEIGHT, WIDTH, CellType::UNKNOWN_CELL);
  for (auto n = 0; n < rc; n++) {
    auto room = getRoom();
    auto ry = rand() % (location->cells.size() - room->height - 2) + 2;
    auto rx = rand() % (location->cells.front().size() - room->width - 2) + 2;
    paste(room->cells, location, rx, ry);
    room->x = rx;
    room->y = ry;
    location->rooms.push_back(room);
  }

  auto pc = 10;

  for (auto n = 0; n < pc; n++) {
    auto room1 = location->rooms[rand() % location->rooms.size()];
    auto room2 = location->rooms[rand() % location->rooms.size()];
    if (room1 == room2) continue;

    auto sc = room1->cells[rand() % room1->height][rand() % room1->width];
    auto ec = room2->cells[rand() % room2->height][rand() % room2->width];
    if (sc == ec) continue;

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

  fixOverlapped(location);


  auto tc = rand() % 7 + 3;
  for (auto n = 0; n < tc; n++) {
    auto room = location->rooms[rand() % location->rooms.size()];
    auto cell = room->cells[rand() % room->height][rand() % room->width];
    auto torch = std::make_shared<TorchStand>();
    torch->currentCell = cell;
    location->objects.push_back(torch);
  }

  //FIXME: lost cells when rooms overlapped
  auto room = location->rooms[rand() % location->rooms.size()];
  location->enterCell = room->cells[rand() % room->height][rand() % room->width];
  location->enterCell->type = CellType::UPSTAIRS;
  location->enterCell->seeThrough = false;

  room = location->rooms[rand() % location->rooms.size()];
  location->exitCell = room->cells[rand() % room->height][rand() % room->width];
  location->exitCell->type = CellType::DOWNSTAIRS;

  placeWalls(location);
  auto t1 = std::chrono::system_clock::now();
  using milliseconds = std::chrono::duration<double, std::milli>;
  milliseconds ms = t1 - t0;
  std::cout << "location gen time taken: " << rang::fg::green << ms.count()
            << rang::style::reset << '\n';
  return location;
}
