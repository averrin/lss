#include <algorithm>
#include <memory>

#include "lss/game/location.hpp"
#include "lss/game/enemy.hpp"
#include "EventBus.hpp"

Location::Location() {}

void Location::onEvent(EnemyDiedEvent & e) {
    auto sender = e.getSender();
    if (auto enemy = std::dynamic_pointer_cast<Enemy>(sender)) {
        objects.push_back(enemy->drop());
    }
    objects.erase(std::remove(objects.begin(), objects.end(), sender), objects.end());   
}

void Location::onEvent(ItemTakenEvent & e) {
    objects.erase(std::remove(objects.begin(), objects.end(), e.item), objects.end());   
}

ItemsFoundEvent::ItemsFoundEvent(eb::ObjectPtr s, Objects i): eb::Event(s), items(i) {}
void Location::onEvent(EnterCellEvent & e) {
    Objects items(objects.size());
    auto it = std::copy_if(objects.begin(), objects.end(), items.begin(), [e](std::shared_ptr<Object> o){
        return std::dynamic_pointer_cast<Item>(o) && o->currentCell == e.cell;
    });
    items.resize(std::distance(items.begin(), it));
    if (items.size() > 0) {
        ItemsFoundEvent ie(nullptr, items);
        eb::EventBus::FireEvent(ie);
    }
}
