#ifndef __ENEMY_H_
#define __ENEMY_H_

#include "lss/game/creature.hpp"
#include "Event.hpp"

class Enemy : public Creature {
public:
    Enemy();
    bool interact();
};

class EnemyTakeDamageEvent: public eb::Event {
public:
    EnemyTakeDamageEvent(eb::ObjectPtr, int);
    int damage;
};

class EnemyDiedEvent: public eb::Event {
public:
    EnemyDiedEvent(eb::ObjectPtr);
};

#endif // __ENEMY_H_
