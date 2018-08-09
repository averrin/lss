#ifndef __ENEMY_H_
#define __ENEMY_H_
#include <optional>

#include "Event.hpp"
#include "EventBus.hpp"
#include "lss/game/creature.hpp"
#include "lss/game/damageSpec.hpp"
#include "lss/game/item.hpp"
#include "lss/game/lootBox.hpp"
#include "micropather/micropather.h"

enum AIType {
  NO_AI,
  AGGRESSIVE,
  PASSIVE,
};

struct EnemySpec {
public:
  std::string name;
  int level;
  float baseSpeed;
  int baseHP;
  int baseMP;
  int defense;
  DamageSpec dmgSpec = DamageSpec(0, 0, 0, DamageType::BASIC);
  LootBox loot;
  std::vector<Trait> traits;
  Items equipped;
  Effects activeEffects;
  AIType aiType = AGGRESSIVE;

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
  micropather::MPVector<void *> path;
  micropather::MPVector<void *> pathToHero;
  int step;
  micropather::MicroPather *pather = nullptr;
  bool randomPath();
  void onDamage(std::shared_ptr<Creature>, std::shared_ptr<Damage>) override;
  void onDie() override;
  bool interact(std::shared_ptr<Object> actor) override;

  virtual void onEvent(CommitEvent &e) override;
};

class EnemySpecHolder : public Object {
public:
  EnemySpecHolder(EnemySpec s) : Object(), spec(s) {}
  EnemySpec spec;
};

#endif // __ENEMY_H_
