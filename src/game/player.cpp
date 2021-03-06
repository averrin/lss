#include <chrono>
#include <memory>
#include <rang.hpp>

#include "EventBus.hpp"
#include "lss/commands.hpp"
#include "lss/game/costs.hpp"
#include "lss/game/damage.hpp"
#include "lss/game/door.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/player.hpp"
#include "lss/game/slot.hpp"
#include "lss/game/spell.hpp"
#include "lss/game/terrain.hpp"
#include "lss/game/trigger.hpp"
#include "lss/utils.hpp"

DigEvent::DigEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c)
    : eb::Event(s), cell(c) {}
HeroDiedEvent::HeroDiedEvent(eb::ObjectPtr s) : eb::Event(s) {}

HeroTakeDamageEvent::HeroTakeDamageEvent(eb::ObjectPtr s,
                                         std::shared_ptr<Damage> d)
    : eb::Event(s), damage(d) {}

Player::~Player() { clearHandlers(); }

Player::Player() : Creature() {
  handlers.push_back(eb::EventBus::AddHandler<MoveCommandEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<WalkCommandEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<PickCommandEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<DigCommandEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<AttackCommandEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<EquipCommandEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<UnEquipCommandEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<DropCommandEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<WaitCommandEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<ZapCommandEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<EnemyDiedEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<ThrowCommandEvent>(*this));

  equipment = std::make_shared<Equipment>();
  auto right_hand_slot = std::make_shared<Slot>(
      "Right hand", std::vector<WearableType>{WEAPON, WEAPON_LIGHT,
                                              WEAPON_TWOHANDED, SHIELD});
  auto left_hand_slot = std::make_shared<Slot>(
      "Left hand", std::vector<WearableType>{WEAPON_LIGHT, SHIELD});
  equipment->slots = {
      std::make_shared<Slot>("Head", std::vector<WearableType>{HEAD}),
      std::make_shared<Slot>("Neck", std::vector<WearableType>{NECK}),
      std::make_shared<Slot>("Cloak", std::vector<WearableType>{CLOAK}),
      std::make_shared<Slot>("Right pauldron",
                             std::vector<WearableType>{RIGHT_PAULDRON}),
      std::make_shared<Slot>("Left pauldron",
                             std::vector<WearableType>{LEFT_PAULDRON}),
      right_hand_slot,
      left_hand_slot,
      std::make_shared<Slot>("Right gauntlet",
                             std::vector<WearableType>{RIGHT_GAUNTLET}),
      std::make_shared<Slot>("Left gauntlet",
                             std::vector<WearableType>{LEFT_GAUNTLET}),
      std::make_shared<Slot>("Right ring", std::vector<WearableType>{RING}),
      std::make_shared<Slot>("Left ring", std::vector<WearableType>{RING}),
      std::make_shared<Slot>("Body", std::vector<WearableType>{BODY}),
      std::make_shared<Slot>("Greaves", std::vector<WearableType>{GREAVES}),
      std::make_shared<Slot>("Boots", std::vector<WearableType>{BOOTS}),
      std::make_shared<Slot>("Light", std::vector<WearableType>{LIGHT})};

  hp_max = 100;
  hp = hp_max;
  mp_max = 50;
  mp = mp_max;
  dmgSpec = DamageSpec(0, 1, 3, DamageType::WEAPON);
  throw_distance = 5;

  activeEffects.push_back(OverTimeEffects::MANA_RESTORE);

  name = "Unnamed hero";

  auto sword = Prototype::BASIC_SWORD->clone();
  inventory.push_back(sword);
  inventory.push_back(Prototype::TORCH->clone());
  inventory.push_back(Prototype::POTION_HEAL_LESSER->clone());
  auto armor = Prototype::BASIC_LEATHER_CUIRASS->clone();
  inventory.push_back(armor);

  auto knives = Prototype::THROWING_KNIVES->clone();
  knives->count = 5;
  inventory.push_back(knives);

  auto questItem = Prototype::QUEST_ITEM->clone();
  questItem->count = 0;
  inventory.push_back(questItem);

  getSlot(sword->type.wearableType)->equip(sword);
  getSlot(armor->type.wearableType)->equip(armor);

  // inventory.push_back(axe);
}

void Player::commitWaited(std::string reason) {
  commit(reason, waitedAP);
  waitedAP = 0;
}

void Player::commit(std::string reason, int ap, bool s) {
  if (hasLight() && !hasTrait(Traits::MAGIC_TORCH)) {
    auto lightSlot = getSlot(WearableType::LIGHT);
    if (lightSlot->item != nullptr && lightSlot->item->durability != -1) {
      lightSlot->item->durability -= ap / 10;
      if (lightSlot->item->durability <= 0) {
        lightSlot->item->durability = 0;
        equipment->unequip(lightSlot);
      }
    }
  }

  if (HP(this) / HP_MAX(this) < 0.2f) {
    currentCell->addFeature(CellFeature::BLOOD);
  }

  auto label = fmt::format("commit {} [{}]", lu::blue(reason),
                           lu::magenta(fmt::format("{}", ap)));
  log.start(lu::blue("HERO"), label);
  auto ptr = shared_from_this();
  CommitEvent e(ptr, ap, s);
  eb::EventBus::FireEvent(e);
  log.stop(label);
}

