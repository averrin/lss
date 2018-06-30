#ifndef __PLAYER_H_
#define __PLAYER_H_
#include <map>
#include <memory>

#include "lss/game/creature.hpp"

typedef std::vector<std::shared_ptr<Item>> Items;

class Player : public Creature,
               public eb::EventHandler<MoveCommandEvent>,
               public eb::EventHandler<DigCommandEvent>,
               public eb::EventHandler<AttackCommandEvent>,
               public eb::EventHandler<WalkCommandEvent>,
               public eb::EventHandler<EquipCommandEvent>,
               public eb::EventHandler<UnEquipCommandEvent>,
               public eb::EventHandler<DropCommandEvent>,
               public eb::EventHandler<WaitCommandEvent>,
               public eb::EventHandler<ZapCommandEvent>,
               public eb::EventHandler<PickCommandEvent> {
public:
  Player();
  bool monsterSense = false;

  bool equip(std::shared_ptr<Slot>, std::shared_ptr<Item>);
  bool unequip(std::shared_ptr<Slot>);
  bool interact(std::shared_ptr<Object>) override;
  void commit(int ap);

  bool haveLight();
  std::string getDmgDesc();

  virtual void onEvent(MoveCommandEvent &e) override;
  virtual void onEvent(PickCommandEvent &e) override;
  virtual void onEvent(DigCommandEvent &e) override;
  virtual void onEvent(WalkCommandEvent &e) override;
  virtual void onEvent(AttackCommandEvent &e) override;
  virtual void onEvent(EquipCommandEvent &e) override;
  virtual void onEvent(UnEquipCommandEvent &e) override;
  virtual void onEvent(DropCommandEvent &e) override;
  virtual void onEvent(WaitCommandEvent &e) override;
  virtual void onEvent(ZapCommandEvent &e) override;
};

#endif // __PLAYER_H_
