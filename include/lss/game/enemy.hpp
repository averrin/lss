#ifndef __ENEMY_H_
#define __ENEMY_H_

#include "lss/game/creature.hpp"

class Enemy : public Creature {
public:
    Enemy();
    bool interact();
};


#endif // __ENEMY_H_
