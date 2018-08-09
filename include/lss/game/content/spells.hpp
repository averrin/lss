#ifndef __SPELLS_H_
#define __SPELLS_H_
#include "lss/game/spell.hpp"
#include "lss/game/content/traits.hpp"

namespace Spells {
const auto REVEAL = std::make_shared<Spell>("Reveal", 2, 50, 2000);
const auto MONSTER_SENSE = std::make_shared<Spell>("Monster Sense");
// const auto MONSTER_FREEZE = std::make_shared<Spell>("Monster Freeze");
const auto SUMMON_THING = std::make_shared<Spell>("Summon thing");

const auto IDENTIFY = std::make_shared<Spell>("Identify", 2, 30, 500);
const auto HEAL_LESSER = std::make_shared<Spell>("Lesser heal", 0, 5, 1000);
const auto HEAL = std::make_shared<Spell>("Heal", 1, 25, 2000);
const auto HEAL_GREATER = std::make_shared<Spell>("Greater Heal", 3, 45, 5000);
const auto TELEPORT_RANDOM = std::make_shared<Spell>("Teleport", 2, 20);

const auto FIREBALL = std::make_shared<RadiusSpell>(
    "Fireball", 1,
    std::make_shared<DamageSpell>("Fire damage",
                                  DamageSpec(0, 2, 6, DamageType::FIRE),
                                  TerrainType::FIREBALL),
    1.5, 20);
const auto FIREBLAST = std::make_shared<RadiusSpell>(
    "Fireblast", 3,
    std::make_shared<DamageSpell>("Fire damage",
                                  DamageSpec(0, 2, 6, DamageType::FIRE),
                                  TerrainType::FIREBALL),
    3.5, 40);

const auto FIRESTREAM = std::make_shared<LineSpell>(
    "Firestream", 2,
    std::make_shared<DamageSpell>("Fire damage",
                                  DamageSpec(0, 2, 6, DamageType::FIRE),
                                  TerrainType::FIREBALL),
    4, 20);

const auto FIRESTRIKE = std::make_shared<TargetSpell>(
    "Firestrike", 0,
    std::make_shared<DamageSpell>("Fire damage",
                                  DamageSpec(0, 2, 6, DamageType::FIRE),
                                  TerrainType::FIREBALL),
    4, 10);

const auto ACIDSTRIKE = std::make_shared<TargetSpell>(
    "Acidstrike", 1,
    std::make_shared<DamageSpell>("Acid damage", DamageSpec(2, 3, 6, DamageType::ACID),
                                 TerrainType::FIREBALL), 4, 20);

const auto FLASH = std::make_shared<RadiusSpell>(
    "Flash", 0, std::make_shared<CellSpell>("Light", TerrainType::MAGIC_LIGHT),
    1.5, 10);
const auto LIGHT = std::make_shared<TargetSpell>(
    "Light", 1,
    std::make_shared<CellSpell>("Light", TerrainType::MAGIC_LIGHT_LONG), 6, 10);
const auto LIGHT_FOREVER = std::make_shared<TargetSpell>(
    "Eternal Light", 2,
    std::make_shared<CellSpell>("Light", TerrainType::MAGIC_LIGHT_FOREVER), 6,
    10);

const auto TOGGLE_DUAL_WIELD = std::make_shared<ToggleTraitSpell>(
    "Toggle Dual Wield trait", Traits::DUAL_WIELD);
const auto TOGGLE_NIGHT_VISION = std::make_shared<ToggleTraitSpell>(
    "Toggle Night Vision trait", Traits::NIGHT_VISION);
const auto TOGGLE_MIND_SIGHT = std::make_shared<ToggleTraitSpell>(
    "Toggle Mind Sight trait", Traits::MIND_SIGHT);
const auto TOGGLE_MAGIC_TORCH = std::make_shared<ToggleTraitSpell>(
    "Toggle Magic Torch trait", Traits::MAGIC_TORCH);
const auto TOGGLE_FLY =
    std::make_shared<ToggleTraitSpell>("Toggle Fly trait", Traits::FLY);
const auto TOGGLE_CAN_SWIM = std::make_shared<ToggleTraitSpell>(
    "Toggle Swimming trait", Traits::CAN_SWIM);
const auto TOGGLE_INVULNERABLE = std::make_shared<ToggleTraitSpell>(
    "Toggle Invulnerable trait", Traits::INVULNERABLE);
const auto TOGGLE_JUMPY =
    std::make_shared<ToggleTraitSpell>("Toggle Jumpy trait", Traits::JUMPY);

const auto ACID_LIGHT_FOREVER = std::make_shared<TargetSpell>(
    "Eternal green Light", 0,
    std::make_shared<CellSpell>("Light", TerrainType::ACID_LIGHT_FOREVER), 6, 0);

/* for consumables */
const auto RESTORE_MANA = std::make_shared<Spell>("Greater mana");

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

const auto EOT_HEAL = std::make_shared<EffectSpell>(
    "Regeneration",
    std::make_shared<LastingEffect>(OverTimeEffects::HEAL, 5000));

const auto EOT_POISON = std::make_shared<EffectSpell>(
    "Poison", std::make_shared<LastingEffect>(OverTimeEffects::POISON, 5000));

const std::vector<std::shared_ptr<Spell>> USABLE = {
    Spells::REVEAL,
    Spells::MONSTER_SENSE, /*Spells::MONSTER_FREEZE,*/
    Spells::HEAL_LESSER,
    Spells::HEAL,
    Spells::HEAL_GREATER,
    Spells::IDENTIFY,
    Spells::TELEPORT_RANDOM,
    Spells::FIREBALL,
    Spells::FIREBLAST,
    Spells::FIRESTREAM,
    Spells::FIRESTRIKE,

    Spells::FLASH,
    Spells::LIGHT,
    Spells::LIGHT_FOREVER,
    Spells::ACID_LIGHT_FOREVER,

    Spells::SUMMON_THING,
    Spells::RESTORE_MANA,

    Spells::TOGGLE_DUAL_WIELD,
    Spells::TOGGLE_NIGHT_VISION,
    Spells::TOGGLE_MIND_SIGHT,
    Spells::TOGGLE_MAGIC_TORCH,
    Spells::TOGGLE_FLY,
    Spells::TOGGLE_CAN_SWIM,
    Spells::TOGGLE_INVULNERABLE,
    Spells::TOGGLE_JUMPY};
} // namespace Spells



#endif // __SPELLS_H_
