#ifndef __PLAYER_H_
#define __PLAYER_H_
#include "lss/profiler.hpp"
#include <map>
#include <memory>

#include "lss/game/creature.hpp"
#include "lss/game/report.hpp"

typedef std::vector<std::shared_ptr<Item>> Items;

class Player : public Creature,
               public eb::EventHandler<EnemyDiedEvent>,
               public eb::EventHandler<MoveCommandEvent>,
               public eb::EventHandler<DigCommandEvent>,
               public eb::EventHandler<AttackCommandEvent>,
               public eb::EventHandler<WalkCommandEvent>,
               public eb::EventHandler<EquipCommandEvent>,
               public eb::EventHandler<UnEquipCommandEvent>,
               public eb::EventHandler<DropCommandEvent>,
               public eb::EventHandler<WaitCommandEvent>,
               public eb::EventHandler<ZapCommandEvent>,
               public eb::EventHandler<ThrowCommandEvent>,
               public eb::EventHandler<PickCommandEvent> {
public:
  Player();
  ~Player();
  bool monsterSense = false;
  Report report;

  bool equip(std::shared_ptr<Slot>, std::shared_ptr<Item>);
  bool equip(std::shared_ptr<Item>);
  bool unequip(std::shared_ptr<Slot>);
  void commit(std::string reason, int ap, bool s = false);
  bool interact(std::shared_ptr<Object> actor) override;

  std::string getDmgDesc();
  void onDamage(std::shared_ptr<Creature>, std::shared_ptr<Damage>) override;
  void onDie() override;
  void increaseIntelligence(float val);
  void increaseStrength(float val);
  std::map<std::shared_ptr<Object>, std::set<std::shared_ptr<Cell>>>
  getLightMap();

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
  virtual void onEvent(EnemyDiedEvent &e) override;
  virtual void onEvent(ThrowCommandEvent &e) override;
};

#endif // __PLAYER_H_
