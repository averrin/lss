#ifndef __DAMAGESPEC_H_
#define __DAMAGESPEC_H_
#include "lss/game/object.hpp"
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
  DamageSpec(R::rndInt m, R::rndInt d, R::rndInt e, DamageType dt)
      : Object(), _dices(R::I(d)), _edges(R::I(e)), _modifier(R::I(m)),
        type(dt) {
    modifier = R::get(_modifier);
    dices = R::get(_dices);
    edges = R::get(_edges);
  }

  int modifier;
  int dices;
  int edges;
  DamageType type;

  DamageSpec roll();
  std::shared_ptr<Damage> getDamage(bool critical = false);

  int hitRoll() { return hitRoll(modifier, dices, edges); }

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

  int criticalHit() { return criticalHit(modifier, dices, edges); }

  static int criticalHit(int m, int d, int e) {
    auto damage = m + d * e;
    if (damage < 0) {
      damage = 0;
    }
    return damage;
  }

private:
  R::rndInt _modifier;
  R::rndInt _dices;
  R::rndInt _edges;
};

#endif // __DAMAGESPEC_H_
