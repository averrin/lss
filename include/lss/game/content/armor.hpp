#ifndef __ARMOR_H_
#define __ARMOR_H_

#include "lss/game/content/specs.hpp"
#include "lss/game/content/spells.hpp"
#include "lss/game/item.hpp"

namespace Prototype {

/* Leather armor */
const auto LEATHER_CUIRASS =
    std::make_shared<Item>("leather cuirass", ItemType::CUIRASS,
                           Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto LEATHER_HELMET =
    std::make_shared<Item>("leather helm", ItemType::HELMET,
                           Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto LEATHER_SHIELD =
    std::make_shared<Item>("leather shield", ItemType::SHIELD,
                           Effects{std::make_shared<ArmorValue>(R::I(2, 3))});
const auto LEATHER_GREAVES =
    std::make_shared<Item>("leather greaves", ItemType::GREAVES,
                           Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto LEATHER_BOOTS =
    std::make_shared<Item>("leather boots", ItemType::BOOTS,
                           Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_RIGHT_PAULDRON =
    std::make_shared<Item>("leather right pauldron", ItemType::RIGHT_PAULDRON,
                           Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_LEFT_PAULDRON =
    std::make_shared<Item>("leather left pauldron", ItemType::LEFT_PAULDRON,
                           Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_RIGHT_GAUNTLET =
    std::make_shared<Item>("leather right gauntlet", ItemType::RIGHT_GAUNTLET,
                           Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_LEFT_GAUNTLET =
    std::make_shared<Item>("leather left gauntlet", ItemType::LEFT_GAUNTLET,
                           Effects{std::make_shared<ArmorValue>(R::I(1, 2))});

/* Iron armor */
const auto IRON_CUIRASS =
    std::make_shared<Item>("iron cuirass", ItemType::CUIRASS,
                           Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto IRON_HELMET =
    std::make_shared<Item>("iron helm", ItemType::HELMET,
                           Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto IRON_SHIELD =
    std::make_shared<Item>("iron shield", ItemType::SHIELD,
                           Effects{std::make_shared<ArmorValue>(R::I(4, 5))});
const auto IRON_GREAVES =
    std::make_shared<Item>("iron greaves", ItemType::GREAVES,
                           Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto IRON_BOOTS =
    std::make_shared<Item>("iron boots", ItemType::BOOTS,
                           Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_RIGHT_PAULDRON =
    std::make_shared<Item>("iron right pauldron", ItemType::RIGHT_PAULDRON,
                           Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_LEFT_PAULDRON =
    std::make_shared<Item>("iron left pauldron", ItemType::LEFT_PAULDRON,
                           Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_RIGHT_GAUNTLET =
    std::make_shared<Item>("iron right gauntlet", ItemType::RIGHT_GAUNTLET,
                           Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_LEFT_GAUNTLET =
    std::make_shared<Item>("iron left gauntlet", ItemType::LEFT_GAUNTLET,
                           Effects{std::make_shared<ArmorValue>(R::I(3, 4))});

/* Steel armor */
const auto STEEL_CUIRASS =
    std::make_shared<Item>("steel cuirass", ItemType::CUIRASS,
                           Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_HELMET =
    std::make_shared<Item>("steel helm", ItemType::HELMET,
                           Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_SHIELD =
    std::make_shared<Item>("steel shield", ItemType::SHIELD,
                           Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_GREAVES =
    std::make_shared<Item>("steel greaves", ItemType::GREAVES,
                           Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_BOOTS =
    std::make_shared<Item>("steel boots", ItemType::BOOTS,
                           Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_RIGHT_PAULDRON =
    std::make_shared<Item>("steel right pauldron", ItemType::RIGHT_PAULDRON,
                           Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_LEFT_PAULDRON =
    std::make_shared<Item>("steel left pauldron", ItemType::LEFT_PAULDRON,
                           Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_RIGHT_GAUNTLET =
    std::make_shared<Item>("steel right gauntlet", ItemType::RIGHT_GAUNTLET,
                           Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_LEFT_GAUNTLET =
    std::make_shared<Item>("steel left gauntlet", ItemType::LEFT_GAUNTLET,
                           Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
}

#endif // __ARMOR_H_
