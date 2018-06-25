#include "lss/game/player.hpp"
#include "EventBus.hpp"
#include "lss/command.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/slot.hpp"
#include <memory>

CommitEvent::CommitEvent(eb::ObjectPtr s, int ap)
    : eb::Event(s), actionPoints(ap) {}
ItemTakenEvent::ItemTakenEvent(eb::ObjectPtr s, std::shared_ptr<Item> i)
    : eb::Event(s), item(i) {}
DigEvent::DigEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c)
    : eb::Event(s), cell(c) {}
DropEvent::DropEvent(eb::ObjectPtr s, std::shared_ptr<Item> i)
    : eb::Event(s), item(i) {}

Player::Player() : Creature() {
  eb::EventBus::AddHandler<MoveCommandEvent>(*this);
  eb::EventBus::AddHandler<WalkCommandEvent>(*this);
  eb::EventBus::AddHandler<PickCommandEvent>(*this);
  eb::EventBus::AddHandler<DigCommandEvent>(*this);
  eb::EventBus::AddHandler<AttackCommandEvent>(*this);
  eb::EventBus::AddHandler<EquipCommandEvent>(*this);
  eb::EventBus::AddHandler<UnEquipCommandEvent>(*this);
  eb::EventBus::AddHandler<DropCommandEvent>(*this);

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
          "Right hand",
          std::vector<WearableType>{WEAPON, WEAPON_TWOHANDED, SHIELD}),
      std::make_shared<Slot>("Left hand",
                             std::vector<WearableType>{WEAPON, SHIELD}),
      std::make_shared<Slot>("Right gauntlet",
                             std::vector<WearableType>{RIGHT_GAUNTLET}),
      std::make_shared<Slot>("Left gauntlet",
                             std::vector<WearableType>{LEFT_GAUNTLET}),
      std::make_shared<Slot>("Right ring", std::vector<WearableType>{RING}),
      std::make_shared<Slot>("Left ring", std::vector<WearableType>{RING}),
      std::make_shared<Slot>("Greaves", std::vector<WearableType>{GREAVES}),
      std::make_shared<Slot>("Boots", std::vector<WearableType>{BOOTS}),
      std::make_shared<Slot>("Light", std::vector<WearableType>{LIGHT})};

  hp = 20;
  hp_max = 20;
  damage_dices = 1;
  damage_edges = 1;
  damage_modifier = 0;
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

bool Player::equip(std::shared_ptr<Slot> slot, std::shared_ptr<Item> item) {
  // auto ptr = shared_from_this();
  fmt::print("Player equip: {}\n", item->type.name);
  equipment->equip(slot, item);
  for (auto e : item->effects) {
    e->apply(this);
  }

  // ItemTakenEvent e(ptr, item);
  // eb::EventBus::FireEvent(e);
  commit(200 / speed);
  return true;
}

bool Player::pick(std::shared_ptr<Item> item) {
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

  commit(200 / speed);
  return true;
}

void Player::onEvent(MoveCommandEvent &e) {
  if (move(e.direction, true)) {
    commit(1000 / speed);
  }
}

void Player::onEvent(AttackCommandEvent &e) {
  if (attack(e.direction)) {
    commit(500 / speed);
  }
}

void Player::onEvent(DropCommandEvent &e) {
  if (e.item == nullptr)
    return;
  fmt::print("drop in player\n");
  inventory.erase(std::remove(inventory.begin(), inventory.end(), e.item),
                  inventory.end());
  DropEvent me(shared_from_this(), e.item);
  eb::EventBus::FireEvent(me);
  commit(200 / speed);
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
    commit(1000 / speed);
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
                             return std::dynamic_pointer_cast<Item>(o) &&
                                    o->currentCell == currentCell;
                           });
  if (item != currentLocation->objects.end()) {
    pick(std::dynamic_pointer_cast<Item>(*item));
  }
}

void Player::onEvent(EquipCommandEvent &e) {
  if (e.item == nullptr)
    return;
  fmt::print("Equipment equip: {} --> {}\n", e.item->type.name, e.slot->name);
  equip(e.slot, e.item);
}

void Player::onEvent(UnEquipCommandEvent &e) { unequip(e.slot); }

bool Player::interact(std::shared_ptr<Object> actor) {
  auto enemy = std::dynamic_pointer_cast<Enemy>(actor);
  auto ptr = shared_from_this();
  if (hp > 0) {
    auto damage = enemy->getDamage(shared_from_this());
    hp -= damage;
    // EnemyTakeDamageEvent e(ptr, damage);
    // eb::EventBus::FireEvent(e);
  }
  if (hp <= 0) {
    passThrough = true;
    MessageEvent e2(ptr, "YOU DIED!!");
    eb::EventBus::FireEvent(e2);
  }
  commit(0);
  return hp > 0;
}
