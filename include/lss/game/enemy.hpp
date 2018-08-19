#ifndef __ENEMY_H_
#define __ENEMY_H_
#include <optional>

#include "Event.hpp"
#include "EventBus.hpp"
#include "lss/game/creature.hpp"
#include "lss/game/damageSpec.hpp"
#include "lss/game/item.hpp"
#include "lss/game/lootBox.hpp"
#include "lss/game/ai.hpp"
#include "micropather/micropather.h"

struct EnemySpec {
public:
  std::string name;
  int level;
  float baseSpeed;
  int baseHP;
  int baseMP;
  int defense;
  DamageSpec dmgSpec = DamageSpec(0, 0, 0, DamageType::BASIC);
  LootBox loot = LootBoxes::ZERO;
  std::vector<Trait> traits;
  Items equipped;
  Effects activeEffects;
  AIType aiType = AIType::AGGRESSIVE;

  friend bool operator<(const EnemySpec &lhs, const EnemySpec &rhs) {
    return lhs.name < rhs.name;
  }
};

class Enemy : public Creature, public eb::EventHandler<CommitEvent> {
public:
  Enemy(EnemySpec);
  ~Enemy();
  std::optional<Items> drop();

  Direction cd = Direction::W;
  int actionPoints = 0;
  EnemySpec type;
  int step;
  bool randomPath();
  void onDamage(std::shared_ptr<Creature>, std::shared_ptr<Damage>) override;
  void onDie() override;
  bool interact(std::shared_ptr<Object> actor);
  void commit(int ap);
  std::optional<int> execAction(int ap);
  std::optional<int> execAiNone(int ap);
  std::optional<int> execAiPassive(int ap);
  std::optional<int> execAiAggressive(int ap);

  virtual void onEvent(CommitEvent &e) override;
};

class EnemySpecHolder : public Object {
public:
  EnemySpecHolder(EnemySpec s) : Object(), spec(s) {}
  EnemySpec spec;
};

#endif // __ENEMY_H_
