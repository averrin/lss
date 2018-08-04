#ifndef __DAMAGESPEC_H_
#define __DAMAGESPEC_H_
#include "lss/game/randomTools.hpp"

enum class DamageType {
  BASIC,
  WEAPON,
  MAGIC,
  FIRE,
  ACID,
  /**/
  ICE,
  HOLY,
  DEATH,
  AIR,
  EARTH,
  LIGHT,
  DARKNESS,
  WATER,
};

class Damage;
class DamageSpec : public Object {
public:
  DamageSpec(R::randInt m, R::randInt d, R::randInt e, DamageType dt)
      : Object(), dices(d), edges(e), modifier(m), type(dt) {}

  R::randInt modifier;
  R::randInt dices;
  R::randInt edges;
  DamageType type;

  static int hitRoll(int m, int d, int e) {
    auto damage = 0;
    for (auto n = 0; n < d; n++) {
      damage += R::Z(1, e);
    }
    damage += m;
    if (damage < 0) {
      damage = 0;
    }
    return damage;
  }

  static int criticalHit(int m, int d, int e) {
    auto damage = m + d * e;
    if (damage < 0) {
      damage = 0;
    }
    return damage;
  }
};



#endif // __DAMAGESPEC_H_
