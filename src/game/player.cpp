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
