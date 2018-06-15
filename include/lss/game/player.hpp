#ifndef __PLAYER_H_
#define __PLAYER_H_
#include "lss/game/creature.hpp"
#include "lss/game/item.hpp"

typedef std::vector<std::shared_ptr<Item>> Inventory;

class Player : public Creature {
public:
  Inventory inventory;
  bool take(std::shared_ptr<Item>);
};

#endif // __PLAYER_H_
