#ifndef __SPELL_H_
#define __SPELL_H_
#include "lss/game/effect.hpp"
#include "lss/game/object.hpp"

class Spell : public Object {
public:
  Spell(std::string n, int c = 0) : Object(), name(n), cost(c) {}
  std::string name;
  int cost;
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
  Trait trait;
};

class EffectSpell : public Spell {
public:
  EffectSpell(std::string n, std::shared_ptr<Effect> e) : Spell(n), effect(e) {}
  std::shared_ptr<Effect> effect;
};

namespace Spells {
const auto REVEAL = std::make_shared<Spell>("Reveal");
const auto MONSTER_SENSE = std::make_shared<Spell>("Monster Sense");
const auto MONSTER_FREEZE = std::make_shared<Spell>("Monster Freeze");
const auto SUMMON_ORK = std::make_shared<Spell>("Summon Ork");
const auto SUMMON_PLATE = std::make_shared<Spell>("Summon Plate");
const auto IDENTIFY = std::make_shared<Spell>("Identify");
const auto HEAL_LESSER = std::make_shared<Spell>("Lesser heal");
const auto HEAL = std::make_shared<Spell>("Heal");
const auto HEAL_GREATER = std::make_shared<Spell>("Greater");
const auto TELEPORT_RANDOM = std::make_shared<Spell>("Teleport");

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
} // namespace Spells

#endif // __SPELL_H_
