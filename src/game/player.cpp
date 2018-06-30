#include "lss/game/player.hpp"
#include "EventBus.hpp"
#include "lss/commands.hpp"
#include "lss/game/costs.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/slot.hpp"
#include "lss/game/spell.hpp"
#include <memory>

CommitEvent::CommitEvent(eb::ObjectPtr s, int ap)
    : eb::Event(s), actionPoints(ap) {}
DigEvent::DigEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c)
    : eb::Event(s), cell(c) {}
HeroDiedEvent::HeroDiedEvent(eb::ObjectPtr s)
    : eb::Event(s) {}

HeroTakeDamageEvent::HeroTakeDamageEvent(eb::ObjectPtr s, int d)
    : eb::Event(s), damage(d) {}

Player::Player() : Creature() {
  eb::EventBus::AddHandler<MoveCommandEvent>(*this);
  eb::EventBus::AddHandler<WalkCommandEvent>(*this);
  eb::EventBus::AddHandler<PickCommandEvent>(*this);
  eb::EventBus::AddHandler<DigCommandEvent>(*this);
  eb::EventBus::AddHandler<AttackCommandEvent>(*this);
  eb::EventBus::AddHandler<EquipCommandEvent>(*this);
  eb::EventBus::AddHandler<UnEquipCommandEvent>(*this);
  eb::EventBus::AddHandler<DropCommandEvent>(*this);
  eb::EventBus::AddHandler<WaitCommandEvent>(*this);
  eb::EventBus::AddHandler<ZapCommandEvent>(*this);

  equipment = std::make_shared<Equipment>();
  equipment->slots = {
      std::make_shared<Slot>("Head", std::vector<WearableType>{HEAD}),
      std::make_shared<Slot>("Neck", std::vector<WearableType>{NECK}),
      std::make_shared<Slot>("Cloak", std::vector<WearableType>{CLOAK}),
      std::make_shared<Slot>("Right pauldron",
                             std::vector<WearableType>{RIGHT_PAULDRON}),
      std::make_shared<Slot>("Left pauldron",
                             std::vector<WearableType>{LEFT_PAULDRON}),
      std::make_shared<Slot>(
          "Right hand", std::vector<WearableType>{WEAPON, WEAPON_LIGHT,
                                                  WEAPON_TWOHANDED, SHIELD}),
      std::make_shared<Slot>("Left hand",
                             std::vector<WearableType>{WEAPON_LIGHT, SHIELD}),
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

  hp = 20;
  hp_max = 20;
  damage_dices = 1;
  damage_edges = 1;
  damage_modifier = 0;
  visibilityDistance = 3.f;

  name = "Unnamed hero";

  auto dagger = std::make_shared<Item>(
      ItemType::DAGGER, Effects{std::make_shared<MeleeDamage>(1, 4, 0)});
  inventory.push_back(dagger);
  auto torch = std::make_shared<Item>(
      ItemType::TORCH, Effects{std::make_shared<VisibilityModifier>(5.f)});
  inventory.push_back(torch);
}

void Player::commit(int ap) {
  auto ptr = shared_from_this();
  CommitEvent e(ptr, ap);
  eb::EventBus::FireEvent(e);
}

bool Player::unequip(std::shared_ptr<Slot> slot) {
  for (auto e : slot->item->effects) {
    e->undo(this);
  }
  equipment->unequip(slot);
  return true;
}

bool Player::haveLight() {
  return std::find_if(inventory.begin(), inventory.end(),
                   [](std::shared_ptr<Item> item) {
                     return item->type == ItemType::TORCH && item->equipped;
                   }) != inventory.end();
}

bool Player::equip(std::shared_ptr<Slot> slot, std::shared_ptr<Item> item) {
  // auto ptr = shared_from_this();
  if (equipment->equip(slot, item)) {
    calcViewField();
    commit(ap_cost::EQUIP / SPEED(this));
    return true;
  }
  return false;

  // ItemTakenEvent e(ptr, item);
  // eb::EventBus::FireEvent(e);
}

void Player::onEvent(MoveCommandEvent &e) {
  if (move(e.direction, true)) {
    commit(ap_cost::STEP / SPEED(this));
  } else {
    commit(0);
  }
}

void Player::onEvent(AttackCommandEvent &e) {
  if (attack(e.direction)) {
    commit(ap_cost::ATTACK / SPEED(this));
  }
}

void Player::onEvent(DropCommandEvent &e) {
  if (e.item == nullptr)
    return;
  drop(e.item);
  commit(ap_cost::DROP / SPEED(this));
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

  auto cell = getCell(e.direction);

  DigEvent de(shared_from_this(), cell);
  eb::EventBus::FireEvent(de);
}

void Player::onEvent(WalkCommandEvent &e) {
  while (move(e.direction)) {
    commit(ap_cost::STEP / SPEED(this));
    auto item = std::find_if(currentLocation->objects.begin(),
                             currentLocation->objects.end(),
                             [&](std::shared_ptr<Object> o) {
                               return std::dynamic_pointer_cast<Item>(o) &&
                                      o->currentCell == currentCell;
                             });
    if (item != currentLocation->objects.end())
      break;
  }
}

void Player::onEvent(PickCommandEvent &e) {
  auto item = std::find_if(currentLocation->objects.begin(),
                           currentLocation->objects.end(),
                           [&](std::shared_ptr<Object> o) {
                             return (std::dynamic_pointer_cast<Item>(o) || std::dynamic_pointer_cast<TorchStand>(o)) &&
                                    o->currentCell == currentCell;
                           });
  if (item != currentLocation->objects.end()) {
    if (std::dynamic_pointer_cast<TorchStand>(*item)) {
      auto torch = std::make_shared<Item>(
        ItemType::TORCH, Effects{std::make_shared<VisibilityModifier>(5.f)});
      pick(torch);
      currentLocation->objects.erase(std::remove(currentLocation->objects.begin(), currentLocation->objects.end(), *item),
                currentLocation->objects.end());
    } else {
      pick(std::dynamic_pointer_cast<Item>(*item));
    }
    commit(ap_cost::PICK / SPEED(this));
  }
}

void Player::onEvent(EquipCommandEvent &e) {
  if (e.item == nullptr)
    return;
  fmt::print("Equipment equip: {} --> {}\n", e.item->type.name, e.slot->name);
  equip(e.slot, e.item);
}

void Player::onEvent(UnEquipCommandEvent &e) { unequip(e.slot); }
void Player::onEvent(WaitCommandEvent &e) { commit(ap_cost::STEP); }
void Player::onEvent(ZapCommandEvent &e) {
  if (e.spell == nullptr)
    return;
  commit(e.spell->cost);
}

bool Player::interact(std::shared_ptr<Object> actor) {
  auto enemy = std::dynamic_pointer_cast<Enemy>(actor);
  auto ptr = shared_from_this();
  if (hp > 0) {
    auto damage = enemy->getDamage(shared_from_this());
    hp -= damage;
    HeroTakeDamageEvent e(ptr, damage);
    eb::EventBus::FireEvent(e);
  }
  if (hp <= 0) {
    passThrough = true;
    HeroDiedEvent e2(ptr);
    // MessageEvent e2(ptr, "YOU DIED!!");
    eb::EventBus::FireEvent(e2);
  }
  commit(0);
  return hp > 0;
}
