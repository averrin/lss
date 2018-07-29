#ifndef __DAMAGE_H_
#define __DAMAGE_H_
#include "lss/game/object.hpp"
#include "lss/game/randomTools.hpp"
#include "lss/game/trait.hpp"

enum class DamageTye {
  WEAPON,
  MAGIC,
  FIRE,
};

class Damage : public Object {
public:
  Damage() : Object() {}
  int damage = 0;
  bool isCritical = false;
  std::vector<Trait> traits;
  int deflected = 0;
};

class DamageSpec : public Object {
public:
  DamageSpec(int m, int d, int e) : Object(), dices(d), edges(e), modifier(m) {}

  int modifier;
  int dices;
  int edges;

  std::shared_ptr<Damage> getDamage(bool critical = false) {
    auto damage = std::make_shared<Damage>();
    damage->damage = hitRoll(modifier, dices, edges);
    return damage;
  }

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

#endif // __DAMAGE_H_
