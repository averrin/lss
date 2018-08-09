#ifndef __LOOTBOX_H_
#define __LOOTBOX_H_
#include "lss/game/content/items.hpp"
#include "lss/game/item.hpp"
#include "lss/utils.hpp"

struct LootBox {
  LootBox() {}
  LootBox(float p, std::vector<std::shared_ptr<Item>> loot,
          std::vector<LootBox> lbs)
      : probability(p), items(loot), children(lbs) {}
  LootBox(float p, std::vector<std::shared_ptr<Item>> loot,
          std::vector<LootBox> lbs, bool e)
      : probability(p), items(loot), children(lbs), exclusive(e) {}

  LootBox(float p, std::vector<std::shared_ptr<Item>> loot)
      : probability(p), items(loot) {
    probability = p;
    items = loot;
  }

  LootBox(float p, LootBox lb)
      : probability(p), children({lb}), exclusive(lb.exclusive) {}

  LootBox(std::vector<std::shared_ptr<Item>> loot) {
    exclusive = true;
    float p = 1.f / loot.size();
    for (auto i : loot) {
      children.push_back(LootBox(p, {i}));
    }
  }
  float probability = 1;
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
const LootBox ZERO = LootBox();
const LootBox POTIONS = LootBox(Prototype::POTIONS);
const LootBox SCROLLS = LootBox(Prototype::SCROLLS);

const LootBox LOOT_TIER_0 = LootBox(Prototype::LOOT_0);
const LootBox LOOT_TIER_1 = LootBox(Prototype::LOOT_1);

const LootBox ARMOR_TIER_1 = LootBox(Prototype::LEATHER_ARMOR);
const LootBox ARMOR_TIER_2 = LootBox(Prototype::IRON_ARMOR);
const LootBox ARMOR_TIER_3 = LootBox(Prototype::STEEL_ARMOR);
const LootBox WEAPONS_TIER_1 = LootBox(Prototype::WEAPONS_1);
const LootBox WEAPONS_TIER_2 = LootBox(Prototype::WEAPONS_2);

const LootBox DUNGEON_0 = ZERO;
const LootBox DUNGEON_1 = ZERO;
const LootBox DUNGEON_2 = ZERO;
const LootBox DUNGEON_3 = ZERO;
}; // namespace LootBoxes

#endif // __LOOTBOX_H_
