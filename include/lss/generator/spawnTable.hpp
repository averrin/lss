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
         {EnemyType::GOBLIN_ROGUE, 0.1},
     }},
    {2,
     {
         {EnemyType::BAT, 0.2},
         {EnemyType::BAT_LARGE, 0.4},
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
    {0, {0.01, {}, {
          {0.10, {Prototype::PLATE}},
          {0.10, {Prototype::SWORD}},
          {0.10, {Prototype::HELMET}},
          {0.10, {Prototype::SHIELD}},
          {0.10, {Prototype::GREAVES}},
          {0.10, {Prototype::BOOTS}},
          {0.10, {Prototype::SCROLL_IDENTIFICATION}},
          {0.10, {Prototype::SCROLL_REVEAL}},
          {0.10, {Prototype::POTION_HEAL}},
          {0.10, {Prototype::POTION_HEAL_LESSER}},
          }, true}},
    {1, {0.015, {}, {
          {0.10, {Prototype::PLATE}},
          {0.10, {Prototype::SWORD}},
          {0.10, {Prototype::HELMET}},
          {0.10, {Prototype::SHIELD}},
          {0.10, {Prototype::GREAVES}},
          {0.10, {Prototype::BOOTS}},
          {0.10, {Prototype::SCROLL_IDENTIFICATION}},
          {0.10, {Prototype::SCROLL_REVEAL}},
          {0.10, {Prototype::POTION_HEAL}},
          {0.10, {Prototype::POTION_HEAL_LESSER}},
          }, true}}  
    };
}

#endif // __SPAWNTABLE_H_
