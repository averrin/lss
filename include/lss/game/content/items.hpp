#ifndef __ITEMS_H_
#define __ITEMS_H_
#include "lss/game/randomTools.hpp"
#include "lss/game/content/spells.hpp"
#include "lss/game/content/specs.hpp"
#include "lss/game/content/traits.hpp"
#include "lss/utils.hpp"

namespace Prototype {
const auto TORCH = std::make_shared<Item>(
    ItemType::TORCH, Effects{std::make_shared<VisibilityModifier>(2.5f)});
const auto LANTERN = std::make_shared<Item>(
    ItemType::LANTERN, Effects{std::make_shared<VisibilityModifier>(1.5f)});
const auto PLATE = std::make_shared<Item>(
    ItemType::PLATE, Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto GOD_PLATE = std::make_shared<Item>(
    "GoD pLaTe", ItemType::PLATE,
    Effects{
        std::make_shared<TraitEffect>(Traits::INVULNERABLE),
        std::make_shared<IntelligenceModifier>(1.f),
        std::make_shared<StrengthModifier>(1.f),
    });
const auto BASIC_LEATHER_ARMOR = std::make_shared<Item>(
    ItemType::LEATHER_ARMOR, Effects{std::make_shared<ArmorValue>(1)});
const auto LEATHER_ARMOR = std::make_shared<Item>(
    ItemType::LEATHER_ARMOR, Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto HELMET = std::make_shared<Item>(
    ItemType::HELMET, Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto SHIELD = std::make_shared<Item>(
    ItemType::SHIELD, Effects{std::make_shared<ArmorValue>(R::I(2, 3))});
const auto GREAVES = std::make_shared<Item>(
    ItemType::GREAVES, Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto BOOTS = std::make_shared<Item>(
    ItemType::BOOTS, Effects{std::make_shared<ArmorValue>(R::I(1, 3))});

const auto BOOTS_OF_FLY =
    std::make_shared<Item>("flying boots", ItemType::BOOTS,
                           Effects{std::make_shared<TraitEffect>(Traits::FLY)});

const auto GRASS = std::make_shared<Item>("just herb", ItemType::GRASS, 1);

const auto SPEED_RING =
    std::make_shared<Item>("ring of lightning", ItemType::GOLD_RING,
                           Effects{std::make_shared<SpeedModifier>(0.3)});

const auto BASIC_SWORD = std::make_shared<Item>(
    ItemType::SWORD, Effects{std::make_shared<MeleeDamage>(
                         DamageSpec(0, 3, 3, DamageType::WEAPON))});
const auto SWORD = std::make_shared<Item>(
    ItemType::SWORD,
    Effects{std::make_shared<MeleeDamage>(
        DamageSpec(R::I(-2, 2), R::I(2, 4), R::I(2, 4), DamageType::WEAPON))});
const auto ORK_SWORD = std::make_shared<Item>(
    ItemType::SWORD, Effects{std::make_shared<MeleeDamage>(
                                 DamageSpec(5, 4, 6, DamageType::WEAPON)),
                             std::make_shared<CritModifier>(0.4)});
const auto GREAT_AXE = std::make_shared<Item>(
    ItemType::GREAT_AXE, Effects{std::make_shared<MeleeDamage>(
                                     DamageSpec(-1, 6, 7, DamageType::WEAPON)),
                                 std::make_shared<SpeedModifier>(-0.3f)});
const auto DAGGER = std::make_shared<Item>(
    ItemType::DAGGER, Effects{std::make_shared<MeleeDamage>(DamageSpec(
                          R::I(-2, 2), 1, R::I(1, 3), DamageType::WEAPON))});
const auto GOBLIN_DAGGER = std::make_shared<Item>(
    ItemType::DAGGER, Effects{std::make_shared<MeleeDamage>(
                                  DamageSpec(2, 3, 3, DamageType::WEAPON)),
                              std::make_shared<CritModifier>(0.2)});
const auto POISON_DAGGER = std::make_shared<Item>(
    ItemType::DAGGER,
    Effects{
        std::make_shared<MeleeDamage>(DamageSpec(2, 3, 3, DamageType::WEAPON)),
        std::make_shared<CritModifier>(0.2),
        std::make_shared<OnHitEffect>(
            std::make_shared<LastingEffect>(OverTimeEffects::POISON, 5000),
            0.3)});

const auto POISON_FANG = std::make_shared<Item>(
    ItemType::ENEMY,
    Effects{
        std::make_shared<MeleeDamage>(DamageSpec(2, 3, 3, DamageType::WEAPON)),
        std::make_shared<CritModifier>(0.2),
        std::make_shared<OnHitEffect>(
            std::make_shared<LastingEffect>(OverTimeEffects::POISON, 5000),
            0.3)});

const auto SWORD_OF_DUALWIELD = std::make_shared<Item>(
    "sword of dualwield", ItemType::SWORD,
    Effects{std::make_shared<MeleeDamage>(DamageSpec(
                R::I(-2, 2), R::I(2, 4), R::I(2, 4), DamageType::WEAPON)),
            std::make_shared<TraitEffect>(Traits::DUAL_WIELD)});

// TODO: make colors unique
const auto POTION_HEAL_LESSER = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "lesser heal potion",
    ItemType::POTION, Spells::HEAL_LESSER);
const auto POTION_HEAL = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "heal potion",
    ItemType::POTION, Spells::HEAL);

const auto POTION_MANA = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "restore mana potion",
    ItemType::POTION, Spells::RESTORE_MANA);

const auto POTION_GOD_SPEED = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "god speed potion",
    ItemType::POTION, Spells::GOD_SPEED);

const auto POTION_HP_BOOST = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "health boost potion",
    ItemType::POTION, Spells::HP_BOOST);

