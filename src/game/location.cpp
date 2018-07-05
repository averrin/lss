#include "rang.hpp"
#include <algorithm>
#include <chrono>
#include <memory>

#include "EventBus.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/item.hpp"
#include "lss/game/location.hpp"
#include "lss/game/player.hpp"
#include "lss/utils.hpp"

const float TORCH_DISTANCE = 4.5f;

float getDistance(std::shared_ptr<Cell> c, std::shared_ptr<Cell> cc) {
  return sqrt(pow(cc->x - c->x, 2) + pow(cc->y - c->y, 2));
}

Location::Location() {}

void Location::onEvent(DoorOpenedEvent &e) {
  fmt::print("on door open\n");
  player->calcViewField(true);
  updateView(player);
}

void Location::onEvent(CommitEvent &e) {
  auto t0 = std::chrono::system_clock::now();
  player->calcViewField();
  updateView(player);
  auto t1 = std::chrono::system_clock::now();
  using milliseconds = std::chrono::duration<double, std::milli>;
  milliseconds ms = t1 - t0;
  std::cout << "location in commit time taken: " << rang::fg::green
            << ms.count() << rang::style::reset << '\n';
}

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
    enemy->currentCell->features.push_back(CellFeature::BLOOD);
    auto loot = enemy->drop();
    if (loot != std::nullopt) {
      for (auto item : *loot) {
        auto new_item = std::make_shared<Item>(*item);
        new_item->currentCell = enemy->currentCell;
        objects.push_back(new_item);
      }
    }
  }
  objects.erase(std::remove(objects.begin(), objects.end(), sender),
                objects.end());
}

void Location::onEvent(ItemTakenEvent &e) {
  objects.erase(std::remove(objects.begin(), objects.end(), e.item),
                objects.end());
}

void Location::reveal() {
  for (auto r : cells) {
    for (auto c : r) {
      c->visibilityState = VisibilityState::VISIBLE;
    }
  }
};

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
  if (e.cell->illuminated) {
    auto vd = TORCH_DISTANCE;
    for (auto ls : e.cell->lightSources) {
      auto lsi = visibilityCache.find({ls, vd});
      if (lsi != visibilityCache.end()) {
        visibilityCache.erase(lsi);
      }
    }
  }
}

void Location::enter(std::shared_ptr<Player> hero, std::shared_ptr<Cell> cell) {
  player = hero;
  hero->currentCell = cell;

  for (auto o : objects) {
    if (auto enemy = std::dynamic_pointer_cast<Enemy>(o)) {
      enemy->registration = eb::EventBus::AddHandler<CommitEvent>(*enemy, hero);
      enemy->calcViewField();
    }
  }
  hero->calcViewField();
  updateView(hero);
  hero->commit(0);

  handlers.push_back(eb::EventBus::AddHandler<EnemyDiedEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<ItemTakenEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<EnterCellEvent>(*this, hero));
  handlers.push_back(eb::EventBus::AddHandler<DigEvent>(*this, hero));
  handlers.push_back(eb::EventBus::AddHandler<DropEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<CommitEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<DoorOpenedEvent>(*this));
}

void Location::leave(std::shared_ptr<Player> hero) {
  for (auto r : handlers) {
    r->removeHandler();
  }

  for (auto o : objects) {
    if (auto enemy = std::dynamic_pointer_cast<Enemy>(o)) {
      enemy->registration->removeHandler();
    }
  }
}

void Location::updateLight(std::shared_ptr<Player> hero) {
  auto heroVD = hero->VISIBILITY_DISTANCE(hero.get());
  auto hasLight = hero->hasLight();
  auto enemies = utils::castObjects<Enemy>(objects);
  std::vector<std::shared_ptr<Cell>> torches;
  for (auto ts : utils::castObjects<TorchStand>(objects)) {
    torches.push_back(ts->currentCell);
  }
  for (auto e : enemies) {
    if (e->hasLight()) {
      torches.push_back(e->currentCell);
    }
  }

  for (auto r : cells) {
    for (auto c : r) {
      if (c->type == CellType::UNKNOWN_CELL)
        continue;
      c->lightSources.clear();
      c->seeThrough = c->type.seeThrough;
      if (std::find_if(objects.begin(), objects.end(),
                       [c](std::shared_ptr<Object> o) {
                         return o->currentCell == c && !o->seeThrough;
                       }) != objects.end()) {
        c->seeThrough = false;
      }
      c->illuminated = false;
      // FIXME: not in distance, but only visible
      if (hasLight && getDistance(c, hero->currentCell) <= heroVD) {
        c->illuminated = true;
        c->lightSources.push_back(hero->currentCell);
        continue;
      }
    }
  }
  for (auto t : torches) {
    for (auto c : getVisible(t, TORCH_DISTANCE)) {
      c->lightSources.push_back(c);
      c->illuminated = true;
    }
  }
}

void Location::updateView(std::shared_ptr<Player> hero) {
  auto t0 = std::chrono::system_clock::now();
  updateLight(hero);
  auto t1 = std::chrono::system_clock::now();
  using milliseconds = std::chrono::duration<double, std::milli>;
  milliseconds ms = t1 - t0;
  std::cout << "update light time taken: " << rang::fg::green << ms.count()
            << rang::style::reset << '\n';
  for (auto r : cells) {
    for (auto c : r) {
      if (c->type == CellType::UNKNOWN_CELL)
        continue;
      if (hero->canSee(c) || hero->hasTrait(Traits::MIND_SIGHT)) {
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
        cells[cell->y - 1][cell->x + 1], cells[cell->y + 1][cell->x]};

    for (auto n : nbrs) {
      if (!n->passThrough)
        continue;
      auto obstacle = std::find_if(
          objects.begin(), objects.end(), [n](std::shared_ptr<Object> o) {
            return !o->passThrough && o->currentCell == n;
          });
      if (obstacle != objects.end())
        continue;
      micropather::StateCost nodeCost = {
          (void *)&(*n), LeastCostEstimate(state, (void *)&(*n)),
      };
      neighbors->push_back(nodeCost);
    }
  }
}
void Location::PrintStateInfo(void *state){};

Objects Location::getObjects(std::shared_ptr<Cell> cell) {
  Objects cellObjects(objects.size());
  auto it = std::copy_if(
      objects.begin(), objects.end(), cellObjects.begin(),
      [cell](std::shared_ptr<Object> o) { return o->currentCell == cell; });

  cellObjects.resize(std::distance(cellObjects.begin(), it));
  return cellObjects;
}
