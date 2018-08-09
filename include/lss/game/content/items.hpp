#ifndef __ITEMS_H_
#define __ITEMS_H_
#include "lss/utils.hpp"
#include "lss/game/item.hpp"
#include "lss/game/randomTools.hpp"
#include "lss/game/content/spells.hpp"
#include "lss/game/content/specs.hpp"
#include "lss/game/content/traits.hpp"

#include "lss/game/content/scrolls.hpp"
#include "lss/game/content/potions.hpp"

namespace Prototype {
/*LIGHT*/
const auto TORCH = std::make_shared<Item>(
    ItemType::TORCH, Effects{std::make_shared<VisibilityModifier>(2.5f)});
const auto LANTERN = std::make_shared<Item>(
    ItemType::LANTERN, Effects{std::make_shared<VisibilityModifier>(1.5f)});

/*DEBUG*/
const auto GOD_PLATE = std::make_shared<Item>(
    "GoD pLaTe", ItemType::PLATE,
    Effects{
        std::make_shared<TraitEffect>(Traits::INVULNERABLE),
        std::make_shared<IntelligenceModifier>(1.f),
        std::make_shared<StrengthModifier>(1.f),
    });

/*Starting items*/
const auto BASIC_LEATHER_CUIRASS = std::make_shared<Item>("leather cuirass",
    ItemType::CUIRASS, Effects{std::make_shared<ArmorValue>(1)});
const auto BASIC_SWORD = std::make_shared<Item>(
    ItemType::ONE_HAND, Effects{std::make_shared<MeleeDamage>(
                         DamageSpec(0, 3, 3, DamageType::WEAPON))});

/*Leather armor*/
const auto LEATHER_CUIRASS = std::make_shared<Item>("leather cuirass",
    ItemType::CUIRASS, Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto LEATHER_HELMET = std::make_shared<Item>("leather helm",
    ItemType::HELMET, Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto LEATHER_SHIELD = std::make_shared<Item>("leather shield",
    ItemType::SHIELD, Effects{std::make_shared<ArmorValue>(R::I(2, 3))});
const auto LEATHER_GREAVES = std::make_shared<Item>("leather greaves",
    ItemType::GREAVES, Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto LEATHER_BOOTS = std::make_shared<Item>("leather boots",
    ItemType::BOOTS, Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_RIGHT_PAULDRON = std::make_shared<Item>("leather right pauldron",
    ItemType::RIGHT_PAULDRON, Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_LEFT_PAULDRON = std::make_shared<Item>("leather left pauldron",
    ItemType::LEFT_PAULDRON, Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_RIGHT_GAUNTLET = std::make_shared<Item>("leather right gauntlet",
    ItemType::RIGHT_GAUNTLET, Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_LEFT_GAUNTLET = std::make_shared<Item>("leather left gauntlet",
    ItemType::LEFT_GAUNTLET, Effects{std::make_shared<ArmorValue>(R::I(1, 2))});


/*Iron armor*/
const auto IRON_CUIRASS = std::make_shared<Item>("iron cuirass",
    ItemType::CUIRASS, Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto IRON_HELMET = std::make_shared<Item>("iron helm",
    ItemType::HELMET, Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto IRON_SHIELD = std::make_shared<Item>("iron shield",
    ItemType::SHIELD, Effects{std::make_shared<ArmorValue>(R::I(4, 5))});
const auto IRON_GREAVES = std::make_shared<Item>("iron greaves",
    ItemType::GREAVES, Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto IRON_BOOTS = std::make_shared<Item>("iron boots",
    ItemType::BOOTS, Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_RIGHT_PAULDRON = std::make_shared<Item>("iron right pauldron",
    ItemType::RIGHT_PAULDRON, Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_LEFT_PAULDRON = std::make_shared<Item>("iron left pauldron",
    ItemType::LEFT_PAULDRON, Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_RIGHT_GAUNTLET = std::make_shared<Item>("iron right gauntlet",
    ItemType::RIGHT_GAUNTLET, Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_LEFT_GAUNTLET = std::make_shared<Item>("iron left gauntlet",
    ItemType::LEFT_GAUNTLET, Effects{std::make_shared<ArmorValue>(R::I(3, 4))});

/*Steel armor*/
const auto STEEL_CUIRASS = std::make_shared<Item>("steel cuirass",
    ItemType::CUIRASS, Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_HELMET = std::make_shared<Item>("steel helm",
    ItemType::HELMET, Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_SHIELD = std::make_shared<Item>("steel shield",
    ItemType::SHIELD, Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_GREAVES = std::make_shared<Item>("steel greaves",
    ItemType::GREAVES, Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_BOOTS = std::make_shared<Item>("steel boots",
    ItemType::BOOTS, Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_RIGHT_PAULDRON = std::make_shared<Item>("steel right pauldron",
    ItemType::RIGHT_PAULDRON, Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_LEFT_PAULDRON = std::make_shared<Item>("steel left pauldron",
    ItemType::LEFT_PAULDRON, Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_RIGHT_GAUNTLET = std::make_shared<Item>("steel right gauntlet",
    ItemType::RIGHT_GAUNTLET, Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_LEFT_GAUNTLET = std::make_shared<Item>("steel left gauntlet",
    ItemType::LEFT_GAUNTLET, Effects{std::make_shared<ArmorValue>(R::I(5, 6))});

const auto BOOTS_OF_FLY =
    std::make_shared<Item>("flying boots", ItemType::BOOTS,
                           Effects{std::make_shared<TraitEffect>(Traits::FLY)});

const auto GRASS = std::make_shared<Item>("just herb", ItemType::GRASS, 1);

const auto SPEED_RING =
    std::make_shared<Item>("ring of lightning", ItemType::RING,
                           Effects{std::make_shared<SpeedModifier>(0.3)});

const auto SWORD = std::make_shared<Item>(
    ItemType::ONE_HAND,
    Effects{std::make_shared<MeleeDamage>(
        DamageSpec(R::I(-2, 2), R::I(2, 4), R::I(2, 4), DamageType::WEAPON))});
const auto ORK_SWORD = std::make_shared<Item>(
    ItemType::ONE_HAND, Effects{std::make_shared<MeleeDamage>(
                                 DamageSpec(5, 4, 6, DamageType::WEAPON)),
                             std::make_shared<CritModifier>(0.4)});
const auto GREAT_AXE = std::make_shared<Item>(
    ItemType::TWO_HAND, Effects{std::make_shared<MeleeDamage>(
                                     DamageSpec(-1, 6, 7, DamageType::WEAPON)),
                                 std::make_shared<SpeedModifier>(-0.3f)});
const auto DAGGER = std::make_shared<Item>(
    ItemType::LIGHT_WEAPON, Effects{std::make_shared<MeleeDamage>(DamageSpec(
                          R::I(-2, 2), 1, R::I(1, 3), DamageType::WEAPON))});
const auto GOBLIN_DAGGER = std::make_shared<Item>(
    ItemType::LIGHT_WEAPON, Effects{std::make_shared<MeleeDamage>(
                                  DamageSpec(2, 3, 3, DamageType::WEAPON)),
                              std::make_shared<CritModifier>(0.2)});
const auto POISON_DAGGER = std::make_shared<Item>(
    ItemType::LIGHT_WEAPON,
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
    "sword of dualwield", ItemType::ONE_HAND,
    Effects{std::make_shared<MeleeDamage>(DamageSpec(
                R::I(-2, 2), R::I(2, 4), R::I(2, 4), DamageType::WEAPON)),
            std::make_shared<TraitEffect>(Traits::DUAL_WIELD)});


const auto GOLD = std::make_shared<Item>(ItemType::GOLD_COINS, 1);

const auto THROWING_KNIVES =
    std::make_shared<Item>(ItemType::THROWING_KNIVES, 1,
                           Effects{std::make_shared<MeleeDamage>(
                               DamageSpec(0, 1, 3, DamageType::WEAPON))});

const std::vector<std::shared_ptr<Item>> LOOT_0 = {
    DAGGER,
    THROWING_KNIVES,
    LEATHER_CUIRASS,
    LEATHER_HELMET,
};

const std::vector<std::shared_ptr<Item>> LOOT_1 = {
    DAGGER,
    THROWING_KNIVES,
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

const std::vector<std::shared_ptr<Item>> WEAPONS_1 = {
    SWORD,
    DAGGER,
    THROWING_KNIVES,
};

const std::vector<std::shared_ptr<Item>> WEAPONS_2 = {
    GREAT_AXE,
    POISON_DAGGER,
};

const std::vector<std::shared_ptr<Item>> LIGHT = {
    TORCH,
    LANTERN,
};

const std::vector<std::shared_ptr<Item>> ARTEFACTS_0 = {
    SPEED_RING,
    SWORD_OF_DUALWIELD,
    BOOTS_OF_FLY,
};
const std::vector<std::shared_ptr<Item>> ARTEFACTS_1 = {
    SPEED_RING,
    SWORD_OF_DUALWIELD,
    BOOTS_OF_FLY,
};

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
};

const std::vector<std::shared_ptr<Item>> BASIC_ITEMS = {
    BASIC_LEATHER_CUIRASS,
    BASIC_SWORD,
};

const std::vector<std::shared_ptr<Item>> LEATHER_ARMOR = {
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

const std::vector<std::shared_ptr<Item>> IRON_ARMOR = {
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

const std::vector<std::shared_ptr<Item>> STEEL_ARMOR = {
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

const std::vector<std::shared_ptr<Item>> SCROLLS = {
    SCROLL_IDENTIFICATION,
    SCROLL_REVEAL,
    SCROLL_TELEPORT,

    SCROLL_WEAPON_IMMUNE,
    SCROLL_WEAPON_RESIST,
    SCROLL_MAGIC_IMMUNE,
    SCROLL_MAGIC_RESIST,
    SCROLL_FIRE_IMMUNE,
    SCROLL_FIRE_RESIST,
    SCROLL_ACID_IMMUNE,
    SCROLL_ACID_RESIST,
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
};

const std::vector<std::vector<std::shared_ptr<Item>>> ALL = {
    IRON_ARMOR, STEEL_ARMOR,
    WEAPONS_1, WEAPONS_2,
    POTIONS, SCROLLS,
    MISC, DEBUG, LIGHT,
    LEATHER_ARMOR, BASIC_ITEMS,
    ARTEFACTS_0, ARTEFACTS_1,
};

} // namespace Prototype


#endif // __ITEMS_H_
