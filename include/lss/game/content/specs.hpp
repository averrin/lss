#ifndef __SPECS_H_
#define __SPECS_H_
#include "lss/game/itemSpec.hpp"

namespace ItemType {
ItemSpec const EMPTY = {"strange item", ItemCategories::USABLE};
ItemSpec const GRASS = {"herb", ItemCategories::CONSUMABLES};
ItemSpec const SCROLL = {"scroll", ItemCategories::CONSUMABLES};
ItemSpec const POTION = {"potion", ItemCategories::CONSUMABLES};

ItemSpec const ROCK = {"rock", ItemCategories::THROWABLE};
ItemSpec const BONES = {"bones", ItemCategories::MISC};
ItemSpec const CLUTTER = {"clutter", ItemCategories::MISC, WearableType::INVALID, -1, true};

ItemSpec const ENEMY = {"enemy item", ItemCategories::MISC,
                        WearableType::ENEMY};

ItemSpec const PICK_AXE = {"pick axe", ItemCategories::WEAPONS,
                           WearableType::WEAPON_TWOHANDED};

ItemSpec const TWO_HAND = {"great axe", ItemCategories::WEAPONS,
                           WearableType::WEAPON_TWOHANDED};
ItemSpec const ONE_HAND = {"sword", ItemCategories::WEAPONS,
                           WearableType::WEAPON};
ItemSpec const LIGHT_WEAPON = {"dagger", ItemCategories::WEAPONS,
                               WearableType::WEAPON_LIGHT};
ItemSpec const RING = {"ring", ItemCategories::JEWELLERY, WearableType::RING};
ItemSpec const AMULET = {"amulet", ItemCategories::JEWELLERY,
                         WearableType::NECK};
ItemSpec const TORCH = {"torch", ItemCategories::LIGHT, WearableType::LIGHT,
                        30000};
ItemSpec const CANDLE = {"candle", ItemCategories::LIGHT, WearableType::LIGHT,
                        45000};
ItemSpec const LANTERN = {"lantern", ItemCategories::LIGHT, WearableType::LIGHT,
                          60000};
ItemSpec const GOLD_COINS = {"gold coins", ItemCategories::MISC};
ItemSpec const PLATE = {"plate", ItemCategories::ARMOR, WearableType::BODY};
ItemSpec const CUIRASS = {"cuirass", ItemCategories::ARMOR, WearableType::BODY};

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

ItemSpec const THROWING_KNIVES = {"throwing knives", ItemCategories::THROWABLE};
}; // namespace ItemType

#endif // __SPECS_H_
