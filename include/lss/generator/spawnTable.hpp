#ifndef __SPAWNTABLE_H_
#define __SPAWNTABLE_H_
#include "lss/game/content/enemies.hpp"
#include "lss/game/enemy.hpp"
#include <map>

namespace SpawnTable {
std::map<int, std::map<const EnemySpec, float>> DUNGEON = {
    {0,
     {
         {EnemyType::RAT, 0.4},
         {EnemyType::BAT, 0.3},
         {EnemyType::SNAKE, 0.3},
         {EnemyType::VIPER, 0.2},
         {EnemyType::BAT_LARGE, 0.1},
         {EnemyType::GOBLIN_ROCKTHROWER, 0.1},
         {EnemyType::GOBLIN, 0.05},
     }},
    {1,
     {
         {EnemyType::RAT, 0.5},
         {EnemyType::BAT, 0.4},
         {EnemyType::ANT, 0.1},
         {EnemyType::SNAKE, 0.3},
         {EnemyType::VIPER, 0.2},
         {EnemyType::BAT_LARGE, 0.15},
         {EnemyType::GOBLIN_ROCKTHROWER, 0.4},
         {EnemyType::GOBLIN, 0.4},
         {EnemyType::GOBLIN_ROGUE, 0.1},
         {EnemyType::GOBLIN_LIEUTENANT, 0.02},
     }},
    {2,
     {
         {EnemyType::ANT, 0.3},
         {EnemyType::ANT_QUEEN, 0.1},
         {EnemyType::BAT_LARGE, 0.4},
         {EnemyType::SNAKE, 0.3},
         {EnemyType::VIPER, 0.2},
         {EnemyType::GOBLIN, 0.4},
         {EnemyType::GOBLIN_ROCKTHROWER, 0.4},
         {EnemyType::GOBLIN_LIEUTENANT, 0.2},
         {EnemyType::GOBLIN_ROGUE, 0.1},
     }},
    {3,
     {
         {EnemyType::ANT, 0.3},
         {EnemyType::ANT_QUEEN, 0.2},
         {EnemyType::ORK, 0.1},
         {EnemyType::VIPER, 0.05},
         {EnemyType::GOBLIN_LIEUTENANT, 0.2},
         {EnemyType::ORK_BERSERK, 0.4},
         {EnemyType::ORK_BLACK, 0.5},
         {EnemyType::OGRE, 0.1},
     }},
    {4,
     {
         {EnemyType::GOBLIN_LIEUTENANT, 0.02},
         {EnemyType::GOBLIN_ROGUE, 0.01},
         {EnemyType::ORK_BERSERK, 0.4},
         {EnemyType::ORK_BLACK, 0.5},
         {EnemyType::OGRE, 0.3},
         {EnemyType::SKELETON, 0.3},
         {EnemyType::ZOMBIE, 0.3},
         {EnemyType::WRAITH, 0.1},
     }},
    {5,
     {
         {EnemyType::ORK_BERSERK, 0.07},
         {EnemyType::ORK_BLACK, 0.05},
         {EnemyType::OGRE, 0.3},
         {EnemyType::SKELETON, 0.3},
         {EnemyType::ZOMBIE, 0.3},
         {EnemyType::WRAITH, 0.1},
     }},
    {6,
     {
         {EnemyType::FIRE_ELEMENTAL, 0.3},
         {EnemyType::FROST_ELEMENTAL, 0.3},
         {EnemyType::ACID_ELEMENTAL, 0.3},
         {EnemyType::WRAITH, 0.3},
         {EnemyType::SKELETON, 0.03},
         {EnemyType::ZOMBIE, 0.03},
     }},
    {7,
     {
         {EnemyType::FIRE_ELEMENTAL, 0.3},
         {EnemyType::FROST_ELEMENTAL, 0.3},
         {EnemyType::ACID_ELEMENTAL, 0.3},
         {EnemyType::STONE_GOLEM, 0.1},
         {EnemyType::STEEL_GOLEM, 0.1},
         {EnemyType::WRAITH, 0.05},
     }},
    {8,
     {
         {EnemyType::FIRE_ELEMENTAL, 0.05},
         {EnemyType::FROST_ELEMENTAL, 0.05},
         {EnemyType::ACID_ELEMENTAL, 0.05},
         {EnemyType::STONE_GOLEM, 0.3},
         {EnemyType::STEEL_GOLEM, 0.3},
     }},
    {9,
     {
         {EnemyType::FIRE_DRAGON, 0.3},
         {EnemyType::FROST_DRAGON, 0.3},
         {EnemyType::ACID_DRAGON, 0.3},
     }},
};
}

namespace LootTable {
std::map<int, LootBox> DUNGEON = {
    {0, LootBox(0.08, LootBoxes::DUNGEON_0)},
    {1, LootBox(0.10, LootBoxes::DUNGEON_0)},
    {2, LootBox(0.12, LootBoxes::DUNGEON_1)},
    {3, LootBox(0.14, LootBoxes::DUNGEON_1)},
    {4, LootBox(0.16, LootBoxes::DUNGEON_2)},
    {5, LootBox(0.18, LootBoxes::DUNGEON_2)},
    {6, LootBox(0.20, LootBoxes::DUNGEON_3)},
    {7, LootBox(0.22, LootBoxes::DUNGEON_3)},
    {8, LootBox(0.24, LootBoxes::DUNGEON_4)},
    {9, LootBox(0.26, LootBoxes::DUNGEON_4)},
};
}

#endif // __SPAWNTABLE_H_