const auto POTION_VISIBILITY_BOOST = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()),
    "visibility boost potion", ItemType::POTION, Spells::VISIBILITY_BOOST);

const auto POTION_CRIT_BOOST = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()),
    "crit chance boost potion", ItemType::POTION, Spells::CRIT_BOOST);

const auto POTION_LEVITATION = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "levitation potion",
    ItemType::POTION, Spells::LEVITATION);

const auto POTION_REGENERATION = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "regeneration potion",
    ItemType::POTION, Spells::EOT_HEAL);

const auto POTION_POISON = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "poison",
    ItemType::POTION, Spells::EOT_POISON);

const auto POTION_CONFUSION = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "potion of confusion",
    ItemType::POTION, Spells::CONFUSION);

const auto POTION_INTELLIGENCE_BOOST = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()),
    "intelligence boost potion", ItemType::POTION, Spells::INTELLIGENCE_BOOST);
const auto POTION_STRENGTH_BOOST = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "strength boost potion",
    ItemType::POTION, Spells::STRENGTH_BOOST);

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

const auto GOLD = std::make_shared<Item>(ItemType::GOLD_COINS, 1);

const auto THROWING_KNIVES =
    std::make_shared<Item>(ItemType::THROWING_KNIVES, 1,
                           Effects{std::make_shared<MeleeDamage>(
                               DamageSpec(0, 1, 3, DamageType::WEAPON))});

const std::vector<std::shared_ptr<Item>> ALL = {
    Prototype::TORCH,
    Prototype::LANTERN,
    Prototype::PLATE,
    Prototype::GOD_PLATE,
    Prototype::BASIC_LEATHER_ARMOR,
    Prototype::LEATHER_ARMOR,
    Prototype::HELMET,
    Prototype::SHIELD,
    Prototype::GREAVES,
    Prototype::BOOTS,
    Prototype::BOOTS_OF_FLY,
    Prototype::GRASS,
    Prototype::SPEED_RING,
    Prototype::BASIC_SWORD,
    Prototype::SWORD,
    Prototype::ORK_SWORD,
    Prototype::GREAT_AXE,
    Prototype::DAGGER,
    Prototype::GOBLIN_DAGGER,
    Prototype::POISON_DAGGER,
    // Prototype::POISON_FANG,
    Prototype::SWORD_OF_DUALWIELD,
    Prototype::THROWING_KNIVES,

    Prototype::POTION_HEAL_LESSER,
    Prototype::POTION_HEAL,
    Prototype::POTION_MANA,
    Prototype::POTION_GOD_SPEED,
    Prototype::POTION_HP_BOOST,
    Prototype::POTION_VISIBILITY_BOOST,
    Prototype::POTION_INTELLIGENCE_BOOST,
    Prototype::POTION_STRENGTH_BOOST,
    Prototype::POTION_CRIT_BOOST,
    Prototype::POTION_LEVITATION,
    Prototype::POTION_REGENERATION,
    Prototype::POTION_POISON,
    Prototype::POTION_CONFUSION,

    Prototype::SCROLL_IDENTIFICATION,
    Prototype::SCROLL_REVEAL,
    Prototype::SCROLL_TELEPORT,

    Prototype::SCROLL_WEAPON_IMMUNE,
    Prototype::SCROLL_WEAPON_RESIST,
    Prototype::SCROLL_MAGIC_IMMUNE,
    Prototype::SCROLL_MAGIC_RESIST,
    Prototype::SCROLL_FIRE_IMMUNE,
    Prototype::SCROLL_FIRE_RESIST,
    Prototype::SCROLL_ACID_IMMUNE,
    Prototype::SCROLL_ACID_RESIST,
};
} // namespace Prototype


#endif // __ITEMS_H_
