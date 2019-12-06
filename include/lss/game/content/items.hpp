#ifndef __ITEMS_H_
#define __ITEMS_H_
#include "lss/game/content/specs.hpp"
#include "lss/game/content/spells.hpp"
#include "lss/game/content/traits.hpp"
#include "lss/game/item.hpp"
#include "lss/game/randomTools.hpp"
#include "lss/utils.hpp"

#include "lss/game/content/armor.hpp"
#include "lss/game/content/artifacts.hpp"
#include "lss/game/content/potions.hpp"
#include "lss/game/content/scrolls.hpp"
#include "lss/game/content/weapons.hpp"

namespace Prototype {
const auto GOLD = std::make_shared<Item>(ItemType::GOLD_COINS, 1);

/* LIGHT */
const auto TORCH = std::make_shared<Item>(
    ItemType::TORCH, Effects{std::make_shared<VisibilityModifier>(
                         LightSpec{4.5f, LightType::FIRE})});
const auto CANDLE = std::make_shared<Item>(
    ItemType::CANDLE, Effects{std::make_shared<VisibilityModifier>(
                         LightSpec{2.5f, LightType::FIRE})});
const auto LANTERN = std::make_shared<Item>(
    ItemType::LANTERN, Effects{std::make_shared<VisibilityModifier>(
                           LightSpec{3.2f, LightType::FIRE})});
const auto MAGIC_TORCH =
    std::make_shared<Item>("magic torch", ItemType::TORCH,
                           Effects{std::make_shared<VisibilityModifier>(
                               LightSpec{4.5f, LightType::MAGIC, true})});

/* DEBUG */
const auto GOD_PLATE = std::make_shared<Item>(
    "GoD pLaTe", ItemType::PLATE,
    Effects{
        std::make_shared<TraitEffect>(Traits::INVULNERABLE),
        std::make_shared<IntelligenceModifier>(1.f),
        std::make_shared<StrengthModifier>(1.f),
    });

/* Starting items */
const auto BASIC_LEATHER_CUIRASS =
    std::make_shared<Item>("leather cuirass", ItemType::CUIRASS,
                           Effects{std::make_shared<ArmorValue>(1)});
const auto BASIC_SWORD = std::make_shared<Item>(
    ItemType::ONE_HAND, Effects{std::make_shared<MeleeDamage>(
                            DamageSpec(0, 3, 3, DamageType::WEAPON))});

/* Misc */
const auto GRASS = std::make_shared<Item>("just herb", ItemType::GRASS, 1);
const auto ROCK =
    std::make_shared<Item>(ItemType::ROCK, 1,
                           Effects{std::make_shared<MeleeDamage>(
                               DamageSpec(1, 0, 0, DamageType::WEAPON))});
const auto PICK_AXE = std::make_shared<Item>(ItemType::PICK_AXE);

const auto QUEST_ITEM = std::make_shared<Usable>(ItemType::EMPTY);

const auto BROOM = std::make_shared<Item>("broom", ItemType::CLUTTER, 1);
const auto FORK = std::make_shared<Item>("fork", ItemType::CLUTTER, 1);
const auto SPOON = std::make_shared<Item>("spoon", ItemType::CLUTTER, 1);
const auto PLATE = std::make_shared<Item>("plate", ItemType::CLUTTER, 1);
const auto CLOTH = std::make_shared<Item>("cloth", ItemType::CLUTTER, 1);
const auto BASKET = std::make_shared<Item>("basket", ItemType::CLUTTER, 1);
const auto BELLOWS = std::make_shared<Item>("bellows", ItemType::CLUTTER, 1);
const auto BUCKET = std::make_shared<Item>("bucket", ItemType::CLUTTER, 1);
const auto DRUM = std::make_shared<Item>("drum", ItemType::CLUTTER, 1);
const auto INKWELL = std::make_shared<Item>("inkwell", ItemType::CLUTTER, 1);
const auto LUTE = std::make_shared<Item>("lute", ItemType::CLUTTER, 1);
const auto SHOVEL = std::make_shared<Item>("shovel", ItemType::CLUTTER, 1);
const auto PILLOW = std::make_shared<Item>("pillow", ItemType::CLUTTER, 1);
const auto SKULL = std::make_shared<Item>("skull", ItemType::CLUTTER, 1);

/* Kits */
const std::vector<std::shared_ptr<Item>> CLUTTER = {
    BROOM,
    FORK,
    SPOON,
    PLATE,
    CLOTH,
    BASKET,
    BELLOWS,
    BUCKET,
    DRUM,
    INKWELL,
    LUTE,
    SHOVEL,
    PILLOW,
    SKULL,
};

const std::vector<std::shared_ptr<Item>> LOOT_0 = {
    DAGGER,
    THROWING_KNIVES->setCount(5),
    LEATHER_CUIRASS,
    LEATHER_HELMET,
};

const std::vector<std::shared_ptr<Item>> LOOT_1 = {
    DAGGER,
    SWORD,
    RAPIER,
    AXE,
    THROWING_KNIVES->setCount(10),
    LEATHER_CUIRASS,
    LEATHER_HELMET,
    LEATHER_SHIELD,
    LEATHER_GREAVES,
    LEATHER_BOOTS,
    LEATHER_RIGHT_PAULDRON,
    LEATHER_LEFT_PAULDRON,
    LEATHER_RIGHT_GAUNTLET,
    LEATHER_LEFT_GAUNTLET,
};

const std::vector<std::shared_ptr<Item>> LOOT_2 = {
    LANTERN,
    STEEL_SWORD,
    STEEL_AXE,
    RAPIER,
    IRON_CUIRASS,
    IRON_HELMET,
    IRON_SHIELD,
    IRON_GREAVES,
    IRON_BOOTS,
    IRON_RIGHT_PAULDRON,
    IRON_LEFT_PAULDRON,
    IRON_RIGHT_GAUNTLET,
    IRON_LEFT_GAUNTLET,
};

const std::vector<std::shared_ptr<Item>> LOOT_3 = {
    LANTERN,
    THROWING_AXES->setCount(5),
    GREAT_AXE,
    GREAT_SWORD,
    STEEL_CUIRASS,
    STEEL_HELMET,
    STEEL_SHIELD,
    STEEL_GREAVES,
    STEEL_BOOTS,
    STEEL_RIGHT_PAULDRON,
    STEEL_LEFT_PAULDRON,
    STEEL_RIGHT_GAUNTLET,
    STEEL_LEFT_GAUNTLET,
};

const std::vector<std::shared_ptr<Item>> WEAPONS_1 = {
    SWORD, DAGGER, THROWING_KNIVES->setCount(5), RAPIER, AXE,
};

const std::vector<std::shared_ptr<Item>> WEAPONS_2 = {
    STEEL_SWORD,
    STEEL_AXE,
    RAPIER,
};

const std::vector<std::shared_ptr<Item>> WEAPONS_3 = {
    THROWING_AXES->setCount(5),
    GREAT_AXE,
    GREAT_SWORD,
};

const std::vector<std::shared_ptr<Item>> LIGHT = {
    TORCH,
    CANDLE,
    LANTERN,
    MAGIC_TORCH,
};

const std::vector<std::shared_ptr<Item>> ARTIFACTS_0 = {
    SPEED_RING,
    BOOTS_OF_FLY,
    PICK_AXE,
    BAND_OF_DARKVISION,
};
const std::vector<std::shared_ptr<Item>> ARTIFACTS_1 = {
    ROBE_OF_INTELLIGENCE,
    LEATHER_CUIRASS_OF_HP,
    LEATHER_LEFT_GAUNTLET_OF_STRENGTH,
    SWORD_OF_DUALWIELD,
    LEFT_GAUNTLET_OF_DUEL,
    POISON_DAGGER,
    AMULET_OF_BERSERK};

const std::vector<std::shared_ptr<Item>> ARTIFACTS_2 = {
    AMULET_OF_ACID_IMMUNE, AMULET_OF_FIRE_IMMUNE, AMULET_OF_FROST_IMMUNE,
    RING_OF_ACID_RESIST,   RING_OF_FROST_RESIST,  RING_OF_FIRE_RESIST,
    RING_OF_REGEN,         RING_OF_MANA_RESTORE,
};

const std::vector<std::shared_ptr<Item>> ARTIFACTS_3 = {};

const std::vector<std::shared_ptr<Item>> ENEMY = {
    ORK_SWORD,
    GOBLIN_DAGGER,
    POISON_FANG,
};

const std::vector<std::shared_ptr<Item>> DEBUG = {
    GOD_PLATE,
};

const std::vector<std::shared_ptr<Item>> MISC = {
    GRASS,
    ROCK,
};

const std::vector<std::shared_ptr<Item>> BASIC_ITEMS = {
    BASIC_LEATHER_CUIRASS,
    BASIC_SWORD,
};

const std::vector<std::shared_ptr<Item>> LEATHER_ARMOR = {
    LEATHER_CUIRASS,       LEATHER_HELMET,         LEATHER_SHIELD,
    LEATHER_GREAVES,       LEATHER_BOOTS,          LEATHER_RIGHT_PAULDRON,
    LEATHER_LEFT_PAULDRON, LEATHER_RIGHT_GAUNTLET, LEATHER_LEFT_GAUNTLET,
};

const std::vector<std::shared_ptr<Item>> IRON_ARMOR = {
    IRON_CUIRASS,       IRON_HELMET,         IRON_SHIELD,
    IRON_GREAVES,       IRON_BOOTS,          IRON_RIGHT_PAULDRON,
    IRON_LEFT_PAULDRON, IRON_RIGHT_GAUNTLET, IRON_LEFT_GAUNTLET,
};

const std::vector<std::shared_ptr<Item>> STEEL_ARMOR = {
    STEEL_CUIRASS,       STEEL_HELMET,         STEEL_SHIELD,
    STEEL_GREAVES,       STEEL_BOOTS,          STEEL_RIGHT_PAULDRON,
    STEEL_LEFT_PAULDRON, STEEL_RIGHT_GAUNTLET, STEEL_LEFT_GAUNTLET,
};

const std::vector<std::shared_ptr<Item>> SCROLLS = {
    SCROLL_IDENTIFICATION, SCROLL_REVEAL,        SCROLL_TELEPORT,

    SCROLL_WEAPON_IMMUNE,  SCROLL_WEAPON_RESIST, SCROLL_MAGIC_IMMUNE,
    SCROLL_MAGIC_RESIST,   SCROLL_FIRE_IMMUNE,   SCROLL_FIRE_RESIST,
    SCROLL_ACID_IMMUNE,    SCROLL_ACID_RESIST,
};

const std::vector<std::shared_ptr<Item>> POTIONS = {
    POTION_HEAL_LESSER,
    POTION_HEAL,
    POTION_MANA,
    POTION_GOD_SPEED,
    POTION_HP_BOOST,
    POTION_VISIBILITY_BOOST,
    POTION_INTELLIGENCE_BOOST,
    POTION_STRENGTH_BOOST,
    POTION_CRIT_BOOST,
    POTION_LEVITATION,
    POTION_REGENERATION,
    POTION_POISON,
    POTION_CONFUSION,
    POTION_INVULNERABILITY,
};

const std::vector<std::vector<std::shared_ptr<Item>>> ALL = {
    DEBUG,   ARTIFACTS_0, ARTIFACTS_1,   ARTIFACTS_2, POTIONS,
    SCROLLS, IRON_ARMOR,  STEEL_ARMOR,   WEAPONS_1,   WEAPONS_2,
    MISC,    LIGHT,       LEATHER_ARMOR, BASIC_ITEMS,
};

} // namespace Prototype

#endif // __ITEMS_H_
