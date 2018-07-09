#include <cmath>

#include "lss/game/creature.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/events.hpp"
#include "lss/game/fov.hpp"
#include "lss/game/player.hpp"

#include "EventBus.hpp"
#include "fmt/format.h"

Creature::Creature() {
  passThrough = false;
  seeThrough = false;
}

float Attribute::operator()(Creature *c) {
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
  case AttributeType::DEFENSE:
    base = c->defense;
    break;
  }

  for (auto s : c->equipment->slots) {
    if (s->item == nullptr ||
        std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                  s->item->type.wearableType) == s->acceptTypes.end()) {
      if (s->item != nullptr) {
        // fmt::print("Skip item {} in {}\n", s->item->name, s->name);
      }
      continue;
    }
    for (auto e : s->item->effects) {
      if (e->type != type)
        continue;
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

std::shared_ptr<Cell> Creature::getCell(std::shared_ptr<Cell> cc, Direction d) {
  auto cells = currentLocation->cells;
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

std::optional<std::tuple<std::shared_ptr<Slot>, int, int, int>>
Creature::getPrimaryDmg() {

  auto primarySlot = std::find_if(
      equipment->slots.begin(), equipment->slots.end(),
      [](std::shared_ptr<Slot> s) {
        return s->item != nullptr &&
               std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                         WEAPON) != s->acceptTypes.end();
      });
  if (primarySlot != equipment->slots.end()) {
    auto primaryWeapon = (*primarySlot)->item;
    auto meleeDmg = std::find_if(
        primaryWeapon->effects.begin(), primaryWeapon->effects.end(),
        [](std::shared_ptr<Effect> e) {
          return e->special && std::dynamic_pointer_cast<MeleeDamage>(e);
        });
    if (meleeDmg != primaryWeapon->effects.end()) {
      auto dmg = std::dynamic_pointer_cast<MeleeDamage>(*meleeDmg);
      return std::make_tuple(*primarySlot, dmg->modifier, dmg->dices,
                             dmg->edges);
    }
  }

  return std::nullopt;
}

std::optional<std::tuple<std::shared_ptr<Slot>, int, int, int>>
Creature::getSecondaryDmg(std::shared_ptr<Slot> primarySlot) {
  if (primarySlot == nullptr) {
    primarySlot = *std::find_if(
        equipment->slots.begin(), equipment->slots.end(),
        [](std::shared_ptr<Slot> s) {
          return std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                           WEAPON) != s->acceptTypes.end();
        });
  }
  auto secondarySlot = std::find_if(
      equipment->slots.begin(), equipment->slots.end(),
      [primarySlot](std::shared_ptr<Slot> s) {
        return s->item != nullptr && s != primarySlot &&
               std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                         WEAPON_LIGHT) != s->acceptTypes.end();
      });
  if (secondarySlot == equipment->slots.end()) {
    return std::nullopt;
  }
  auto secondaryWeapon = (*secondarySlot)->item;

  auto secondaryMeleeDmg = std::find_if(
      secondaryWeapon->effects.begin(), secondaryWeapon->effects.end(),
      [](std::shared_ptr<Effect> e) {
        return e->special && std::dynamic_pointer_cast<MeleeDamage>(e);
      });
  if (secondaryMeleeDmg != secondaryWeapon->effects.end()) {
    auto dmg = std::dynamic_pointer_cast<MeleeDamage>(*secondaryMeleeDmg);
    return std::make_tuple(*secondarySlot, dmg->modifier, dmg->dices,
                           dmg->edges);
  }

  return std::nullopt;
}

int criticalHit(int m, int d, int e) {
  auto damage = m + d * e;
  if (damage < 0) {
    damage = 0;
  }
  fmt::print("critical hit: {}\n", damage);
  return damage;
}

int Creature::hitRoll(int m, int d, int e) {
  auto inShadow = !currentCell->illuminated;
  if (inShadow && hasTrait(Traits::DEADLY_SHADOWS)) {
    return criticalHit(m, d, e);
  }
  auto damage = 0;
  for (auto n = 0; n < d; n++) {
    damage += rand() % e + 1;
  }
  damage += m;
  if (damage < 0) {
    damage = 0;
  }
  fmt::print("hit roll: {}\n", damage);
  return damage;
}

