#include "lss/game/enemy.hpp"
#include "EventBus.hpp"

Enemy::Enemy(): Creature() {
    hp = 10;
}

std::shared_ptr<Item> Enemy::drop() {
    auto item = std::make_shared<Item>(ItemType::CORPSE);
    item->currentCell = currentCell;
    return item;
}

bool Enemy::interact() {
    auto ptr = shared_from_this();
    if (hp > 0) {
        hp -= 5;
        EnemyTakeDamageEvent e(ptr, 5);
		eb::EventBus::FireEvent(e);
    }
    if (hp <= 0) {
        passThrough = true;
        EnemyDiedEvent e2(ptr);
		eb::EventBus::FireEvent(e2);
    }
    return hp > 0;
}


EnemyTakeDamageEvent::EnemyTakeDamageEvent(eb::ObjectPtr s, int d): eb::Event(s), damage(d) {}
EnemyDiedEvent::EnemyDiedEvent(eb::ObjectPtr s): eb::Event(s) {}
