#ifndef __SCROLLS_H_
#define __SCROLLS_H_
#include "lss/game/content/specs.hpp"
#include "lss/game/content/spells.hpp"
#include "lss/game/item.hpp"

namespace Prototype {
const auto SCROLL_IDENTIFICATION = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of identification", ItemType::SCROLL, Spells::IDENTIFY);
const auto SCROLL_REVEAL = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of reveal", ItemType::SCROLL, Spells::REVEAL);
const auto SCROLL_TELEPORT = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of teleport", ItemType::SCROLL, Spells::TELEPORT_RANDOM);

const auto SCROLL_WEAPON_RESIST = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of weapon resistance", ItemType::SCROLL, Spells::WEAPON_RESIST);
const auto SCROLL_WEAPON_IMMUNE = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of weapon immunity", ItemType::SCROLL, Spells::WEAPON_IMMUNE);
const auto SCROLL_MAGIC_RESIST = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of magic resistance", ItemType::SCROLL, Spells::MAGIC_RESIST);
const auto SCROLL_MAGIC_IMMUNE = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of magic immunity", ItemType::SCROLL, Spells::MAGIC_IMMUNE);

const auto SCROLL_FIRE_RESIST = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of fire resistance", ItemType::SCROLL, Spells::FIRE_RESIST);
const auto SCROLL_FIRE_IMMUNE = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of fire immunity", ItemType::SCROLL, Spells::FIRE_IMMUNE);
const auto SCROLL_ACID_RESIST = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of acid resistance", ItemType::SCROLL, Spells::ACID_RESIST);
const auto SCROLL_ACID_IMMUNE = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of acid immunity", ItemType::SCROLL, Spells::ACID_IMMUNE);
}

#endif // __SCROLLS_H_
