#include "lss/game/player.hpp"
#include "EventBus.hpp"
#include "lss/command.hpp"
#include "lss/game/slot.hpp"
#include <memory>

CommitEvent::CommitEvent(eb::ObjectPtr s, int ap)
    : eb::Event(s), actionPoints(ap) {}
ItemTakenEvent::ItemTakenEvent(eb::ObjectPtr s, std::shared_ptr<Item> i)
    : eb::Event(s), item(i) {}
DigEvent::DigEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c)
    : eb::Event(s), cell(c) {}

Player::Player() : Creature() {
  eb::EventBus::AddHandler<MoveCommandEvent>(*this);
  eb::EventBus::AddHandler<WalkCommandEvent>(*this);

  equipment = std::make_shared<Equipment>();
  equipment->slots = {
        std::make_shared<Slot>("Right hand", std::vector<WearableType>{WEAPON, WEAPON_TWOHANDED, SHIELD}),
        std::make_shared<Slot>("Left hand", std::vector<WearableType>{WEAPON, SHIELD}),
        std::make_shared<Slot>("Right ring", std::vector<WearableType>{RING})
  };

  hp = 20;
  damage = 5;
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
  inventory.push_back(item);

  ItemTakenEvent e(ptr, item);
  eb::EventBus::FireEvent(e);
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
    if (e.item == nullptr) return;
    fmt::print("Equipment equip: {} --> {}\n", e.item->type.name, e.slot->name);
    equip(e.slot, e.item);
}

void Player::onEvent(UnEquipCommandEvent &e) {
    unequip(e.slot);
}
