#ifndef __ITEMSPEC_H_
#define __ITEMSPEC_H_
#include "lss/game/wearableType.hpp"
#include <string>

struct ItemCategory {
  std::string name;
  friend bool operator!=(ItemCategory &lhs, const ItemCategory &rhs) {
    return lhs.name != rhs.name;
  }
  friend bool operator==(ItemCategory &lhs, const ItemCategory &rhs) {
    return lhs.name == rhs.name;
  }
};
namespace ItemCategories {
static const ItemCategory WEAPONS = {"Weapons"};
static const ItemCategory ARMOR = {"Armor"};
static const ItemCategory LIGHT = {"Light"};
static const ItemCategory JEWELLERY = {"Jewellery"};
static const ItemCategory CONSUMABLES = {"Consumables"};
static const ItemCategory MISC = {"Misc"};
} // namespace ItemCategories

struct ItemSpec {
public:
  std::string name;
  ItemCategory category;
  WearableType wearableType = INVALID;
  int durability = -1;

  friend bool operator<(const ItemSpec &lhs, const ItemSpec &rhs) {
    return lhs.name < rhs.name;
  }
  friend bool operator==(ItemSpec &lhs, const ItemSpec &rhs) {
    return lhs.name == rhs.name;
  }
};

namespace ItemType {
ItemSpec const TORCH_STAND = {"torch stand", ItemCategories::MISC};
ItemSpec const STATUE = {"stone statue", ItemCategories::MISC};

ItemSpec const GRASS = {"herb", ItemCategories::CONSUMABLES};
ItemSpec const SCROLL = {"scroll", ItemCategories::CONSUMABLES};
ItemSpec const POTION = {"potion", ItemCategories::CONSUMABLES};

ItemSpec const ROCK = {"rock", ItemCategories::MISC};
ItemSpec const PICK_AXE = {"pick axe", ItemCategories::WEAPONS,
                           WearableType::WEAPON_TWOHANDED};
ItemSpec const GREAT_AXE = {"great axe", ItemCategories::WEAPONS,
                            WearableType::WEAPON_TWOHANDED};
ItemSpec const SWORD = {"sword", ItemCategories::WEAPONS, WearableType::WEAPON};
ItemSpec const DAGGER = {"dagger", ItemCategories::WEAPONS,
                         WearableType::WEAPON_LIGHT};
ItemSpec const GOLD_RING = {"gold ring", ItemCategories::JEWELLERY,
                            WearableType::RING};
ItemSpec const TORCH = {"torch", ItemCategories::LIGHT, WearableType::LIGHT,
                        30000};
ItemSpec const GOLD_COINS = {"gold coins", ItemCategories::MISC};
ItemSpec const PLATE = {"plate", ItemCategories::ARMOR, WearableType::BODY};
ItemSpec const LEATHER_ARMOR = {"leather armor", ItemCategories::ARMOR,
                                WearableType::BODY};

ItemSpec const HELMET = {"helmet", ItemCategories::ARMOR, WearableType::HEAD};
ItemSpec const SHIELD = {"shield", ItemCategories::ARMOR, WearableType::SHIELD};
ItemSpec const GREAVES = {"greaves", ItemCategories::ARMOR,
                          WearableType::GREAVES};
ItemSpec const BOOTS = {"boots", ItemCategories::ARMOR, WearableType::BOOTS};
ItemSpec const CLOAK = {"cloak", ItemCategories::ARMOR, WearableType::CLOAK};

ItemSpec const RIGHT_PAULDRON = {"right pauldron", ItemCategories::ARMOR,
                                 WearableType::RIGHT_PAULDRON};
ItemSpec const LEFT_PAULDRON = {"left pauldron", ItemCategories::ARMOR,
                                WearableType::LEFT_PAULDRON};
ItemSpec const RIGHT_GAUNTLET = {"right gauntlet", ItemCategories::ARMOR,
                                 WearableType::RIGHT_GAUNTLET};
ItemSpec const LEFT_GAUNTLET = {"left gauntlet", ItemCategories::ARMOR,
                                WearableType::LEFT_GAUNTLET};

ItemSpec const CORPSE = {"corpse", ItemCategories::CONSUMABLES};
}; // namespace ItemType

#endif // __ITEMSPEC_H_
