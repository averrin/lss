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

float getDistance(std::shared_ptr<Cell> c, std::shared_ptr<Cell> cc) {
  return sqrt(pow(cc->x - c->x, 2) + pow(cc->y - c->y, 2));
}

void Location::invalidateVisibilityCache(std::shared_ptr<Cell> cell) {
  auto vd = TORCH_DISTANCE;
  // fmt::print("--- {} ", visibilityCache.size());
  std::vector<std::pair<std::shared_ptr<Cell>, float>> hits;
  for (auto ls : cell->lightSources) {
    for (auto [lsk, _] : visibilityCache) {
      if (lsk.first != ls && lsk.first != player->currentCell)
        continue;
      // fmt::print("cache hit: {}.{} - {}\n", lsk.first->x, lsk.first->y,
      // lsk.second);
      hits.push_back(lsk);
    }
  }
  for (auto lsk : hits) {
    auto lsi = visibilityCache.find(lsk);
    if (lsi != visibilityCache.end()) {
      // fmt::print(".");
      visibilityCache.erase(lsi);
    }
  }
  // fmt::print(" {} ---\n", visibilityCache.size());
}

void Location::onEvent(DoorOpenedEvent &e) {
  fmt::print("on door open\n");
  needUpdateLight = true;
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
  // std::cout << "onCommit: " << rang::fg::yellow << "location"
  //           << rang::style::reset << ": " << rang::fg::green << ms.count()
  //           << rang::style::reset << '\n';

  LocationChangeEvent ec(nullptr);
  eb::EventBus::FireEvent(ec);
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
        auto new_item = item->roll();
        new_item->currentCell = enemy->currentCell;
        objects.push_back(new_item);
      }
    }
    invalidateVisibilityCache(enemy->currentCell);
    objects.erase(std::remove(objects.begin(), objects.end(), sender),
                  objects.end());
  }

  needUpdateLight = true;
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
  needUpdateLight = true;
};

void Location::onEvent(DigEvent &e) {
  e.cell->type = CellType::FLOOR;
  e.cell->passThrough = true;
  e.cell->seeThrough = true;

  auto rock = std::make_shared<Item>(ItemType::ROCK);
  rock->currentCell = e.cell;
  objects.push_back(rock);

  for (auto c : getNeighbors(e.cell)) {
    c->type = CellType::WALL;
  }
  needUpdateLight = true;
}

ItemsFoundEvent::ItemsFoundEvent(eb::ObjectPtr s, Objects i)
    : eb::Event(s), items(i) {}
void Location::onEvent(EnterCellEvent &e) {
  if (auto hero = std::dynamic_pointer_cast<Player>(e.getSender())) {
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
  }
  // if (e.cell->illuminated) {
  invalidateVisibilityCache(e.cell);
  needUpdateLight = true;
  // }
}
void Location::onEvent(LeaveCellEvent &e) { invalidateVisibilityCache(e.cell); }

void Location::enter(std::shared_ptr<Player> hero, std::shared_ptr<Cell> cell) {
  needUpdateLight = true;
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

  hero->commit("location enter", 0);

  handlers.push_back(eb::EventBus::AddHandler<CommitEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<EnemyDiedEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<ItemTakenEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<EnterCellEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<LeaveCellEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<DigEvent>(*this, hero));
  handlers.push_back(eb::EventBus::AddHandler<DropEvent>(*this));
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
  if (!needUpdateLight)
    return;
  auto t0 = std::chrono::system_clock::now();
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
      if (c->type == CellType::UNKNOWN)
        continue;
      c->lightSources.clear();
      c->seeThrough = c->type.seeThrough;
      if (std::find_if(objects.begin(), objects.end(),
                       [c](std::shared_ptr<Object> o) {
                         return o->currentCell == c && !o->seeThrough;
                       }) != objects.end() ||
          c == player->currentCell) {
        c->seeThrough = false;
      }
      c->passThrough = c->type.passThrough;
      if (std::find_if(objects.begin(), objects.end(),
                       [c](std::shared_ptr<Object> o) {
                         return o->currentCell == c && !o->passThrough;
                       }) != objects.end() ||
          c == player->currentCell) {
        c->passThrough = false;
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
      c->lightSources.push_back(t);
      c->illuminated = true;
    }
  }
  auto t1 = std::chrono::system_clock::now();
  using milliseconds = std::chrono::duration<double, std::milli>;
  milliseconds ms = t1 - t0;
  // std::cout << rang::fg::yellow << "update light" << rang::style::reset << ":
  // "
  // << rang::fg::green << ms.count() << rang::style::reset << '\n';
  needUpdateLight = false;
}

void Location::updateView(std::shared_ptr<Player> hero) {
  updateLight(hero);
  for (auto r : cells) {
    for (auto c : r) {
      if (c->type == CellType::UNKNOWN)
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

  for (auto n : getNeighbors(cell)) {
    if (!n->passThrough && (player == nullptr || n != player->currentCell))
      continue;
    micropather::StateCost nodeCost = {
        (void *)&(*n),
        LeastCostEstimate(state, (void *)&(*n)),
    };
    neighbors->push_back(nodeCost);
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

std::vector<std::shared_ptr<Cell>>
Location::getVisible(std::shared_ptr<Cell> start, float distance) {
  auto it = visibilityCache.find({start, distance});
  if (it != visibilityCache.end()) {
    return visibilityCache[{start, distance}];
  }
  // fmt::print("cache miss for {}.{} : {}\n", start->x, start->y, distance);
  std::vector<std::shared_ptr<Cell>> result;
  fov::Vec creaturePoint{start->x, start->y};
  fov::Vec bounds{(int)cells.front().size(), (int)cells.size()};
  start->seeThrough = true;
  auto field = fov::refresh(creaturePoint, bounds, cells);
  // fmt::print("{}.{} : {}\n", start->x, start->y, field.size());
  for (auto v : field) {
    auto c = cells[v.y][v.x];
    auto d = sqrt(pow(start->x - c->x, 2) + pow(start->y - c->y, 2));
    if (c->illuminated || d <= distance) {
      result.push_back(c);
    }
  }
  start->seeThrough = false;
  visibilityCache[{start, distance}] = result;
  return result;
}
