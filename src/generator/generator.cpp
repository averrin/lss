#include <chrono>
#include <iostream>

#include "lss/generator/generator.hpp"
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
  for (auto r = y; r < room.size() + y; r++) {
    for (auto c = x; c < room.front().size() + x; c++) {
      location->cells[r][c] = room[r-y][c-x];
      location->cells[r][c]->x = c;
      location->cells[r][c]->y = r;
    }
  }
}

void placeWalls(std::shared_ptr<Location> location) {
  auto done = false;
  while (!done) {
    auto found = false;
    for (auto r : location->cells) {
      for (auto cell : r) {
        if (cell->type == CellType::UNKNOWN_CELL) {
          auto n = location->getNeighbors(cell);
          if (std::find_if(n.begin(), n.end(), [](std::shared_ptr<Cell> c){
            return c->type == CellType::FLOOR;
          }) != n.end()) {
            cell->type = CellType::WALL;
            cell->seeThrough = false;
            cell->passThrough = false;
            found = true;
          }
        }
      }
    }
    if (!found) {
      done = true;
    }
  }
}

Cells Generator::getRoom(int h, int w) {
  Cells cells;
  for (auto r = 0; r < h; r++) {
    std::vector<std::shared_ptr<Cell>> row;
    for (auto c = 0; c < w; c++) {
      auto cell = std::make_shared<Cell>(c, r, CellType::UNKNOWN_CELL);
      cell->visibilityState = VisibilityState::UNKNOWN;

      if (cell->y > 0 && cell->x > 0 && cell->x < w - 1 && cell->y < h - 1) {
        cell->type = CellType::FLOOR;
        cell->seeThrough = true;
        cell->passThrough = true;
      } else {
        cell->type = CellType::WALL;
        cell->seeThrough = false;
        cell->passThrough = false;
      }
      row.push_back(cell);
    }

    cells.push_back(row);
  }
  return cells;
}

std::shared_ptr<Location> Generator::getLocation() {
  auto t0 = std::chrono::system_clock::now();

  auto location = std::make_shared<Location>();
  auto WIDTH = 120;
  auto HEIGHT = 40;

  location->cells = fill(HEIGHT, WIDTH, CellType::UNKNOWN_CELL);
  auto rh = rand() % 20 + 3;
  auto rw = rand() % 40 + 3;
  
  auto room = fill(rh, rw, CellType::FLOOR);
  auto ry = rand() % (location->cells.size() - rh - 2) + 2;
  auto rx = rand() % (location->cells.front().size() - rw - 2) + 2;
  paste(room, location, rx, ry);
  placeWalls(location);
  location->enterCell = location->cells[ry+2][rx+2];
  location->enterCell->type = CellType::UPSTAIRS;
  location->enterCell->seeThrough = false;

  location->exitCell = location->cells[ry+rh-1][rx+rw-1];
  location->exitCell->type = CellType::DOWNSTAIRS;

  auto t1 = std::chrono::system_clock::now();
  using milliseconds = std::chrono::duration<double, std::milli>;
  milliseconds ms = t1 - t0;
  std::cout << "location gen time taken: " << rang::fg::green << ms.count()
            << rang::style::reset << '\n';
  return location;
}
