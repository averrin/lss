#ifndef __SPAWNTABLE_H_
#define __SPAWNTABLE_H_
#include "lss/game/enemy.hpp"
#include <map>

namespace SpawnTable {
std::map<int, std::map<const EnemySpec, float>> DUNGEON = {
    {0,
     {
         {EnemyType::RAT, 0.5},
         {EnemyType::BAT, 0.4},
         {EnemyType::BAT_LARGE, 0.1},
     }},
    {1,
     {
         {EnemyType::RAT, 0.5},
         {EnemyType::BAT, 0.4},
         {EnemyType::BAT_LARGE, 0.15},
         {EnemyType::GOBLIN, 0.4},
     }},
    {2,
     {
         {EnemyType::BAT, 0.2},
         {EnemyType::BAT_LARGE, 0.4},
         {EnemyType::GOBLIN, 0.4},
         {EnemyType::GOBLIN_LIEUTENANT, 0.2},
     }}

};
}

#endif // __SPAWNTABLE_H_
