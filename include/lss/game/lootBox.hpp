#ifndef __LOOTBOX_H_
#define __LOOTBOX_H_
#include "lss/game/item.hpp"
#include "lss/game/content/items.hpp"
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
    {0.1, {Prototype::POTION_HEAL_LESSER}},
    {0.1, {Prototype::POTION_HEAL}},
    {0.1, {Prototype::POTION_GOD_SPEED}},
    // {.30, {Prototype::POTION_HP_BOOST}},
    {0.1, {Prototype::POTION_VISIBILITY_BOOST}},
    {0.1, {Prototype::POTION_CRIT_BOOST}},
    {0.1, {Prototype::POTION_LEVITATION}},
    {0.1, {Prototype::POTION_REGENERATION}},
    {0.1, {Prototype::POTION_POISON}},
    {0.1, {Prototype::POTION_CONFUSION}},
    {0.1, {Prototype::POTION_MANA}},
};

const std::vector<LootBox> SCROLLS = {
    {0.30, {Prototype::SCROLL_IDENTIFICATION}},
    {0.30, {Prototype::SCROLL_REVEAL}},
    {0.30, {Prototype::SCROLL_TELEPORT}}};

const std::vector<LootBox> WEAPONS_TIER_1 = {
    {0.50, {Prototype::SWORD}},
    {0.50, {Prototype::DAGGER}}
};
const std::vector<LootBox> WEAPONS_TIER_2 = {
    // {0.50, {Prototype::POISON_SWORD}},
    {0.50, {Prototype::POISON_DAGGER}}
};
const std::vector<LootBox> ARMOR_TIER_1 = {
  {0.20, {Prototype::PLATE}},
  {0.20, {Prototype::HELMET}},
  {0.20, {Prototype::SHIELD}},
  {0.20, {Prototype::GREAVES}},
  {0.20, {Prototype::BOOTS}},
};
}; // namespace LootBoxes

#endif // __LOOTBOX_H_
