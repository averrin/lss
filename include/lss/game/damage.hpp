#ifndef __DAMAGE_H_
#define __DAMAGE_H_
#include "lss/game/damageSpec.hpp"
#include "lss/game/object.hpp"
#include "lss/game/randomTools.hpp"
#include "lss/game/trait.hpp"

class Damage : public Object {
public:
  Damage(DamageSpec ds) : Object(), spec(ds) {}
  int damage = 0;
  bool isCritical = false;
  std::vector<Trait> traits;
  std::vector<Trait> defTraits;
  int deflected = 0;
  DamageSpec spec;
};

#endif // __DAMAGE_H_
