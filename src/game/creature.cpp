#include <cmath>
#include <rang.hpp>
#include <variant>
// #include <fmt/format.h>

#include "lss/game/creature.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/events.hpp"
#include "lss/game/fov.hpp"
#include "lss/game/player.hpp"
#include "lss/generator/room.hpp"
#include "lss/utils.hpp"

#include "EventBus.hpp"

float NIGHT_VISION_DISTANCE = 10;

Creature::Creature() {
  passThrough = false;
  seeThrough = false;

  zIndex = 2;
}

// TOD: implement attribute setter (for hp_boost spell)
float Attribute::operator()(Creature *c) {
  float base = 0;
  switch (type) {
  case AttributeType::VISIBILITY_DISTANCE:
    base = c->visibility_distance;
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
  case AttributeType::CRIT_CHANCE:
    base = c->crit_chance;
    break;
  case AttributeType::MP:
    base = c->mp;
    break;
  case AttributeType::MP_MAX:
    base = c->mp_max;
    break;
  }

  Effects effects;
  effects.insert(effects.end(), c->activeEffects.begin(),
                 c->activeEffects.end());
  for (auto s : c->equipment->slots) {
    if (s->item == nullptr ||
        std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                  s->item->type.wearableType) == s->acceptTypes.end()) {
      continue;
    }
    for (auto e : s->item->effects) {
      if (e->type != type)
        continue;
      effects.push_back(e);
    }
  }
  for (auto e : effects) {
    if (std::dynamic_pointer_cast<OverTimeEffect>(e))
      continue;
    auto mod = e->getModifier();
    if (auto m = std::get_if<int>(&mod)) {
      base += *m;
    } else if (auto m = std::get_if<float>(&mod)) {
      base += *m;
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

std::optional<std::tuple<std::shared_ptr<Slot>, DamageSpec>>
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
      return std::make_tuple(*primarySlot, dmg->dmgSpec);
    }
  }

  return std::nullopt;
}

std::optional<std::tuple<std::shared_ptr<Slot>, DamageSpec>>
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
    return std::make_tuple(*secondarySlot, dmg->dmgSpec);
  }

  return std::nullopt;
}

std::shared_ptr<Damage> Creature::updateDamage(std::shared_ptr<Damage> damage,
                                               DamageSpec spec) {
  auto inShadow = !currentCell->illuminated;
  float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  if (inShadow && hasTrait(Traits::DEADLY_SHADOWS)) {
    damage->traits.push_back(Traits::DEADLY_SHADOWS);
    damage->damage += spec.criticalHit();
    damage->isCritical = true;
    return damage;
  } else if (r < CRIT(this)) {
    damage->damage += spec.criticalHit();
    damage->isCritical = true;
    return damage;
  }
  damage->damage += spec.hitRoll();
  return damage;
}

std::shared_ptr<Damage> Creature::getDamage(std::shared_ptr<Object>) {
  // TODO: get damageType from creature
  auto damage =
      std::make_shared<Damage>(DamageSpec(0, 0, 0, DamageType::WEAPON));
  auto primaryDmg = getPrimaryDmg();
  if (primaryDmg != std::nullopt) {
    auto [primarySlot, spec] = *primaryDmg;
    damage = updateDamage(damage, spec);
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
    auto [secondarySlot, spec] = *secondaryDmg;
    if (hasTrait(Traits::DUAL_WIELD)) {
      damage = updateDamage(damage, spec);
    } else {
      damage->damage += spec.modifier;
    }
  }
  if (damage->damage == 0) {
    damage = updateDamage(damage, dmgSpec);
  }
  if (hasTrait(Traits::MOB)) {
    auto nbrs = currentLocation->getNeighbors(currentCell);
    if (std::find_if(nbrs.begin(), nbrs.end(), [&](std::shared_ptr<Cell> c) {
          auto enemies =
              utils::castObjects<Enemy>(currentLocation->getObjects(c));
          return enemies.size() > 0 && enemies.front()->hasTrait(Traits::MOB);
        }) != nbrs.end()) {
      damage->traits.push_back(Traits::MOB);
    }
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
                               return item->getTitle(true) == i->getTitle(true);
                             });
      it != inventory.end() && item != 0 && item->count != 0) {
    (*it)->count += item->count;
  } else {
    item->identified = isIdentified(item);
    inventory.push_back(item);
  }

  return true;
}

