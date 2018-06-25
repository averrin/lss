#ifndef __SPELL_H_
#define __SPELL_H_
#include "lss/game/object.hpp"

class Spell : public Object {
public:
    Spell(std::string n) : Object(), name(n) {}
    std::string name;
};

namespace Spells {
    const auto REVEAL = std::make_shared<Spell>("Reveal");
    const auto MONSTER_SENSE = std::make_shared<Spell>("Monster Sense");
    const auto MONSTER_FREEZE = std::make_shared<Spell>("Monster Freeze");
    const auto SUMMON_ORK = std::make_shared<Spell>("Summon Ork");
}

#endif // __SPELL_H_
