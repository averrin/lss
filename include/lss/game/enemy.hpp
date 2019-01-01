#ifndef __ENEMY_H_
#define __ENEMY_H_
#include <optional>

#include "Event.hpp"
#include "EventBus.hpp"
#include "lss/game/ai.hpp"
#include "lss/game/creature.hpp"
#include "lss/game/damageSpec.hpp"
#include "lss/game/item.hpp"
#include "lss/game/lootBox.hpp"
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
  LibLog::Logger &log = LibLog::Logger::getInstance();
public:
  Enemy(EnemySpec);
  ~Enemy();

  std::string getId() {return fmt::format("{}@{}.{}", lu::magenta(name),
                                  currentCell->x, currentCell->y);}

  std::optional<Items> drop();

  Direction cd = Direction::W;
  EnemySpec type;
  int step;
  bool randomPath();
  void onDamage(std::shared_ptr<Creature>, std::shared_ptr<Damage>) override;
  void onDie() override;
  bool interact(std::shared_ptr<Object> actor);
  void commit(int ap);
  void prepareAiState();

  std::optional<AiAction> execAction();
  std::optional<AiAction> execAction(int ap);
  std::optional<AiAction> execAiNone(int ap);
  std::optional<AiAction> execAiPassive(int ap);
  std::optional<AiAction> execAiAggressive(int ap);

  virtual void onEvent(CommitEvent &e) override;
};

class EnemySpecHolder : public Object {
public:
  EnemySpecHolder(EnemySpec s) : Object(), spec(s) {}
  EnemySpec spec;
};

#endif // __ENEMY_H_