bool Creature::attack(Direction d) {
  auto nc = currentLocation->getCell(currentCell, d);
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

// TODO: apply durning effect with non-zero fire damage
void Creature::applyDamage(std::shared_ptr<Object> a,
                           std::shared_ptr<Damage> damage) {
  auto attacker = std::dynamic_pointer_cast<Creature>(a);
  auto def = R::Z(0, DEF(this));
  if (damage->spec.type == DamageType::ACID && !hasTrait(Traits::ACID_IMMUNE)) {
    def /= 2;
  }
  if (damage->damage - def < 0) {
    damage->damage = 0;
  } else {
    damage->damage -= def;
  }
  damage->deflected = def;

  // TODO: declarative
  if (damage->spec.type == DamageType::ACID && hasTrait(Traits::ACID_IMMUNE)) {
    damage->damage = 0;
  }
  if (damage->spec.type == DamageType::FIRE && hasTrait(Traits::FIRE_IMMUNE)) {
    damage->damage = 0;
  }
  if (damage->spec.type == DamageType::ACID && hasTrait(Traits::ACID_RESIST)) {
    damage->damage /= 2;
  }
  if (damage->spec.type == DamageType::FIRE && hasTrait(Traits::FIRE_RESIST)) {
    damage->damage /= 2;
  }

  if (damage->spec.type == DamageType::WEAPON &&
      hasTrait(Traits::WEAPON_IMMUNE)) {
    damage->damage = 0;
  }
  if (damage->spec.type == DamageType::MAGIC &&
      hasTrait(Traits::MAGIC_IMMUNE)) {
    damage->damage = 0;
  }
  if (damage->spec.type == DamageType::WEAPON &&
      hasTrait(Traits::WEAPON_RESIST)) {
    damage->damage /= 2;
  }
  if (damage->spec.type == DamageType::MAGIC &&
      hasTrait(Traits::MAGIC_RESIST)) {
    damage->damage /= 2;
  }

  hp -= damage->damage;
  onDamage(attacker, damage);
  if (hp <= 0) {
    onDie();
  }
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
  auto nc = currentLocation->getCell(currentCell, d);

  if (hasTrait(Traits::JUMPY) && R::R() < 0.01) {
    auto room = currentLocation->rooms[rand() % currentLocation->rooms.size()];
    nc = room->cells[rand() % room->cells.size()];

    MessageEvent me(nullptr, "Your feel jumpy.");
    eb::EventBus::FireEvent(me);
  }

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
    vd = NIGHT_VISION_DISTANCE;
  }
  viewField = currentLocation->getVisible(currentCell, vd);

  // cachedCell = currentCell;
  for (auto c : viewField) {
    c->lightSources.insert(shared_from_this());
  }
}

// TODO: migrate to emit
bool Creature::hasLight() {
  return std::find_if(inventory.begin(), inventory.end(),
                      [](std::shared_ptr<Item> item) {
                        return item->type.wearableType == WearableType::LIGHT &&
                               item->equipped;
                      }) != inventory.end();
}

void Creature::applyEoT(EoT eot, int modifier) {
  switch (eot) {
  case EoT::HEAL: {
    hp += modifier;
    auto max = HP_MAX(this);
    if (hp > max)
      hp = max;
  } break;
  case EoT::MANA_RESTORE: {
    mp += modifier;
    auto max = MP_MAX(this);
    if (mp > max)
      mp = max;
  } break;
  case EoT::POISON:
    hp -= modifier;
    break;
  }
}

// TODO: get not vissible, but reachable
std::vector<std::shared_ptr<Cell>> Creature::getInRadius(float distance) {
  std::vector<std::shared_ptr<Cell>> cells(viewField.size());
  auto it = std::copy_if(
      viewField.begin(), viewField.end(), cells.begin(), [&](auto c) {
        auto d =
            sqrt(pow(c->x - currentCell->x, 2) + pow(c->y - currentCell->y, 2));
        return c != currentCell && d <= distance && c->type.passThrough;
      });
  cells.resize(std::distance(cells.begin(), it));
  return cells;
}

// TODO: apply effect on throw
bool Creature::throwItem(std::shared_ptr<Item> item,
                         std::shared_ptr<Cell> cell) {
  std::shared_ptr<Item> throwed;
  if (item->count == 0) {
    throwed = item;
    inventory.erase(std::remove(inventory.begin(), inventory.end(), item),
                    inventory.end());
  } else {
    item->count--;
    throwed = item->clone();
    throwed->count = 1;
  }
  currentLocation->objects.push_back(throwed);
  auto cells = currentLocation->getLine(currentCell, cell);
  auto a = std::make_shared<MoveAnimation>(throwed, cells, cells.size());
  a->animationCallback = [=]() {
    auto e =
        std::find_if(item->effects.begin(), item->effects.end(), [](auto ef) {
          return std::dynamic_pointer_cast<MeleeDamage>(ef);
        });
    if (e != item->effects.end()) {
      auto objects = currentLocation->getObjects(cell);
      for (auto o : objects) {
        if (auto creature = std::dynamic_pointer_cast<Creature>(o)) {
          fmt::print("apply t d\n");
          creature->applyDamage(
              shared_from_this(),
              std::dynamic_pointer_cast<MeleeDamage>(*e)->dmgSpec.getDamage());
        }
      }
    }
  };
  AnimationEvent ae(a);
  eb::EventBus::FireEvent(ae);
  return true;
}
