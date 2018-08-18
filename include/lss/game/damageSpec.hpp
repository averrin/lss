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
  FROST,
  HOLY,
  DEATH,
  AIR,
  EARTH,
  LIGHT,
  DARKNESS,
  WATER,
  ELECTRICITY,
};

const std::map<DamageType, Trait> RESISTS = {
    {DamageType::WEAPON, Traits::WEAPON_RESIST},
    {DamageType::MAGIC, Traits::MAGIC_RESIST},

    {DamageType::ACID, Traits::ACID_RESIST},
    {DamageType::FIRE, Traits::FIRE_RESIST},
    {DamageType::FROST, Traits::FROST_RESIST},
};

const std::map<DamageType, Trait> IMMUNITIES = {
    {DamageType::WEAPON, Traits::WEAPON_IMMUNE},
    {DamageType::MAGIC, Traits::MAGIC_IMMUNE},

    {DamageType::ACID, Traits::ACID_IMMUNE},
    {DamageType::FIRE, Traits::FIRE_IMMUNE},
    {DamageType::FROST, Traits::FROST_IMMUNE},
};

const std::map<DamageType, Trait> VULNERABLES = {
    {DamageType::WEAPON, Traits::WEAPON_VULNERABLE},
    {DamageType::MAGIC, Traits::MAGIC_VULNERABLE},

    {DamageType::ACID, Traits::ACID_VULNERABLE},
    {DamageType::FIRE, Traits::FIRE_VULNERABLE},
    {DamageType::FROST, Traits::FROST_VULNERABLE},
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
  std::shared_ptr<Damage> getDamage(float modifier = 1, bool critical = false);

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
