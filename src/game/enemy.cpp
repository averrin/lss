#include "lss/game/enemy.hpp"
#include "EventBus.hpp"

Enemy::Enemy(): Creature() {
    hp = 10;
}

bool Enemy::interact() {
    if (hp > 0) {
        hp -= 5;
        EnemyTakeDamageEvent e(shared_from_this(), 5);
		eb::EventBus::FireEvent(e);
    }
    if (hp <= 0) {
        passThrough = true;
        EnemyDiedEvent e2(shared_from_this());
		eb::EventBus::FireEvent(e2);
    }
    return hp > 0;
}


EnemyTakeDamageEvent::EnemyTakeDamageEvent(eb::ObjectPtr s, int d): eb::Event(s), damage(d) {}
EnemyDiedEvent::EnemyDiedEvent(eb::ObjectPtr s): eb::Event(s) {}