bool Player::unequip(std::shared_ptr<Slot> slot) {
  equipment->unequip(slot);
  commit("unequip", ap_cost::UNEQUIP / SPEED(this));
  return true;
}

bool Player::equip(std::shared_ptr<Item> item) { return false; }

bool Player::equip(std::shared_ptr<Slot> slot, std::shared_ptr<Item> item) {
  // auto ptr = shared_from_this();
  if (equipment->equip(slot, item)) {
    commit("equip", ap_cost::EQUIP / SPEED(this));
    return true;
  }
  return false;

  // ItemTakenEvent e(ptr, item);
  // eb::EventBus::FireEvent(e);
}

void Player::onEvent(MoveCommandEvent &e) {

  if (hasTrait(Traits::CONFUSED)) {
    auto ds =
        std::vector<std::string>{"e", "s", "w", "n", "nw", "ne", "sw", "se"};
    auto d = ds[rand() % ds.size()];
    e.direction = *utils::getDirectionByName(d);
  }
  auto res = move(e.direction, true);
  viewField = calcViewField();
  currentLocation->updateLight(currentLocation->player);
  currentLocation->updateView(currentLocation->player);
  commit("move", res ? ap_cost::STEP / SPEED(this) : 0);
}

void Player::onEvent(AttackCommandEvent &e) {
  if (hasTrait(Traits::CONFUSED)) {
    auto ds =
        std::vector<std::string>{"e", "s", "w", "n", "nw", "ne", "sw", "se"};
    auto d = ds[rand() % ds.size()];
    e.direction = *utils::getDirectionByName(d);
  }

  if (attack(e.direction)) {
    fmt::print("attack [{}]\n", ap_cost::ATTACK / SPEED(this));
    commit("attack", ap_cost::ATTACK / SPEED(this));
  }
}

void Player::onEvent(DropCommandEvent &e) {
  if (e.item == nullptr)
    return;
  drop(e.item);
  commit("drop", ap_cost::DROP / SPEED(this));
}

void Player::onEvent(DigCommandEvent &e) {
  if (std::find_if(inventory.begin(), inventory.end(),
                   [](std::shared_ptr<Item> item) {
                     return item->type == ItemType::PICK_AXE && item->equipped;
                   }) == inventory.end()) {
    MessageEvent me(shared_from_this(), "You need to equip pick axe");
    eb::EventBus::FireEvent(me);
    return;
  }

  auto cell = currentLocation->getCell(currentCell, e.direction);

  DigEvent de(shared_from_this(), *cell);
  eb::EventBus::FireEvent(de);
}

// TODO: fix stops parallel rooms (maybe count  < 3...)
void Player::onEvent(WalkCommandEvent &e) {
  if (hasTrait(Traits::CONFUSED)) {
    auto ds =
        std::vector<std::string>{"e", "s", "w", "n", "nw", "ne", "sw", "se"};
    auto d = ds[rand() % ds.size()];
    e.direction = *utils::getDirectionByName(d);
  }

  auto enemies = utils::castObjects<Enemy>(currentLocation->objects);
  auto step = 0;
  while (move(e.direction)) {
    viewField = calcViewField();
    currentLocation->updateLight(currentLocation->player);
    auto lastHP = HP(this);
    commit("walk move", ap_cost::STEP / SPEED(this), true);
    if (HP(this) != lastHP) {
      break;
    }
    auto item = std::find_if(currentLocation->objects.begin(),
                             currentLocation->objects.end(),
                             [&](std::shared_ptr<Object> o) {
                               return std::dynamic_pointer_cast<Item>(o) &&
                                      o->currentCell == currentCell;
                             });
    auto seeEnemy = std::find_if(enemies.begin(), enemies.end(),
                                 [&](std::shared_ptr<Enemy> e) {
                                   return canSee(e->currentCell);
                                 }) != enemies.end();
    auto nc = currentLocation->getCell(currentCell, e.direction);
    if (!nc || item != currentLocation->objects.end() || seeEnemy ||
        currentCell->type != CellType::FLOOR ||
        ((*nc)->room != currentCell->room && step > 1)) {
      break;
    }
    auto nbrs = currentLocation->getNeighbors(currentCell);
    if (step > 1 &&
        std::find_if(nbrs.begin(), nbrs.end(), [&](std::shared_ptr<Cell> c) {
          return /*utils::castObjects<Door>(currentLocation->getObjects(c))
                     .size() > 0 || */
              (c->type == CellType::FLOOR && c->room != currentCell->room);
        }) != nbrs.end()) {
      break;
    }
    step++;
  }
  currentLocation->updateView(currentLocation->player);
  commit("walk end", 0);
}

