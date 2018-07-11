#ifndef __SPELL_H_
#define __SPELL_H_
#include "lss/game/object.hpp"

class Spell : public Object {
public:
  Spell(std::string n, int c = 0) : Object(), name(n), cost(c) {}
  std::string name;
  int cost;
};

class ToggleTraitSpell : public Spell {
public:
  ToggleTraitSpell(std::string n, Trait t) : Spell(n), trait(t) {}
  Trait trait;
};

namespace Spells {
const auto REVEAL = std::make_shared<Spell>("Reveal");
const auto MONSTER_SENSE = std::make_shared<Spell>("Monster Sense");
const auto MONSTER_FREEZE = std::make_shared<Spell>("Monster Freeze");
const auto SUMMON_ORK = std::make_shared<Spell>("Summon Ork");
const auto SUMMON_PLATE = std::make_shared<Spell>("Summon Plate");
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
} // namespace Spells

#endif // __SPELL_H_
