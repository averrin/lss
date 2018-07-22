#ifndef __SPELL_H_
#define __SPELL_H_
#include "lss/game/effect.hpp"
#include "lss/game/object.hpp"

class Spell : public Object {
public:
  Spell(std::string n, int c = 0, int a = 0) : Object(), name(n), cost(c), ap(a) {}
  std::string name;
  int cost;
  int ap;
  friend bool operator!=(Spell &lhs, const Spell &rhs) {
    return lhs.name != rhs.name;
  }
  friend bool operator==(Spell &lhs, const Spell &rhs) {
    return lhs.name == rhs.name;
  }
};

class ToggleTraitSpell : public Spell {
public:
  ToggleTraitSpell(std::string n, Trait t) : Spell(n), trait(t) {}
  ToggleTraitSpell(std::string n, Trait t, float p) : Spell(n), trait(t), probability(p) {}
  float probability = 1;
  Trait trait;
};

class EffectSpell : public Spell {
public:
  EffectSpell(std::string n, std::shared_ptr<Effect> e) : Spell(n), effect(e) {}
  std::shared_ptr<Effect> effect;
};

namespace Spells {
const auto REVEAL = std::make_shared<Spell>("Reveal", 25);
const auto MONSTER_SENSE = std::make_shared<Spell>("Monster Sense", 25);
// const auto MONSTER_FREEZE = std::make_shared<Spell>("Monster Freeze");
const auto SUMMON_ORK = std::make_shared<Spell>("Summon Ork");
const auto SUMMON_PLATE = std::make_shared<Spell>("Summon thing");

const auto IDENTIFY = std::make_shared<Spell>("Identify", 30);
const auto HEAL_LESSER = std::make_shared<Spell>("Lesser heal", 5);
const auto HEAL = std::make_shared<Spell>("Heal", 25);
const auto HEAL_GREATER = std::make_shared<Spell>("Greater Heal", 45);
const auto TELEPORT_RANDOM = std::make_shared<Spell>("Teleport", 20);

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
const auto TOGGLE_JUMPY = std::make_shared<ToggleTraitSpell>(
    "Toggle Jumpy trait", Traits::JUMPY);

  /* for consumables */
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

const auto LEVITATION = std::make_shared<EffectSpell>(
    "Levitation", std::make_shared<LastingEffect>(
                      std::make_shared<TraitEffect>(Traits::FLY), 10000));

const auto CONFUSION = std::make_shared<EffectSpell>(
    "Confusion", std::make_shared<LastingEffect>(
                      std::make_shared<TraitEffect>(Traits::CONFUSED), 50000));

const auto EOT_HEAL = std::make_shared<EffectSpell>(
    "Regeneration",
    std::make_shared<LastingEffect>(OverTimeEffects::HEAL, 5000));

const auto EOT_POISON = std::make_shared<EffectSpell>(
    "Poison", std::make_shared<LastingEffect>(OverTimeEffects::POISON, 5000));

const std::vector<std::shared_ptr<Spell>> USABLE = {
            Spells::REVEAL, Spells::MONSTER_SENSE, /*Spells::MONSTER_FREEZE,*/
            Spells::HEAL_LESSER, Spells::HEAL, Spells::HEAL_GREATER,
            Spells::IDENTIFY, Spells::TELEPORT_RANDOM,

            Spells::SUMMON_PLATE,

            Spells::TOGGLE_DUAL_WIELD, Spells::TOGGLE_NIGHT_VISION,
            Spells::TOGGLE_MIND_SIGHT, Spells::TOGGLE_MAGIC_TORCH,
            Spells::TOGGLE_FLY, Spells::TOGGLE_CAN_SWIM, Spells::SUMMON_ORK,
            Spells::TOGGLE_INVULNERABLE,  Spells::TOGGLE_JUMPY}; 
} // namespace Spells



#endif // __SPELL_H_
