#include "lss/game/player.hpp"
#include "EventBus.hpp"


bool Player::take(std::shared_ptr<Item> item) {
    auto ptr = shared_from_this();
    inventory.push_back(item);
    ItemTakenEvent e(ptr, item);
    eb::EventBus::FireEvent(e);
    return true;
}

ItemTakenEvent::ItemTakenEvent(eb::ObjectPtr s, std::shared_ptr<Item> i): eb::Event(s), item(i) {}
