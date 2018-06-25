#ifndef __PLAYER_H_
#define __PLAYER_H_
#include <memory>

#include "lss/game/creature.hpp"
#include "lss/game/equipment.hpp"
#include "lss/game/item.hpp"

typedef std::vector<std::shared_ptr<Item>> Items;

class Player : public Creature,
               public eb::EventHandler<MoveCommandEvent>,
               public eb::EventHandler<DigCommandEvent>,
               public eb::EventHandler<AttackCommandEvent>,
               public eb::EventHandler<WalkCommandEvent>,
               public eb::EventHandler<EquipCommandEvent>,
               public eb::EventHandler<UnEquipCommandEvent>,
               public eb::EventHandler<DropCommandEvent>,
               public eb::EventHandler<PickCommandEvent> {
public:
  Player();
  std::string name = "Unnamed hero";

  Items inventory;
  std::shared_ptr<Equipment> equipment;
  Effects activeEffects;
  bool monsterSense = false;

  bool pick(std::shared_ptr<Item>);
  bool equip(std::shared_ptr<Slot>, std::shared_ptr<Item>);
  bool unequip(std::shared_ptr<Slot>);
  bool interact(std::shared_ptr<Object>) override;
  void commit(int ap);

  virtual void onEvent(MoveCommandEvent &e) override;
  virtual void onEvent(PickCommandEvent &e) override;
  virtual void onEvent(DigCommandEvent &e) override;
  virtual void onEvent(WalkCommandEvent &e) override;
  virtual void onEvent(AttackCommandEvent &e) override;
  virtual void onEvent(EquipCommandEvent &e) override;
  virtual void onEvent(UnEquipCommandEvent &e) override;
  virtual void onEvent(DropCommandEvent &e) override;
};

#endif // __PLAYER_H_