void Player::onEvent(PickCommandEvent &e) {
  auto item = std::find_if(currentLocation->objects.begin(),
                           currentLocation->objects.end(),
                           [&](std::shared_ptr<Object> o) {
                             return (std::dynamic_pointer_cast<Item>(o) ||
                                     std::dynamic_pointer_cast<Terrain>(o)) &&
                                    o->currentCell == currentCell;
                           });
  if (item != currentLocation->objects.end()) {
    if (auto t = std::dynamic_pointer_cast<Terrain>(*item); t) {
      for (auto trg : t->triggers) {
        auto trigger = std::dynamic_pointer_cast<PickTrigger>(trg);
        if (trigger) {
          auto ptr = shared_from_this();
          trigger->activate(std::dynamic_pointer_cast<Player>(ptr));
        }
      }
    } else {
      pick(std::dynamic_pointer_cast<Item>(*item));
    }
    commit("pick", ap_cost::PICK / SPEED(this));
  }
}

void Player::onEvent(EquipCommandEvent &e) {
  if (e.item == nullptr)
    return;
  fmt::print("Equipment equip: {} --> {}\n", e.item->type.name, e.slot->name);
  equip(e.slot, e.item);
}

void Player::onEvent(UnEquipCommandEvent &e) { unequip(e.slot); }
void Player::onEvent(WaitCommandEvent &e) {
  commit("wait", ap_cost::STEP);
  commit("after wait", 0);
}
void Player::onEvent(ZapCommandEvent &e) {
  if (e.spell == nullptr)
    return;
  // TODO: do it before applying spell
  // commit("zap", e.spell->ap);
  waitedAP += e.spell->ap;
  mp -= e.spell->cost;
  commit("after zap", 0, true);
  increaseIntelligence(e.spell->cost * 4);
}

// TODO: move to creature
bool Player::interact(std::shared_ptr<Object> actor) {
  if (hasTrait(Traits::INVULNERABLE) || HP(this) <= 0)
    return true;
  auto enemy = std::dynamic_pointer_cast<Enemy>(actor);

  for (auto s : equipment->slots) {
    if (s->item == nullptr ||
        std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                  s->item->type.wearableType) == s->acceptTypes.end()) {
      continue;
    }
    for (auto e : s->item->effects) {
      if (auto ohe = std::dynamic_pointer_cast<OnHitEffect>(e)) {
        if (R::R() < ohe->probability) {
          enemy->activeEffects.push_back(ohe->effect);
        }
      }
    }
  }

  auto damage = enemy->getDamage(shared_from_this());
  applyDamage(enemy, damage);
  return hp > 0;
}

void Player::onDamage(std::shared_ptr<Creature> attacker,
                      std::shared_ptr<Damage> damage) {

  increaseStrength(damage->deflected);
  HeroTakeDamageEvent e(attacker, damage);
  eb::EventBus::FireEvent(e);
}

void Player::onDie() {
  auto ptr = shared_from_this();
  passThrough = true;
  HeroDiedEvent e2(ptr);
  eb::EventBus::FireEvent(e2);
  commit("death", 0);
}

void Player::onEvent(EnemyDiedEvent &e) {}

void Player::onEvent(ThrowCommandEvent &e) {
  if (e.item == nullptr)
    return;
  throwItem(e.item, e.cell);
  commit("throw", ap_cost::THROW / speed);
}

void Player::increaseIntelligence(float val) {
  intelligence += (val / 10000.f) * (currentLocation->depth + 1) * 1.5;
  if (intelligence >= 1.1 && std::find(traits.begin(), traits.end(),
                                       Traits::NOVICE_MAGE) == traits.end()) {
    traits.push_back(Traits::NOVICE_MAGE);
    mp_max += 30;
  }
  if (intelligence >= 1.2 &&
      std::find(traits.begin(), traits.end(), Traits::APPRENTICE_MAGE) ==
          traits.end()) {
    traits.push_back(Traits::APPRENTICE_MAGE);
    activeEffects.push_back(OverTimeEffects::MANA_RESTORE_LESSER);
  }
}

void Player::increaseStrength(float val) {
  strength += (val / 10000.f) * (currentLocation->depth + 1) * 1.5;
  if (strength >= 1.1 && std::find(traits.begin(), traits.end(),
                                   Traits::NATURAL_ARMOR) == traits.end()) {
    traits.push_back(Traits::NATURAL_ARMOR);
    defense += 3;
  }
  if (strength >= 1.2 && std::find(traits.begin(), traits.end(),
                                   Traits::DUAL_WIELD) == traits.end()) {
    traits.push_back(Traits::DUAL_WIELD);
  }
}

std::map<std::shared_ptr<Object>, std::set<std::shared_ptr<Cell>>>
Player::getLightMap() {
  std::map<std::shared_ptr<Object>, std::set<std::shared_ptr<Cell>>> lightMap;
  for (auto c : viewField) {
    if (!c->illuminated)
      continue;
    for (auto ls : c->lightSources) {
      if (lightMap.find(ls) == lightMap.end()) {
        lightMap[ls] = {};
      }
      lightMap[ls].insert(c);
    }
  }
  return lightMap;
}
