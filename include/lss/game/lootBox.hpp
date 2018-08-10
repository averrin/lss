#ifndef __LOOTBOX_H_
#define __LOOTBOX_H_
#include "lss/game/content/items.hpp"
#include "lss/game/item.hpp"
#include "lss/utils.hpp"

struct LootBox {
  LootBox() {}
  LootBox(float p, std::vector<std::shared_ptr<Item>> loot,
          std::vector<LootBox> lbs = {}, bool e = false)
      : probability(p), items(loot), children(lbs), exclusive(e) {}

  LootBox(float p, LootBox lb)
      : probability(p), children({lb}), exclusive(lb.exclusive) {}

  LootBox(std::vector<LootBox> lbs) : children(lbs), exclusive(true) {}

  LootBox(std::vector<std::shared_ptr<Item>> loot) {
    exclusive = true;
    float p = 1.f / loot.size();
    for (auto i : loot) {
      children.push_back(LootBox(p, {i}, {}, true));
    }
  }
  float probability = 1;
  Items items;
  std::vector<LootBox> children;
  bool exclusive = false;

  Items open() {
    fmt::print("exclusive: {}\n", exclusive);
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

const LootBox ARMOR_TIER_1 = LootBox(Prototype::LEATHER_ARMOR);
const LootBox ARMOR_TIER_2 = LootBox(Prototype::IRON_ARMOR);
const LootBox ARMOR_TIER_3 = LootBox(Prototype::STEEL_ARMOR);
const LootBox WEAPONS_TIER_1 = LootBox(Prototype::WEAPONS_1);
const LootBox WEAPONS_TIER_2 = LootBox(Prototype::WEAPONS_2);

/* Enemy loot */
const LootBox LOOT_TIER_0 = LootBox(Prototype::LOOT_0);
const LootBox LOOT_TIER_1 = LootBox(
    {LootBox(0.3, LOOT_TIER_0), LootBox(0.7, Prototype::LOOT_1, {}, true)});

/* Location loot */
const LootBox DUNGEON_0 = LootBox({
    LootBox(0.3, LOOT_TIER_0), LootBox(0.3, POTIONS), LootBox(0.3, SCROLLS),
    LootBox(0.1, Prototype::ARTEFACTS_0, {}, true),
});
const LootBox DUNGEON_1 = LootBox({
    LootBox(0.3, LOOT_TIER_1), LootBox(0.3, POTIONS), LootBox(0.3, SCROLLS),
    LootBox(0.1, Prototype::ARTEFACTS_1, {}, true),
});
const LootBox DUNGEON_2 = DUNGEON_1;
const LootBox DUNGEON_3 = DUNGEON_2;
}; // namespace LootBoxes

#endif // __LOOTBOX_H_
