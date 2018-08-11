#ifndef __SPELLS_H_
#define __SPELLS_H_
#include "lss/game/content/lastingSpells.hpp"
#include "lss/game/content/traits.hpp"
#include "lss/game/spell.hpp"

namespace Spells {
const auto REVEAL = std::make_shared<Spell>("Reveal", 2, 50, 2000);
const auto MONSTER_SENSE = std::make_shared<Spell>("Monster Sense");
// const auto MONSTER_FREEZE = std::make_shared<Spell>("Monster Freeze");
const auto SUMMON_THING = std::make_shared<Spell>("Summon thing");
const auto IDENTIFY = std::make_shared<Spell>("Identify", 2, 30, 500);
const auto TELEPORT_RANDOM = std::make_shared<Spell>("Teleport", 2, 20);

/* HEAL */
const auto HEAL_LESSER = std::make_shared<Spell>("Lesser heal", 0, 15, 1000);
const auto HEAL = std::make_shared<Spell>("Heal", 1, 25, 2000);
const auto HEAL_GREATER = std::make_shared<Spell>("Greater Heal", 3, 45, 5000);

/* COMBAT */
const auto FIREBALL = std::make_shared<RadiusSpell>(
    "Fireball", 1, std::make_shared<DamageSpell>(
                       "Fire damage", DamageSpec(0, 2, 6, DamageType::FIRE),
                       TerrainType::FIREBALL),
    1.5, 20);
const auto FIREBLAST = std::make_shared<RadiusSpell>(
    "Fireblast", 3, std::make_shared<DamageSpell>(
                        "Fire damage", DamageSpec(0, 2, 6, DamageType::FIRE),
                        TerrainType::FIREBALL),
    3.5, 40);

const auto FIRESTREAM = std::make_shared<LineSpell>(
    "Firestream", 2, std::make_shared<DamageSpell>(
                         "Fire damage", DamageSpec(0, 2, 6, DamageType::FIRE),
                         TerrainType::FIREBALL),
    4, 20);

const auto FIRESTRIKE = std::make_shared<TargetSpell>(
    "Firestrike", 0, std::make_shared<DamageSpell>(
                         "Fire damage", DamageSpec(0, 2, 6, DamageType::FIRE),
                         TerrainType::FIREBALL),
    4, 15);

const auto ACIDSTRIKE = std::make_shared<TargetSpell>(
    "Acidstrike", 1, std::make_shared<DamageSpell>(
                         "Acid damage", DamageSpec(2, 3, 6, DamageType::ACID),
                         TerrainType::FIREBALL),
    4, 20);

/* LIGHT */
const auto FLASH = std::make_shared<RadiusSpell>(
    "Flash", 0, std::make_shared<CellSpell>("Light", TerrainType::MAGIC_LIGHT),
    2.5, 10);
const auto LIGHT = std::make_shared<TargetSpell>(
    "Light", 1,
    std::make_shared<CellSpell>("Light", TerrainType::MAGIC_LIGHT_LONG), 6, 10);
const auto LIGHT_FOREVER = std::make_shared<TargetSpell>(
    "Eternal Light", 2,
    std::make_shared<CellSpell>("Light", TerrainType::MAGIC_LIGHT_FOREVER), 6,
    10);

/* DEBUG */
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
const auto RESTORE_MANA = std::make_shared<Spell>("Greater mana");

const auto ACID_LIGHT_FOREVER = std::make_shared<TargetSpell>(
    "Eternal green Light", 0,
    std::make_shared<CellSpell>("Light", TerrainType::ACID_LIGHT_FOREVER), 6,
    0);

const std::vector<std::shared_ptr<Spell>> USABLE = {
    REVEAL,
    MONSTER_SENSE, /*Spells::MONSTER_FREEZE,*/
    HEAL_LESSER,
    HEAL,
    HEAL_GREATER,
    IDENTIFY,
    TELEPORT_RANDOM,
    FIREBALL,
    FIREBLAST,
    FIRESTREAM,
    FIRESTRIKE,

    FLASH,
    LIGHT,
    LIGHT_FOREVER,
    ACID_LIGHT_FOREVER,

    SUMMON_THING,
    RESTORE_MANA,

    TOGGLE_DUAL_WIELD,
    TOGGLE_NIGHT_VISION,
    TOGGLE_MIND_SIGHT,
    TOGGLE_MAGIC_TORCH,
    TOGGLE_FLY,
    TOGGLE_CAN_SWIM,
    TOGGLE_INVULNERABLE,
    TOGGLE_JUMPY,
};
} // namespace Spells

#endif // __SPELLS_H_
