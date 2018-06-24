#include <algorithm>
#include <memory>

#include "EventBus.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/location.hpp"
#include "lss/game/player.hpp"

Location::Location() {}

void Location::onEvent(DropEvent &e) {
  if (e.item == nullptr)
    return;
  auto item = std::make_shared<Item>(*e.item);
  item->currentCell =
      std::dynamic_pointer_cast<Creature>(e.getSender())->currentCell;
  objects.push_back(item);
}

void Location::onEvent(EnemyDiedEvent &e) {
  auto sender = e.getSender();
  if (auto enemy = std::dynamic_pointer_cast<Enemy>(sender)) {
    auto item = enemy->drop();
    if (item != std::nullopt) {
      objects.push_back(std::make_shared<Item>(**item));
    }
  }
  objects.erase(std::remove(objects.begin(), objects.end(), sender),
                objects.end());
}

void Location::onEvent(ItemTakenEvent &e) {
  objects.erase(std::remove(objects.begin(), objects.end(), e.item),
                objects.end());
}

void Location::onEvent(DigEvent &e) {
  e.cell->type = CellType::FLOOR;
  e.cell->passThrough = true;
  e.cell->seeThrough = true;

  auto rock = std::make_shared<Item>(ItemType::ROCK);
  rock->currentCell = e.cell;
  objects.push_back(rock);

  if (auto cell = cells[e.cell->y - 1][e.cell->x];
      cell->type == CellType::UNKNOWN_CELL)
    cell->type = CellType::WALL;
  if (auto cell = cells[e.cell->y][e.cell->x + 1];
      cell->type == CellType::UNKNOWN_CELL)
    cell->type = CellType::WALL;
  if (auto cell = cells[e.cell->y + 1][e.cell->x];
      cell->type == CellType::UNKNOWN_CELL)
    cell->type = CellType::WALL;
  if (auto cell = cells[e.cell->y][e.cell->x - 1];
      cell->type == CellType::UNKNOWN_CELL)
    cell->type = CellType::WALL;
  if (auto cell = cells[e.cell->y - 1][e.cell->x - 1];
      cell->type == CellType::UNKNOWN_CELL)
    cell->type = CellType::WALL;
  if (auto cell = cells[e.cell->y + 1][e.cell->x + 1];
      cell->type == CellType::UNKNOWN_CELL)
    cell->type = CellType::WALL;
  if (auto cell = cells[e.cell->y + 1][e.cell->x - 1];
      cell->type == CellType::UNKNOWN_CELL)
    cell->type = CellType::WALL;
  if (auto cell = cells[e.cell->y + 1][e.cell->x + 1];
      cell->type == CellType::UNKNOWN_CELL)
    cell->type = CellType::WALL;
}

ItemsFoundEvent::ItemsFoundEvent(eb::ObjectPtr s, Objects i)
    : eb::Event(s), items(i) {}
void Location::onEvent(EnterCellEvent &e) {
  Objects items(objects.size());
  auto it = std::copy_if(objects.begin(), objects.end(), items.begin(),
                         [e](std::shared_ptr<Object> o) {
                           return std::dynamic_pointer_cast<Item>(o) &&
                                  o->currentCell == e.cell;
                         });
  items.resize(std::distance(items.begin(), it));
  if (items.size() > 0) {
    ItemsFoundEvent ie(nullptr, items);
    eb::EventBus::FireEvent(ie);
  }
  auto hero = std::dynamic_pointer_cast<Player>(e.getSender());
  updateView(hero);
}

void Location::updateView(std::shared_ptr<Player> hero) {
  for (auto r : cells) {
    for (auto c : r) {
      if (hero->canSee(c)) {
        c->visibilityState = VisibilityState::VISIBLE;
      } else if (c->visibilityState == VisibilityState::VISIBLE) {
        c->visibilityState = VisibilityState::SEEN;
      }
    }
  }
}

float Location::LeastCostEstimate(void *stateStart, void *stateEnd) {
  auto c = static_cast<Cell *>(stateStart);
  auto cc = static_cast<Cell *>(stateEnd);
  auto d = sqrt(pow(cc->x - c->x, 2) + pow(cc->y - c->y, 2));
  return d;
}
void Location::AdjacentCost(void *state,
                            MP_VECTOR<micropather::StateCost> *neighbors) {
  auto cell = static_cast<Cell *>(state);

  if (cell->y > 0 && cell->x > 0 && cell->x < cells.front().size() - 1 &&
      cell->y < cells.size() - 1) {
    std::vector<std::shared_ptr<Cell>> nbrs = {
        cells[cell->y - 1][cell->x],     cells[cell->y - 1][cell->x - 1],
        cells[cell->y + 1][cell->x - 1], cells[cell->y][cell->x - 1],
        cells[cell->y][cell->x + 1],     cells[cell->y + 1][cell->x + 1],
        cells[cell->y + 1][cell->x + 1], cells[cell->y + 1][cell->x]};

    for (auto n : nbrs) {
      if (!n->passThrough) continue;
      auto obstacle =
          std::find_if(objects.begin(), objects.end(),
                       [n](std::shared_ptr<Object> o) {
                         return !o->passThrough && o->currentCell == n;
                       });
      if (obstacle != objects.end()) continue;
      micropather::StateCost nodeCost = {
          (void *)&(*n),
          LeastCostEstimate(state, (void *)&(*n)),
      };
      neighbors->push_back(nodeCost);
    }
  }
}
void Location::PrintStateInfo(void *state){};
