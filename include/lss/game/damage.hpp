#ifndef __DAMAGE_H_
#define __DAMAGE_H_
#include "lss/game/object.hpp"
#include "lss/game/trait.hpp"

class Damage : public Object {
public:
  Damage() : Object() {}
  int damage = 0;
  bool isCritical = false;
  std::vector<Trait> traits;
  int deflected = 0;
};

#endif // __DAMAGE_H_
