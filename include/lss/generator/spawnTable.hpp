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
     }},
    {1,
     {
         {EnemyType::RAT, 0.5},
         {EnemyType::BAT, 0.4},
         {EnemyType::SNAKE, 0.3},
         {EnemyType::VIPER, 0.2},
         {EnemyType::BAT_LARGE, 0.15},
         {EnemyType::GOBLIN, 0.4},
         {EnemyType::GOBLIN_ROGUE, 0.1},
     }},
    {2,
     {
         {EnemyType::BAT, 0.2},
         {EnemyType::BAT_LARGE, 0.4},
         {EnemyType::SNAKE, 0.3},
         {EnemyType::VIPER, 0.2},
         {EnemyType::GOBLIN, 0.4},
         {EnemyType::GOBLIN_LIEUTENANT, 0.2},
         {EnemyType::GOBLIN_ROGUE, 0.1},
     }},
    {3,
     {
         {EnemyType::BAT_LARGE, 0.1},
         {EnemyType::ORK, 0.4},
         {EnemyType::ORK_BLACK, 0.4},
         {EnemyType::GOBLIN_LIEUTENANT, 0.2},
         {EnemyType::GOBLIN_ROGUE, 0.1},
     }},
};
}

namespace LootTable {
std::map<int, LootBox> DUNGEON = {
    {0, LootBox(0.06, LootBoxes::DUNGEON_0)},
    {1, LootBox(0.08, LootBoxes::DUNGEON_1)},
    {2, LootBox(0.10, LootBoxes::DUNGEON_2)},
    {3, LootBox(0.12, LootBoxes::DUNGEON_3)},
};
}

#endif // __SPAWNTABLE_H_
