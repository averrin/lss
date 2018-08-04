#ifndef __ENEMY_H_
#define __ENEMY_H_
#include <optional>

#include "Event.hpp"
#include "EventBus.hpp"
#include "lss/game/creature.hpp"
#include "lss/game/item.hpp"
#include "lss/game/lootBox.hpp"
#include "lss/game/damageSpec.hpp"
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
  DamageSpec dmgSpec;
  LootBox loot;
  std::vector<Trait> traits;
  Items equipped;
  Effects activeEffects;
  AIType aiType = AGGRESSIVE;

  friend bool operator<(const EnemySpec &lhs, const EnemySpec &rhs) {
    return lhs.name < rhs.name;
  }
};

const auto ds_1d3 = DamageSpec(0, 1, 3, DamageType::BASIC);

// clang-format off
namespace EnemyType {
EnemySpec const RAT = {
    "rat", 0,
    2, 3, 0, 1,
    ds_1d3,
    LootBox{1, {}},
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB},
    Items{}
};

EnemySpec const SNAKE = {
    "snake", 0,
    1.5, 3, 0, 1,
    ds_1d3,
    LootBox{1, {}},
    {},
    Items{}
};

EnemySpec const VIPER = {
    "viper", 0,
    2, 3, 0, 1,
    ds_1d3,
    LootBox{1, {}},
    {},
    Items{Prototype::POISON_FANG}
};
EnemySpec const BAT = {
    "bat", 0,
    1.5, 3, 0, 1,
    ds_1d3,
    LootBox{1, {}},
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}, {} //vampire
};
EnemySpec const BAT_LARGE = {
    "large bat", 0,
    1.5, 10, 0, 1,
    DamageSpec(0, 2, 3, DamageType::BASIC),
    LootBox{100, {}},
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}, {} //vampire
};
EnemySpec const GOBLIN = {
    "goblin", 1,
    1, 15, 0, 1,
    DamageSpec(0, 1, 3, DamageType::WEAPON),
    LootBox{1, {}, {
        {0.40, {}, LootBoxes::POTIONS, true},
        {0.60, {}, {
          {0.25, {Prototype::DAGGER}},
          {0.25, {Prototype::HELMET}},
          {0.25, {Prototype::GREAVES}},
          {0.25, {Prototype::BOOTS}},
          }, true},
        {0.90, {Prototype::GOLD->setCount(50)}}}},
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::GOBLIN_DAGGER, Prototype::GOBLIN_DAGGER}
};
// Goblin lieutenant
EnemySpec const GOBLIN_LIEUTENANT = {
    "goblin lieutenant", 1,
    1, 25, 0, 1,
    DamageSpec(1, 1, 5, DamageType::WEAPON),
    LootBox{1, {}, {
        {0.40, {Prototype::POTION_HEAL}},
        {0.20, {}, LootBoxes::POTIONS, true},
        {0.40, {}, LootBoxes::SCROLLS, true},
        {0.80, {}, {
          {0.25, {Prototype::DAGGER}},
          {0.25, {Prototype::HELMET}},
          {0.25, {Prototype::GREAVES}},
          {0.25, {Prototype::BOOTS}},
          }, true},
        {0.90, {Prototype::GOLD->setCount(50)}}}},
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::GOBLIN_DAGGER, Prototype::GOBLIN_DAGGER, Prototype::LEATHER_ARMOR}
};

EnemySpec const GOBLIN_ROGUE = {
    "goblin rogue", 1,
    1.5, 15, 0, 1,
    DamageSpec(1, 1, 3, DamageType::WEAPON),
    LootBox{1, {}, {
        {0.60, {}, {
          {0.25, {Prototype::DAGGER}},
          {0.25, {Prototype::HELMET}},
          {0.25, {Prototype::GREAVES}},
          {0.25, {Prototype::BOOTS}},
          }, true},
        {0.90, {Prototype::GOLD->setCount(50)}}}},
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::POISON_DAGGER, Prototype::GOBLIN_DAGGER}
};
EnemySpec const ORK = {
    "ork", 2,
    1, 55, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox{1, {Prototype::TORCH}, {
        {0.40, {Prototype::POTION_HEAL}},
        {0.40, {}, LootBoxes::SCROLLS, true},
        {0.70, {}, {
          {0.15, {Prototype::PLATE}},
          {0.35, {Prototype::SWORD}},
          {0.35, {Prototype::HELMET}},
          {0.35, {Prototype::SHIELD}},
          {0.35, {Prototype::GREAVES}},
          {0.35, {Prototype::BOOTS}},
          }, true},
        {0.90, {Prototype::GOLD->setCount(100)}}}},
    {}, Items{Prototype::TORCH, Prototype::ORK_SWORD, Prototype::SHIELD}
};

EnemySpec const ORK_BLACK = {
    "black ork", 2,
    1.1, 75, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox{1, {Prototype::TORCH}, {
        {0.50, {Prototype::POTION_HEAL}},
        {0.20, {}, LootBoxes::POTIONS, true},
        {0.70, {}, {
          {0.15, {Prototype::PLATE}},
          {0.35, {Prototype::SWORD}},
          {0.35, {Prototype::HELMET}},
          {0.35, {Prototype::SHIELD}},
          {0.35, {Prototype::GREAVES}},
          {0.35, {Prototype::BOOTS}},
          }, true},
        {0.90, {Prototype::GOLD->setCount(100)}}}},
    {}, Items{Prototype::TORCH, Prototype::ORK_SWORD, Prototype::SHIELD, Prototype::LEATHER_ARMOR}
};
EnemySpec const PIXI = {
    "pixi", 3,
    3, 25, 30, 0,
    DamageSpec(0, 3, 4, DamageType::WEAPON),
    {0.90, {Prototype::GOLD->setCount(200)}, {
        {0.40, {}, LootBoxes::SCROLLS, true},
        {10, {Prototype::SPEED_RING}}}},
    {Traits::FLY, Traits::MOB}};
EnemySpec const OGRE = {
    "ogre", 4,
    1, 100, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox{0.80, {Prototype::GREAT_AXE}},
    {Traits::NIGHT_VISION},
    Items{Prototype::GREAT_AXE},
};

  const std::vector<EnemySpec> ALL = {
    RAT,
    SNAKE, VIPER,
    BAT, BAT_LARGE,
    GOBLIN, GOBLIN_ROGUE, GOBLIN_LIEUTENANT,
    ORK, ORK_BLACK,
    PIXI,
    OGRE,
  };
} // namespace EnemyType
// clang-format on

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
  void onDamage(std::shared_ptr<Creature>, std::shared_ptr<Damage>);
  void onDie();
  bool interact(std::shared_ptr<Object> actor);

  virtual void onEvent(CommitEvent &e) override;
};

class EnemySpecHolder : public Object {
public:
  EnemySpecHolder(EnemySpec s) : Object(), spec(s) {}
  EnemySpec spec;
};

#endif // __ENEMY_H_
