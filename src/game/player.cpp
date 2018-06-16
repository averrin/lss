#include <memory>
#include "lss/game/player.hpp"
#include "EventBus.hpp"

Player::Player() : Creature() {
    std::cout << "player constructor" << std::endl;
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

void Player::onEvent(MoveCommandEvent &e) {
    std::cout << "player onEvent" << std::endl;
        move(e.direction);
}

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
