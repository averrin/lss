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
static const ItemCategory THROWABLE = {"Throwable"};
static const ItemCategory USABLE = {"Usable"};
} // namespace ItemCategories

struct ItemSpec {
public:
  std::string name;
  ItemCategory category;
  WearableType wearableType = INVALID;
  int durability = -1;
  bool identified = false;

  friend bool operator<(const ItemSpec &lhs, const ItemSpec &rhs) {
    return lhs.name < rhs.name;
  }
  friend bool operator==(ItemSpec &lhs, const ItemSpec &rhs) {
    return lhs.name == rhs.name;
  }
};

#endif // __ITEMSPEC_H_
