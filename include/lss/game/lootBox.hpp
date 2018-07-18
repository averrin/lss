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
    //TODO: remove useless roll calls
    std::transform(loot.begin(), loot.end(), loot.begin(), [](auto item) {return item->roll();});
    return loot;
  };
};

#endif // __LOOTBOX_H_
