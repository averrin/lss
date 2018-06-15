#include <algorithm>

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
