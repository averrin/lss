#ifndef __ENEMIES_H_
#define __ENEMIES_H_
#include "lss/game/enemy.hpp"
#include "lss/game/content/items.hpp"
#include "lss/game/content/traits.hpp"

const auto ds_1d3 = DamageSpec(0, 1, 3, DamageType::BASIC);

// clang-format off
namespace EnemyType {
EnemySpec const RAT = {
    "rat", 0,
    2, 3, 0, 1,
    ds_1d3,
    LootBoxes::ZERO,
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB},
    Items{}
};

EnemySpec const SNAKE = {
    "snake", 0,
    1.5, 3, 0, 1,
    ds_1d3,
    LootBoxes::ZERO,
    {},
    Items{}
};

EnemySpec const VIPER = {
    "viper", 0,
    2, 3, 0, 1,
    ds_1d3,
    LootBoxes::ZERO,
    {},
    Items{Prototype::POISON_FANG}
};
EnemySpec const BAT = {
    "bat", 0,
    1.5, 3, 0, 1,
    ds_1d3,
    LootBoxes::ZERO,
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}, {} //vampire
};
EnemySpec const BAT_LARGE = {
    "large bat", 0,
    1.5, 10, 0, 1,
    DamageSpec(0, 2, 3, DamageType::BASIC),
    LootBoxes::ZERO,
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}, {} //vampire
};
EnemySpec const GOBLIN = {
    "goblin", 1,
    1, 15, 0, 1,
    DamageSpec(0, 1, 3, DamageType::WEAPON),
    LootBox(1, {}, {
        LootBox(0.40, LootBoxes::POTIONS),
        LootBox(0.40, LootBoxes::LOOT_TIER_0),
        LootBox(0.90, {Prototype::GOLD->setCount(50)})}),
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::GOBLIN_DAGGER, Prototype::GOBLIN_DAGGER}
};
// Goblin lieutenant
EnemySpec const GOBLIN_LIEUTENANT = {
    "goblin lieutenant", 1,
    1, 25, 0, 1,
    DamageSpec(1, 1, 5, DamageType::WEAPON),
    LootBox(1, {}, {
        LootBox(0.40, {Prototype::POTION_HEAL}),
        LootBox(0.20, LootBoxes::POTIONS),
        LootBox(0.40, LootBoxes::SCROLLS),
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox(0.90, {Prototype::GOLD->setCount(50)})
    }),
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::GOBLIN_DAGGER, Prototype::GOBLIN_DAGGER, Prototype::LEATHER_CUIRASS}
};

EnemySpec const GOBLIN_ROGUE = {
    "goblin rogue", 1,
    1.5, 15, 0, 1,
    DamageSpec(1, 1, 3, DamageType::WEAPON),
    LootBox(1, {}, {
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox(0.90, {Prototype::GOLD->setCount(50)})
    }),
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::POISON_DAGGER, Prototype::GOBLIN_DAGGER}
};
EnemySpec const ORK = {
    "ork", 2,
    1, 55, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox(1, {Prototype::TORCH}, {
        LootBox(0.40, {Prototype::POTION_HEAL}),
        LootBox(0.40, LootBoxes::SCROLLS),
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox(0.90, {Prototype::GOLD->setCount(100)})
    }),
    {}, Items{Prototype::TORCH, Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD}
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
EnemySpec const I_OGRE = {
    "loooong ogre", 4,
    1, 1000000, 0, 2,
    DamageSpec(0, 0, 0, DamageType::WEAPON),
    LootBox{},
    {},
    Items{},
};

  const std::vector<EnemySpec> ALL = {
    RAT,
    SNAKE, VIPER,
    BAT, BAT_LARGE,
    GOBLIN, GOBLIN_ROGUE, GOBLIN_LIEUTENANT,
    ORK, ORK_BLACK,
    PIXI,
    OGRE,
    I_OGRE,
  };
} // namespace EnemyType
// clang-format on

#endif // __ENEMIES_H_
