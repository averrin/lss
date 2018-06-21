#ifndef __PLAYER_H_
#define __PLAYER_H_
#include "lss/game/creature.hpp"
#include "lss/game/item.hpp"

class Slot {};

typedef std::vector<std::shared_ptr<Item>> Inventory;

class Player : public Creature,
               public eb::EventHandler<MoveCommandEvent>,
               public eb::EventHandler<DigCommandEvent>,
               public eb::EventHandler<AttackCommandEvent>,
               public eb::EventHandler<WalkCommandEvent>,
               public eb::EventHandler<EquipCommandEvent>,
               public eb::EventHandler<PickCommandEvent> {
public:
  Player();
  Inventory inventory;
  Inventory equipment;
  bool pick(std::shared_ptr<Item>);
  bool equip(std::shared_ptr<Item>);
  void commit(int ap);

  virtual void onEvent(MoveCommandEvent &e) override;
  virtual void onEvent(PickCommandEvent &e) override;
  virtual void onEvent(DigCommandEvent &e) override;
  virtual void onEvent(WalkCommandEvent &e) override;
  virtual void onEvent(AttackCommandEvent &e) override;
  virtual void onEvent(EquipCommandEvent &e) override;
};

#endif // __PLAYER_H_
