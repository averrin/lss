#ifndef __ENEMY_H_
#define __ENEMY_H_
#include <optional>

#include "Event.hpp"
#include "EventBus.hpp"
#include "lss/game/creature.hpp"
#include "lss/game/effect.hpp"
#include "lss/game/item.hpp"
#include "micropather/micropather.h"

enum AIType {
  NO_AI,
  AGGRESSIVE,
  PASSIVE,
};

struct EnemySpec {
public:
  std::string name;
  float baseSpeed;
  int baseHP;
  int defense;
  int baseDamage_dices;
  int baseDamage_edges;
  int baseDamage_modifier;
  Items loot;
  std::vector<Trait> traits;
  AIType aiType = AGGRESSIVE;

  friend bool operator<(const EnemySpec &lhs, const EnemySpec &rhs) {
    return lhs.name < rhs.name;
  }
};

// clang-format off
namespace EnemyType {
EnemySpec const GOBLIN = {
    "goblin",
    0.5, 5, 1, 1, 3, 0,
    Items{std::make_shared<Item>(ItemType::GOLD_COINS, 200)},
    {Traits::NIGHT_VISION}};
EnemySpec const ORK = {
    "ork",
    1, 35, 2, 1, 6, 1,
    Items{std::make_shared<Item>(ItemType::CORPSE)}};
EnemySpec const PIXI = {
    "pixi",
    2, 1, 0, 1, 2, 0,
    Items{std::make_shared<Item>(
        ItemType::GOLD_RING,
        Effects{std::make_shared<SpecialPostfix>("of lightning"),
                std::make_shared<SpeedModifier>(1)})}};
} // namespace EnemyType
// clang-format on

class Enemy : public Creature, public eb::EventHandler<CommitEvent> {
public:
  Enemy(EnemySpec);
  ~Enemy();
  eb::HandlerRegistrationPtr registration;
  bool interact(std::shared_ptr<Object>) override;
  std::optional<Items> drop();

  Direction cd = Direction::W;
  int actionPoints = 0;
  EnemySpec type;
  micropather::MPVector<void *> path;

  virtual void onEvent(CommitEvent &e) override;
};

#endif // __ENEMY_H_
