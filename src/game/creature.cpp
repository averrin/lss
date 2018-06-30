#include <cmath>

#include "lss/game/creature.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/events.hpp"
#include "lss/game/player.hpp"
#include "lss/game/fov.hpp"

#include "EventBus.hpp"
#include "fmt/format.h"

Creature::Creature() { passThrough = false; }

float Attribute::operator()(Creature* c) {
  float base = 0;
  switch (type) {
    case AttributeType::VISIBILITY_DISTANCE:
      base = c->visibilityDistance;
      break;
    case AttributeType::SPEED:
      base = c->speed;
      break;
    case AttributeType::HP:
      base = c->hp;
      break;
    case AttributeType::HP_MAX:
      base = c->hp_max;
      break;
  }

  for (auto s : c->equipment->slots) {
    if (s->item == nullptr) continue;
    for (auto e : s->item->effects) {
      if (e->type != type) continue;
      base += e->modifier;
    }

  }
  return base;
}

bool Creature::interact(std::shared_ptr<Object> actor) { return false; }

LeaveCellEvent::LeaveCellEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c)
    : eb::Event(s), cell(c) {}
EnterCellEvent::EnterCellEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c)
    : eb::Event(s), cell(c) {}
DropEvent::DropEvent(eb::ObjectPtr s, std::shared_ptr<Item> i)
    : eb::Event(s), item(i) {}
ItemTakenEvent::ItemTakenEvent(eb::ObjectPtr s, std::shared_ptr<Item> i)
    : eb::Event(s), item(i) {}

std::shared_ptr<Cell> Creature::getCell(Direction d) {
  auto cells = currentLocation->cells;
  auto cc = currentCell;
  std::shared_ptr<Cell> cell;
  switch (d) {
  case N:
    cell = cells[cc->y - 1][cc->x];
    break;
  case E:
    cell = cells[cc->y][cc->x + 1];
    break;
  case S:
    cell = cells[cc->y + 1][cc->x];
    break;
  case W:
    cell = cells[cc->y][cc->x - 1];
    break;
  case NW:
    cell = cells[cc->y - 1][cc->x - 1];
    break;
  case NE:
    cell = cells[cc->y - 1][cc->x + 1];
    break;
  case SW:
    cell = cells[cc->y + 1][cc->x - 1];
    break;
  case SE:
    cell = cells[cc->y + 1][cc->x + 1];
    break;
  }
  return cell;
}

int Creature::getDamage(std::shared_ptr<Object>) {
  auto damage = 0;
  for (auto n = 0; n < damage_dices; n++) {
    damage += rand() % damage_edges + 1;
  }
  damage += damage_modifier;
  return damage;
}

bool Creature::drop(std::shared_ptr<Item> item) {
  inventory.erase(std::remove(inventory.begin(), inventory.end(), item),
                  inventory.end());
  DropEvent me(shared_from_this(), item);
  eb::EventBus::FireEvent(me);
  return true;
}

bool Creature::pick(std::shared_ptr<Item> item) {
  auto ptr = shared_from_this();

  ItemTakenEvent e(ptr, item);
  eb::EventBus::FireEvent(e);

  if (auto it = std::find_if(inventory.begin(), inventory.end(),
                             [item](std::shared_ptr<Item> i) {
                               return item->getTitle() == i->getTitle();
                             });
      it != inventory.end() && item != 0) {
    (*it)->count += item->count;
  } else {
    inventory.push_back(item);
  }

  return true;
}

bool Creature::attack(Direction d) {
  auto nc = getCell(d);
  auto opit = std::find_if(
      currentLocation->objects.begin(), currentLocation->objects.end(),
      [&](std::shared_ptr<Object> o) {
        return o->currentCell == nc && std::dynamic_pointer_cast<Creature>(o);
      });
  if (opit == currentLocation->objects.end() &&
      currentLocation->player->currentCell != nc) {
    MessageEvent me(shared_from_this(), "There is no target.");
    eb::EventBus::FireEvent(me);
    return false;
  }
  auto opponent =
      opit == currentLocation->objects.end() ? currentLocation->player : *opit;
  opponent->interact(shared_from_this());
  return true;
}

bool Creature::move(Direction d, bool autoAction) {
  auto cc = currentCell;
  auto nc = getCell(d);
  auto obstacle = std::find_if(currentLocation->objects.begin(),
                               currentLocation->objects.end(),
                               [&](std::shared_ptr<Object> o) {
                                 return o->currentCell == nc && !o->passThrough;
                               });
  auto hasObstacles = obstacle != currentLocation->objects.end();
  auto hasPlayer = currentLocation->player->currentCell == nc;
  // fmt::print("{} - {} - {}.{} -> {}.{}\n", d, hasObstacles, cc->x, cc->y,
  // nc->x, nc->y);
  if (!nc->passThrough || hasObstacles || hasPlayer) {
    if (autoAction && hasObstacles && !(*obstacle)->passThrough) {
      return !(*obstacle)->interact(shared_from_this());
    }
    return false;
  }

  currentCell = nc;
  calcViewField();

  LeaveCellEvent le(shared_from_this(), cc);
  eb::EventBus::FireEvent(le);
  EnterCellEvent ee(shared_from_this(), currentCell);
  eb::EventBus::FireEvent(ee);
  return true;
}

void Creature::calcViewField() {
  auto vd = VISIBILITY_DISTANCE(this);
  viewField.clear();
  std::vector<std::shared_ptr<Cell>> temp;
  viewField = currentLocation->getVisible(currentCell, vd);
}