int Creature::getDamage(std::shared_ptr<Object>) {
  int damage = 0;
  auto primaryDmg = getPrimaryDmg();
  if (primaryDmg != std::nullopt) {
    auto [primarySlot, m, d, e] = *primaryDmg;
    damage += hitRoll(m, d, e);
  }
  auto haveLeft =
      std::count_if(
          equipment->slots.begin(), equipment->slots.end(),
          [](std::shared_ptr<Slot> s) {
            return s->item != nullptr &&
                   s->item->type.wearableType !=
                       WearableType::WEAPON_TWOHANDED &&
                   std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                             WEAPON_LIGHT) != s->acceptTypes.end() &&
                   std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                             WEAPON) == s->acceptTypes.end();
          }) > 0;
  auto secondaryDmg = getSecondaryDmg(nullptr);
  if (secondaryDmg != std::nullopt && haveLeft) {
    auto [secondarySlot, m, d, e] = *secondaryDmg;
    if (hasTrait(Traits::DUAL_WIELD)) {
      damage += hitRoll(m, d, e);
    } else {
      damage += m;
    }
  }
  if (damage == 0) {
    damage = hitRoll(damage_modifier, damage_dices, damage_edges);
  }
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
      it != inventory.end() && item != 0 && item->count != 0) {
    (*it)->count += item->count;
  } else {
    inventory.push_back(item);
  }

  return true;
}

bool Creature::attack(Direction d) {
  auto nc = getCell(currentCell, d);
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

std::shared_ptr<Slot> Creature::getSlot(WearableType type) {
  return *std::find_if(equipment->slots.begin(), equipment->slots.end(),
                       [type](std::shared_ptr<Slot> s) {
                         return std::find(s->acceptTypes.begin(),
                                          s->acceptTypes.end(),
                                          type) != s->acceptTypes.end();
                       });
}

std::optional<std::shared_ptr<Slot>> Creature::getSlot(WearableType type,
                                                       bool busy) {
  auto slot = std::find_if(
      equipment->slots.begin(), equipment->slots.end(),
      [type, busy](std::shared_ptr<Slot> s) {
        return (busy ? s->item != nullptr : s->item == nullptr) &&
               std::find(s->acceptTypes.begin(), s->acceptTypes.end(), type) !=
                   s->acceptTypes.end();
      });
  if (slot != equipment->slots.end()) {
    return *slot;
  }
  return std::nullopt;
}

bool Creature::move(Direction d, bool autoAction) {
  auto cc = currentCell;
  auto nc = getCell(currentCell, d);
  auto obstacle = std::find_if(currentLocation->objects.begin(),
                               currentLocation->objects.end(),
                               [&](std::shared_ptr<Object> o) {
                                 return o->currentCell == nc && !o->passThrough;
                               });
  auto hasObstacles = obstacle != currentLocation->objects.end();
  auto hasPlayer = currentLocation->player->currentCell == nc;
  // fmt::print("{} - {} - {}.{} -> {}.{}\n", d, hasObstacles, cc->x, cc->y,
  // nc->x, nc->y);
  if (!nc->canPass(traits) || hasObstacles || hasPlayer) {
    if (autoAction && hasObstacles && !(*obstacle)->passThrough) {
      if (std::dynamic_pointer_cast<Enemy>(*obstacle)) {
        return attack(d);
      }
      return !(*obstacle)->interact(shared_from_this());
    }
    return false;
  }

  currentCell = nc;

  LeaveCellEvent le(shared_from_this(), cc);
  eb::EventBus::FireEvent(le);
  EnterCellEvent ee(shared_from_this(), currentCell);
  eb::EventBus::FireEvent(ee);
  calcViewField();
  return true;
}

// FIXME: fix doors!
void Creature::calcViewField(bool force) {
  // if (cachedCell == currentCell && !force) {
  //   return;
  // }
  currentLocation->updateLight(currentLocation->player);
  auto vd = VISIBILITY_DISTANCE(this);
  if (hasTrait(Traits::NIGHT_VISION)) {
    vd = 1000;
  }
  viewField = currentLocation->getVisible(currentCell, vd);

  // cachedCell = currentCell;
  for (auto c : viewField) {
    c->lightSources.push_back(currentCell);
  }
}

bool Creature::hasLight() {
  return std::find_if(inventory.begin(), inventory.end(),
                      [](std::shared_ptr<Item> item) {
                        return item->type.wearableType == WearableType::LIGHT &&
                               item->equipped;
                      }) != inventory.end();
}
