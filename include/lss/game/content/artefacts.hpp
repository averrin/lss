#ifndef __ARTEFACTS_H_
#define __ARTEFACTS_H_

#include "lss/game/content/specs.hpp"
#include "lss/game/content/spells.hpp"
#include "lss/game/item.hpp"

namespace Prototype {
const auto ROBE_OF_INTELLIGENCE = std::make_shared<Item>(
    "blue robe", "robe of intelligence", ItemType::CLOAK,
    Effects{std::make_shared<IntelligenceModifier>(0.1)});

const auto AMULET_OF_BERSERK = std::make_shared<Item>(
    "ruby neckless", "amulet of berserk", ItemType::AMULET,
    Effects{std::make_shared<TraitEffect>(Traits::BLOOD_THIRST)});

const auto LEATHER_CUIRASS_OF_HP = std::make_shared<Item>(
    "damaged leather cuirass", "leather cuirass of health", ItemType::CUIRASS,
    Effects{std::make_shared<ArmorValue>(R::I(1, 2)),
            std::make_shared<HPModifier>(25)});

const auto LEATHER_LEFT_GAUNTLET_OF_STRENGTH = std::make_shared<Item>(
    "worn leather left gauntlet", "left gauntlet of strength",
    ItemType::LEFT_GAUNTLET, Effects{std::make_shared<ArmorValue>(R::I(1, 2)),
                                     std::make_shared<StrengthModifier>(0.1)});

const auto LEFT_GAUNTLET_OF_DUEL = std::make_shared<Item>(
    "black left gauntlet", "duelist's left gauntlet", ItemType::LEFT_GAUNTLET,
    Effects{
        std::make_shared<TraitEffect>(Traits::DUAL_WIELD),
        std::make_shared<CritModifier>(0.2),
    });

const auto BOOTS_OF_FLY =
    std::make_shared<Item>("flying boots", ItemType::BOOTS,
                           Effects{std::make_shared<TraitEffect>(Traits::FLY)});

const auto SPEED_RING =
    std::make_shared<Item>("gold ring", "ring of lightning", ItemType::RING,
                           Effects{std::make_shared<SpeedModifier>(0.1)});

const auto SWORD_OF_DUALWIELD = std::make_shared<Item>(
    "sword of dualwield", ItemType::ONE_HAND,
    Effects{std::make_shared<MeleeDamage>(DamageSpec(
                R::I(-2, 2), R::I(2, 4), R::I(2, 4), DamageType::WEAPON)),
            std::make_shared<TraitEffect>(Traits::DUAL_WIELD)});

const auto POISON_DAGGER = std::make_shared<Item>(
    ItemType::LIGHT_WEAPON,
    Effects{
        std::make_shared<MeleeDamage>(DamageSpec(2, 3, 3, DamageType::WEAPON)),
        std::make_shared<CritModifier>(0.2),
        std::make_shared<OnHitEffect>(
            std::make_shared<LastingEffect>(OverTimeEffects::POISON, 5000),
            0.3)});
}

#endif // __ARTEFACTS_H_
