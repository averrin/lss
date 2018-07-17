#ifndef __ENEMY_H_
#define __ENEMY_H_
#include <optional>

#include "Event.hpp"
#include "EventBus.hpp"
#include "lss/game/creature.hpp"
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
  int defense;
  int baseDamage_dices;
  int baseDamage_edges;
  int baseDamage_modifier;
  LootBox loot;
  std::vector<Trait> traits;
  Items equipped;
  Effects activeEffects;
  AIType aiType = AGGRESSIVE;

  friend bool operator<(const EnemySpec &lhs, const EnemySpec &rhs) {
    return lhs.name < rhs.name;
  }
};

// clang-format off
namespace EnemyType {
EnemySpec const RAT = {
    "rat", 0,
    2, 3, 1,
    1, 3, 0,
    LootBox{},
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB},
    Items{}
};
EnemySpec const BAT = {
    "bat", 0,
    1.5, 3, 1,
    1, 3, 0,
    LootBox{1, {}},
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}, {std::make_shared<Vampire>(1)}
};
EnemySpec const BAT_LARGE = {
    "large bat", 0,
    1.5, 10, 1,
    2, 3, 0,
    LootBox{100, {}},
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}, {std::make_shared<Vampire>(3)}
};
EnemySpec const GOBLIN = {
    "goblin", 1,
    1, 15, 1,
    1, 3, 0,
    LootBox{1, {}, {
        {0.40, {Prototype::POTION_HEAL_LESSER}},
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
    1, 25, 1,
    1, 5, 1,
    LootBox{1, {}, {
        {0.40, {Prototype::POTION_HEAL}},
        {0.40, {}, {{0.30, {Prototype::SCROLL_IDENTIFICATION}}, {0.30, {Prototype::SCROLL_REVEAL}}, {0.30, {Prototype::SCROLL_TELEPORT}}}, true},
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
    1.5, 15, 1,
    1, 3, 0,
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
    1, 55, 2, 1, 6, 1,
    LootBox{1, {Prototype::TORCH}, {
        {0.40, {Prototype::POTION_HEAL}},
        {0.40, {}, {{0.30, {Prototype::SCROLL_IDENTIFICATION}}, {0.30, {Prototype::SCROLL_REVEAL}}, {0.30, {Prototype::SCROLL_TELEPORT}}}, true},
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
    1.1, 75, 2,
    1, 6, 1,
    LootBox{1, {Prototype::TORCH}, {
        {0.50, {Prototype::POTION_HEAL}},
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
    3, 25, 0, 3, 4, 0,
    {0.90, {Prototype::GOLD->setCount(200)}, {{10, {Prototype::SPEED_RING}}}},
    {Traits::FLY, Traits::MOB}};
EnemySpec const OGRE = {
    "ogre", 4,
    1, 100, 2, 1, 6, 1,
    LootBox{0.80, {Prototype::GREAT_AXE}},
    {Traits::NIGHT_VISION},
    Items{Prototype::GREAT_AXE},
};
} // namespace EnemyType
// clang-format on

class Enemy : public Creature, public eb::EventHandler<CommitEvent> {
public:
  Enemy(EnemySpec);
  ~Enemy();
  bool interact(std::shared_ptr<Object>) override;
  std::optional<Items> drop();

  Direction cd = Direction::W;
  int actionPoints = 0;
  EnemySpec type;
  micropather::MPVector<void *> path;
  micropather::MPVector<void *> pathToHero;
  int step;
  micropather::MicroPather *pather = nullptr;
  bool randomPath();

  virtual void onEvent(CommitEvent &e) override;
};

#endif // __ENEMY_H_
