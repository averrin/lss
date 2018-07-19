#ifndef __LOOTBOX_H_
#define __LOOTBOX_H_
#include "lss/game/item.hpp"
#include "lss/utils.hpp"

struct LootBox {
  float probability = 100;
  Items items;
  std::vector<LootBox> children;
  bool exclusive = false;

  Items open() {
    Items loot;
    auto roll = R::R();
    if (roll < probability) {
      loot.insert(loot.end(), items.begin(), items.end());
      for (auto child : children) {
        auto childLoot = child.open();
        if (childLoot.size() > 0) {
          loot.insert(loot.end(), childLoot.begin(), childLoot.end());
          if (exclusive)
            break;
        }
      }
    }
    // TODO: remove useless roll calls
    std::transform(loot.begin(), loot.end(), loot.begin(),
                   [](auto item) { return item->roll(); });
    return loot;
  };
};

namespace LootBoxes {
const std::vector<LootBox> POTIONS = {
    {0.14, {Prototype::POTION_HEAL_LESSER}},
    {0.14, {Prototype::POTION_HEAL}},
    {0.14, {Prototype::POTION_GOD_SPEED}},
    // {0.30, {Prototype::POTION_HP_BOOST}},
    {0.14, {Prototype::POTION_VISIBILITY_BOOST}},
    {0.14, {Prototype::POTION_CRIT_BOOST}},
    {0.14, {Prototype::POTION_LEVITATION}},
    {0.14, {Prototype::POTION_REGENERATION}},
};
const std::vector<LootBox> SCROLLS = {
    {0.30, {Prototype::SCROLL_IDENTIFICATION}},
    {0.30, {Prototype::SCROLL_REVEAL}},
    {0.30, {Prototype::SCROLL_TELEPORT}}};
;
};

#endif // __LOOTBOX_H_
