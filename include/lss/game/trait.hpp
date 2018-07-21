#ifndef __TRAIT_H_
#define __TRAIT_H_
#include <string>

struct Trait {
  std::string name;

  friend bool operator==(Trait &t1, const Trait &t2) {
    return t1.name == t2.name;
  }
  friend bool operator<(Trait &t1, const Trait &t2) {
    return t1.name < t2.name;
  }
};

namespace Traits {
const Trait DUAL_WIELD = {"Dual wield"};
const Trait NIGHT_VISION = {"Night vision"};
const Trait MIND_SIGHT = {"Mind sight"};
const Trait MAGIC_TORCH = {"Magic torch"};
const Trait FLY = {"Fly"};
const Trait CAN_SWIM = {"Swimming"};
const Trait INVULNERABLE = {"Invulnerable"};
const Trait MOB = {"Mob"};
const Trait CONFUSED = {"Confused"};

// TODO: make it works for hero
const Trait DEADLY_SHADOWS = {"Deadly shadows"};
const Trait SHADOW_RUNNER = {"Shadow runner"};
} // namespace Traits

#endif // __TRAIT_H_
