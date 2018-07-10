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
  int level;
  float baseSpeed;
  int baseHP;
  int defense;
  int baseDamage_dices;
  int baseDamage_edges;
  int baseDamage_modifier;
  Items loot;
  std::vector<Trait> traits;
  Items equipped;
  AIType aiType = AGGRESSIVE;

  friend bool operator<(const EnemySpec &lhs, const EnemySpec &rhs) {
    return lhs.name < rhs.name;
  }
};

// clang-format off
namespace EnemyType {
EnemySpec const RAT = {
    "rat", 0,
    2, 10, 1, 1, 3, 0,
    Items{},
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB},
    Items{}
};
EnemySpec const BAT = {
    "bat", 0,
    1.5, 10, 1, 1, 3, 0,
    Items{},
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}
};
EnemySpec const GOBLIN = {
    "goblin", 1,
    1, 15, 1, 1, 3, 0,
    Items{Prototype::GOLD->clone(50)},
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB},
    Items{Prototype::DAGGER, Prototype::DAGGER}
};
EnemySpec const ORK = {
    "ork", 2,
    1, 55, 2, 1, 6, 1,
    Items{std::make_shared<Item>(ItemType::CORPSE), Prototype::TORCH},
    {}, Items{Prototype::TORCH}
};
EnemySpec const PIXI = {
    "pixi", 3,
    2, 25, 0, 1, 2, 0,
    Items{std::make_shared<Item>(
        ItemType::GOLD_RING,
        Effects{std::make_shared<SpecialPostfix>("of lightning"),
                std::make_shared<SpeedModifier>(0.3)})},
    {Traits::FLY, Traits::MOB}};
EnemySpec const OGRE = {
    "ogre", 4,
    1, 100, 2, 1, 6, 1,
    Items{std::make_shared<Item>(ItemType::CORPSE)},
    {Traits::NIGHT_VISION},
    Items{Prototype::GREAT_AXE},
};
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
  micropather::MPVector<void *> pathToHero;
  int step;
  bool randomPath();

  virtual void onEvent(CommitEvent &e) override;
};

#endif // __ENEMY_H_
