#include "lss/game/enemy.hpp"
#include "EventBus.hpp"

Enemy::Enemy(EnemySpec t): Creature(), type(t) {
    hp = type.baseHP;
    speed = type.baseSpeed;
}

Enemy::~Enemy() {
    registration->removeHandler();
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

void Enemy::onEvent(CommitEvent &e) {
    actionPoints += e.actionPoints;
    auto stepCost = 1000 / speed;

    // fmt::print("{}\n", actionPoints);

    //TODO: solve endless cycle
    auto n = 0;
    while (actionPoints >= stepCost && n != 2) {
        if (!move(cd)) {
            if (cd == Direction::W) {
                cd = Direction::E;
            } else {
                cd = Direction::W;
            }
            n++;
        } else {
            actionPoints -= stepCost;
        }
    }
}


EnemyTakeDamageEvent::EnemyTakeDamageEvent(eb::ObjectPtr s, int d): eb::Event(s), damage(d) {}
EnemyDiedEvent::EnemyDiedEvent(eb::ObjectPtr s): eb::Event(s) {}
