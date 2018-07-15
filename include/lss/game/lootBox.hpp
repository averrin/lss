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
    // TODO: implement float probability
    auto roll = R::R();
    std::vector<std::string> itemNames;
    for (auto i : items) {
      itemNames.push_back(i->getFullTitle());
    }
    // fmt::print("LootBox roll: {}/{} [{}]\n", roll, probability,
    // utils::join(itemNames, ", "));
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
    return loot;
  };
};

#endif // __LOOTBOX_H_
