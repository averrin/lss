#ifndef __LASTINGSPELLS_H_
#define __LASTINGSPELLS_H_

#include "lss/game/spell.hpp"
#include "lss/game/content/traits.hpp"

namespace Spells {
const auto GOD_SPEED = std::make_shared<EffectSpell>(
    "Make you faster than Flash",
    std::make_shared<LastingEffect>(std::make_shared<SpeedModifier>(2), 10000));

// TODO: link this with current HP
const auto HP_BOOST = std::make_shared<EffectSpell>(
    "Boost your health",
    std::make_shared<LastingEffect>(std::make_shared<HPModifier>(50), 10000));

const auto VISIBILITY_BOOST = std::make_shared<EffectSpell>(
    "Boost your eyes", std::make_shared<LastingEffect>(
                           std::make_shared<VisibilityModifier>(5), 10000));

const auto CRIT_BOOST = std::make_shared<EffectSpell>(
    "Boost your crit chance", std::make_shared<LastingEffect>(
                                  std::make_shared<CritModifier>(0.3), 10000));

const auto INTELLIGENCE_BOOST = std::make_shared<EffectSpell>(
    "Boost your brains",
    std::make_shared<LastingEffect>(std::make_shared<IntelligenceModifier>(0.1),
                                    10000));
const auto STRENGTH_BOOST = std::make_shared<EffectSpell>(
    "Boost your strength", std::make_shared<LastingEffect>(
                               std::make_shared<StrengthModifier>(0.1), 10000));

const auto LEVITATION = std::make_shared<EffectSpell>(
    "Levitation", std::make_shared<LastingEffect>(
                      std::make_shared<TraitEffect>(Traits::FLY), 10000));

const auto CONFUSION = std::make_shared<EffectSpell>(
    "Confusion", std::make_shared<LastingEffect>(
                     std::make_shared<TraitEffect>(Traits::CONFUSED), 50000));

const auto WEAPON_RESIST = std::make_shared<EffectSpell>(
    "Weapon resist",
    std::make_shared<LastingEffect>(
        std::make_shared<TraitEffect>(Traits::WEAPON_RESIST), 10000));
const auto WEAPON_IMMUNE = std::make_shared<EffectSpell>(
    "Weapon immune",
    std::make_shared<LastingEffect>(
        std::make_shared<TraitEffect>(Traits::WEAPON_IMMUNE), 10000));
const auto MAGIC_RESIST = std::make_shared<EffectSpell>(
    "Magic resist",
    std::make_shared<LastingEffect>(
        std::make_shared<TraitEffect>(Traits::MAGIC_RESIST), 10000));
const auto MAGIC_IMMUNE = std::make_shared<EffectSpell>(
    "Magic immune",
    std::make_shared<LastingEffect>(
        std::make_shared<TraitEffect>(Traits::MAGIC_IMMUNE), 10000));

const auto FIRE_RESIST = std::make_shared<EffectSpell>(
    "Fire resist",
    std::make_shared<LastingEffect>(
        std::make_shared<TraitEffect>(Traits::FIRE_RESIST), 10000));
const auto FIRE_IMMUNE = std::make_shared<EffectSpell>(
    "Fire immune",
    std::make_shared<LastingEffect>(
        std::make_shared<TraitEffect>(Traits::FIRE_IMMUNE), 10000));
const auto ACID_RESIST = std::make_shared<EffectSpell>(
    "Acid resist",
    std::make_shared<LastingEffect>(
        std::make_shared<TraitEffect>(Traits::ACID_RESIST), 10000));
const auto ACID_IMMUNE = std::make_shared<EffectSpell>(
    "Acid immune",
    std::make_shared<LastingEffect>(
        std::make_shared<TraitEffect>(Traits::ACID_IMMUNE), 10000));

/* EOT */
const auto EOT_HEAL = std::make_shared<EffectSpell>(
    "Regeneration",
    std::make_shared<LastingEffect>(OverTimeEffects::HEAL, 5000));

const auto EOT_POISON = std::make_shared<EffectSpell>(
    "Poison", std::make_shared<LastingEffect>(OverTimeEffects::POISON, 5000));
}


#endif // __LASTINGSPELLS_H_
