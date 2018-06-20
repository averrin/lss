#include <memory>
#include "lss/game/player.hpp"
#include "lss/command.hpp"
#include "EventBus.hpp"

CommitEvent::CommitEvent(eb::ObjectPtr s, int ap): eb::Event(s), actionPoints(ap) {}

Player::Player() : Creature() {
  eb::EventBus::AddHandler<MoveCommandEvent>(*this);
  eb::EventBus::AddHandler<WalkCommandEvent>(*this);
}

void Player::commit(int ap) {
    auto ptr = shared_from_this();
    CommitEvent e(ptr, ap);
    eb::EventBus::FireEvent(e);
}

bool Player::pick(std::shared_ptr<Item> item) {
    auto ptr = shared_from_this();
    inventory.push_back(item);
    ItemTakenEvent e(ptr, item);
    eb::EventBus::FireEvent(e);
    commit(1000 / speed);
    return true;
}

ItemTakenEvent::ItemTakenEvent(eb::ObjectPtr s, std::shared_ptr<Item> i): eb::Event(s), item(i) {}
MoveCommandEvent::MoveCommandEvent(Direction d): CommandEvent(nullptr), direction(d) {}

PickCommandEvent::PickCommandEvent(): CommandEvent(nullptr){}
std::optional<std::shared_ptr<CommandEvent>> PickCommand::getEvent(std::string s) {
  return std::make_shared<PickCommandEvent>();
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


DigEvent::DigEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c): eb::Event(s), cell(c) {}
void Player::onEvent(DigCommandEvent &e) {
  if (std::find_if(inventory.begin(), inventory.end(), [](std::shared_ptr<Item> item) {
    return item->type == ItemType::PICK_AXE;
  }) == inventory.end()) {
    MessageEvent me(shared_from_this(), "You have no pick axe");
    eb::EventBus::FireEvent(me);
    return;
  }

  auto cell = getCell(e.direction);
  
  DigEvent de(shared_from_this(), cell);
  eb::EventBus::FireEvent(de);
}

void Player::onEvent(WalkCommandEvent &e) {
  while(move(e.direction)) {
    commit(1000 / speed);
    auto item = std::find_if(currentLocation->objects.begin(),
                             currentLocation->objects.end(),
                             [&](std::shared_ptr<Object> o) {
                               return std::dynamic_pointer_cast<Item>(o) && o->currentCell == currentCell;
                             });
    if (item != currentLocation->objects.end()) break;
  }
}

void Player::onEvent(PickCommandEvent &e) {
    auto item = std::find_if(currentLocation->objects.begin(),
                             currentLocation->objects.end(),
                             [&](std::shared_ptr<Object> o) {
                               return std::dynamic_pointer_cast<Item>(o) && o->currentCell == currentCell;
                             });
    if (item != currentLocation->objects.end()) {
      pick(std::dynamic_pointer_cast<Item>(*item));
    }
}
