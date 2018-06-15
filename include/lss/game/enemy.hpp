#ifndef __ENEMY_H_
#define __ENEMY_H_

#include "Event.hpp"
#include "lss/game/creature.hpp"
#include "lss/game/item.hpp"

class Enemy : public Creature {
public:
  Enemy();
  bool interact();
  std::shared_ptr<Item> drop();
};

#endif // __ENEMY_H_
