#ifndef __ENEMIES_H_
#define __ENEMIES_H_
#include "lss/game/content/items.hpp"
#include "lss/game/content/traits.hpp"
#include "lss/game/enemy.hpp"

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
    "large bat", 1,
    1.5, 10, 0, 1,
    DamageSpec(0, 2, 3, DamageType::BASIC),
    LootBoxes::ZERO,
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}, {} //vampire
};

EnemySpec const ANT = {
    "giant ant", 0,
    1.5, 10, 0, 5,
    DamageSpec(0, 2, 3, DamageType::ACID),
    LootBoxes::ZERO,
    {Traits::MOB, Traits::FIRE_VULNERABLE},
    Items{}, {} //vampire
};

EnemySpec const ANT_QUEEN = {
    "giant ant", 0,
    1.5, 20, 0, 8,
    DamageSpec(1, 3, 3, DamageType::ACID),
    LootBoxes::ZERO,
    {Traits::MOB, Traits::FIRE_VULNERABLE},
    Items{}, {} //vampire
};

EnemySpec const GOBLIN = {
    "goblin", 1,
    1, 15, 0, 1,
    DamageSpec(0, 1, 3, DamageType::WEAPON),
    LootBox(1, {}, {
        LootBox(0.40, LootBoxes::POTIONS),
        LootBox(0.40, LootBoxes::LOOT_TIER_0),
        LootBox(0.90, {Prototype::GOLD->setCount(50)})}, false),
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::GOBLIN_DAGGER, Prototype::GOBLIN_DAGGER}
};

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
    }, false),
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::GOBLIN_DAGGER, Prototype::GOBLIN_DAGGER, Prototype::LEATHER_CUIRASS}
};

EnemySpec const GOBLIN_ROGUE = {
    "goblin rogue", 2,
    1.5, 15, 0, 1,
    DamageSpec(1, 1, 3, DamageType::WEAPON),
    LootBox(1, {}, {
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox(0.90, {Prototype::GOLD->setCount(50)})
    }, false),
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
    }, false),
    {}, Items{Prototype::TORCH, Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD}
};

EnemySpec const ORK_BERSERK = {
    "ork berserk", 3,
    1.5, 40, 0, 0,
    DamageSpec(1, 3, 8, DamageType::WEAPON),
    LootBox(1, {}, {
        LootBox(0.40, {Prototype::POTION_HEAL}),
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox(0.10, Prototype::ARTEFACTS_1),
        LootBox(0.90, {Prototype::GOLD->setCount(100)})
    }, false),
    {Traits::FIRE_VULNERABLE, Traits::BLOOD_THIRST}, Items{}
};

EnemySpec const ORK_BLACK = {
    "black ork", 3,
    1.1, 75, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox(1, {Prototype::TORCH}, {
        LootBox(0.50, {Prototype::POTION_HEAL}),
        LootBox(0.20, LootBoxes::POTIONS),
        LootBox(0.80, LootBoxes::LOOT_TIER_2),
        LootBox(0.90, {Prototype::GOLD->setCount(100)})
    }, false),
    {}, Items{Prototype::TORCH, Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD, Prototype::LEATHER_CUIRASS}
};

EnemySpec const PIXI = {
    "pixi", 3,
    3, 25, 30, 0,
    DamageSpec(0, 3, 4, DamageType::WEAPON),
    LootBox(0.90, {Prototype::GOLD->setCount(200)}, {
        LootBox(0.40, LootBoxes::SCROLLS),
        LootBox(0.10, {Prototype::SPEED_RING})}, false),
    {Traits::FLY, Traits::MOB}};

EnemySpec const OGRE = {
    "ogre", 4,
    1, 100, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox(0.80, {Prototype::GREAT_AXE}),
    {Traits::NIGHT_VISION, Traits::FIRE_VULNERABLE},
    Items{Prototype::GREAT_AXE},
};

EnemySpec const I_OGRE = {
    "loooong ogre", 4,
    1, 1000000, 0, 2,
    DamageSpec(0, 0, 0, DamageType::WEAPON),
    LootBoxes::ZERO,
    {},
    Items{},
};



EnemySpec const SKELETON = {
    "skeleton", 3,
    1, 55, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox(1, {}, {
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox(0.90, {Prototype::GOLD->setCount(100)})
    }, false),
    {Traits::WEAPON_RESIST, Traits::MAGIC_VULNERABLE}, Items{Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD}
};

EnemySpec const ZOMBIE = {
    "zombie", 3,
    0.5, 75, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox(1, {}, {
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox(0.90, {Prototype::GOLD->setCount(100)})
    }, false),
    {Traits::WEAPON_RESIST}, Items{Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD}
};

EnemySpec const WRAITH = {
    "wraith", 4,
    1.5, 45, 0, 2,
    DamageSpec(1, 3, 6, DamageType::MAGIC),
    LootBox(1, {}, {}, false),
    {Traits::WEAPON_IMMUNE}, Items{Prototype::WRAITH_TOUCH}
};

  const std::vector<EnemySpec> ALL = {
    RAT,
    SNAKE, VIPER,
    BAT, BAT_LARGE,
    ANT, ANT_QUEEN,
    GOBLIN, GOBLIN_ROGUE, GOBLIN_LIEUTENANT,
    ORK, ORK_BLACK, ORK_BERSERK,
    PIXI,
    OGRE,
    I_OGRE,

    SKELETON, ZOMBIE, WRAITH
  };
} // namespace EnemyType
// clang-format on

#endif // __ENEMIES_H_
