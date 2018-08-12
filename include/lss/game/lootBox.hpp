#ifndef __LOOTBOX_H_
#define __LOOTBOX_H_
#include "lss/game/content/items.hpp"
#include "lss/game/item.hpp"
#include "lss/utils.hpp"
#include <algorithm>

struct LootBox {
  LootBox() {}
  LootBox(float p, std::vector<std::shared_ptr<Item>> loot,
          std::vector<LootBox> lbs = {}, bool e = true)
      : probability(p), items(loot), children(lbs), exclusive(e) {}

  LootBox(float p, LootBox lb)
      : probability(p), children({lb}), exclusive(lb.exclusive) {}

  LootBox(std::vector<LootBox> lbs) : children(lbs), exclusive(true) {}

  LootBox(std::vector<std::shared_ptr<Item>> loot) : exclusive(true) {
    float p = 1.f / loot.size();
    children.resize(loot.size());
    std::transform(loot.begin(), loot.end(), children.begin(),
                   [p](auto i) { return LootBox(p, {i}, {}, true); });
  }
  float probability = 1.f;
  Items items;
  std::vector<LootBox> children;
  bool exclusive = true;

  Items open(bool force = false) {
    Items loot;
    auto roll = R::R();

    if (roll <= probability || force) {
      loot.insert(loot.end(), items.begin(), items.end());

      float fullP = 0;
      std::for_each(children.begin(), children.end(),
                    [&](auto box) { fullP += box.probability; });
      roll *= fullP;
      float ap = 0;
      for (auto child : children) {
        ap += child.probability;
        if (roll < ap) {
          auto childLoot = child.open(true);
          if (childLoot.size() > 0) {
            loot.insert(loot.end(), childLoot.begin(), childLoot.end());
            if (exclusive)
              break;
          }
        }
      }
    }
    // TODO: remove useless roll calls
    std::transform(loot.begin(), loot.end(), loot.begin(),
                   [](auto item) { return item->roll(); });
    return loot;
  };
};

class LootBoxHolder : public Object {
public:
  LootBoxHolder(LootBox s) : Object(), box(s) {}
  LootBox box;
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
const LootBox WEAPONS_TIER_3 = LootBox(Prototype::WEAPONS_3);

/* Enemy loot */
const LootBox LOOT_TIER_0 = LootBox(Prototype::LOOT_0);
const LootBox LOOT_TIER_1 = LootBox(
    {LootBox(0.3, LOOT_TIER_0), LootBox(0.7, LootBox(Prototype::LOOT_1))});
const LootBox LOOT_TIER_2 = LootBox(
    {LootBox(0.3, LOOT_TIER_1), LootBox(0.7, LootBox(Prototype::LOOT_2))});
const LootBox LOOT_TIER_3 = LootBox(
    {LootBox(0.3, LOOT_TIER_2), LootBox(0.7, LootBox(Prototype::LOOT_3))});

/* Location loot */
const LootBox DUNGEON_0 = LootBox({
    LootBox(0.3, LOOT_TIER_0),
    LootBox(0.3, POTIONS),
    LootBox(0.3, SCROLLS),
    LootBox(0.1, LootBox(Prototype::ARTIFACTS_0)),
});
const LootBox DUNGEON_1 = LootBox({
    LootBox(0.3, LOOT_TIER_1),
    LootBox(0.3, POTIONS),
    LootBox(0.3, SCROLLS),
    LootBox(0.1, LootBox(Prototype::ARTIFACTS_1)),
});
const LootBox DUNGEON_2 = LootBox({
    LootBox(0.3, LOOT_TIER_2),
    LootBox(0.3, POTIONS),
    LootBox(0.3, SCROLLS),
    LootBox(0.1, LootBox(Prototype::ARTIFACTS_2)),
});

const LootBox DUNGEON_3 = LootBox({
    LootBox(0.3, LOOT_TIER_3),
    LootBox(0.3, POTIONS),
    LootBox(0.3, SCROLLS),
    LootBox(0.1, LootBox(Prototype::ARTIFACTS_3)),
});

const LootBox DUNGEON_4 = DUNGEON_3;

const std::vector<LootBox> ALL = {
    POTIONS,        SCROLLS,        ARMOR_TIER_1, ARMOR_TIER_2, ARMOR_TIER_3,
    WEAPONS_TIER_1, WEAPONS_TIER_2, LOOT_TIER_0,  LOOT_TIER_1,  LOOT_TIER_2,
    DUNGEON_0,      DUNGEON_1,      DUNGEON_2,    DUNGEON_3,
};
} // namespace LootBoxes

#endif // __MAGIC_H_
