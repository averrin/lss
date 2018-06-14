#include "lss/game/enemy.hpp"

Enemy::Enemy(): Creature() {
    hp = 10;
}

bool Enemy::interact() {
    if (hp > 0) {
        hp -= 5;
        return true;
    }
    passThrough = true;
    return false;
}


