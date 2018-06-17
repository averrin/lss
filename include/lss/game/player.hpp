#ifndef __PLAYER_H_
#define __PLAYER_H_
#include "lss/game/creature.hpp"
#include "lss/game/item.hpp"

typedef std::vector<std::shared_ptr<Item>> Inventory;

class Player : public Creature,
               public eb::EventHandler<MoveCommandEvent>,
               public eb::EventHandler<DigCommandEvent>,
               public eb::EventHandler<PickCommandEvent> {
public:
  Player();
  Inventory inventory;
  bool pick(std::shared_ptr<Item>);

  virtual void onEvent(MoveCommandEvent &e) override;
  virtual void onEvent(PickCommandEvent &e) override;
  virtual void onEvent(DigCommandEvent &e) override;
};

#endif // __PLAYER_H_
