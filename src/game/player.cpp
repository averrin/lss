#include <memory>
#include "lss/game/player.hpp"
#include "lss/command.hpp"
#include "EventBus.hpp"

Player::Player() : Creature() {
  eb::EventBus::AddHandler<MoveCommandEvent>(*this);
}

bool Player::pick(std::shared_ptr<Item> item) {
    auto ptr = shared_from_this();
    inventory.push_back(item);
    ItemTakenEvent e(ptr, item);
    eb::EventBus::FireEvent(e);
    return true;
}

ItemTakenEvent::ItemTakenEvent(eb::ObjectPtr s, std::shared_ptr<Item> i): eb::Event(s), item(i) {}
MoveCommandEvent::MoveCommandEvent(Direction d): CommandEvent(nullptr), direction(d) {}

PickCommandEvent::PickCommandEvent(): CommandEvent(nullptr){}
std::shared_ptr<CommandEvent> PickCommand::getEvent(std::string s) {
  return std::make_shared<PickCommandEvent>();
}


void Player::onEvent(MoveCommandEvent &e) {
    move(e.direction);
}


void Player::onEvent(DigCommandEvent &e) {
  auto cell = getCell(e.direction);
  
  DigEvent de(shared_from_this(), cell);
  eb::EventBus::FireEvent(de);
}

DigEvent::DigEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c): eb::Event(s), cell(c) {}
void Player::onEvent(PickCommandEvent &e) {
    auto item = std::find_if(currentLocation->objects.begin(),
                             currentLocation->objects.end(),
                             [](std::shared_ptr<Object> o) {
                               return std::dynamic_pointer_cast<Item>(o);
                             });
    if (item != currentLocation->objects.end()) {
      pick(std::dynamic_pointer_cast<Item>(*item));
    }
}
