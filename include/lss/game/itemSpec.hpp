#ifndef __ITEMSPEC_H_
#define __ITEMSPEC_H_
#include "lss/game/wearableType.hpp"
#include <string>

struct ItemSpec {
public:
  std::string name;
  bool equipable = false;
  WearableType wearableType;

  friend bool operator<(const ItemSpec &lhs, const ItemSpec &rhs) {
    return lhs.name < rhs.name;
  }
  friend bool operator==(ItemSpec &lhs, const ItemSpec &rhs) {
    return lhs.name == rhs.name;
  }
};

namespace ItemType {
ItemSpec const CORPSE = {"corpse", false};
ItemSpec const ROCK = {"rock", false};
ItemSpec const PICK_AXE = {"pick axe", true, WearableType::WEAPON_TWOHANDED};
ItemSpec const GREAT_AXE = {"great axe", true, WearableType::WEAPON_TWOHANDED};
ItemSpec const SWORD = {"sword", true, WearableType::WEAPON};
ItemSpec const DAGGER = {"dagger", true, WearableType::WEAPON_LIGHT};
ItemSpec const GOLD_RING = {"ring", true, WearableType::RING};
ItemSpec const TORCH = {"torch", true, WearableType::LIGHT};
ItemSpec const GOLD_COINS = {"gold coins", false};
ItemSpec const TORCH_STAND = {"torch stand", false};
ItemSpec const PLATE = {"plate", true, WearableType::BODY};
}; // namespace ItemType

#endif // __ITEMSPEC_H_
