#ifndef __SPELL_H_
#define __SPELL_H_
#include "lss/game/object.hpp"

class Spell : public Object {
public:
  Spell(std::string n, int c = 0) : Object(), name(n), cost(c) {}
  std::string name;
  int cost;
};

namespace Spells {
const auto REVEAL = std::make_shared<Spell>("Reveal");
const auto MONSTER_SENSE = std::make_shared<Spell>("Monster Sense");
const auto MONSTER_FREEZE = std::make_shared<Spell>("Monster Freeze");
const auto SUMMON_ORK = std::make_shared<Spell>("Summon Ork");
const auto TOGGLE_DUAL_WIELD = std::make_shared<Spell>("Toggle Dual Wield trait");
const auto TOGGLE_NIGHT_VISION = std::make_shared<Spell>("Toggle Night Vision trait");
const auto TOGGLE_MIND_SIGHT = std::make_shared<Spell>("Toggle Mind Sight trait");
} // namespace Spells

#endif // __SPELL_H_
