#include "lss/game/damageSpec.hpp"
#include "lss/game/damage.hpp"

std::shared_ptr<Damage> DamageSpec::getDamage(float modifier, bool critical) {
  auto damage = std::make_shared<Damage>(*this);
  if (!critical) {
    damage->damage = hitRoll() * modifier;
  } else {
    damage->damage = criticalHit() * modifier;
  }
  return damage;
}

DamageSpec DamageSpec::roll() {
  _modifier = R::I(_modifier);
  _dices = R::I(_dices);
  _edges = R::I(_edges);
  modifier = R::get(_modifier);
  dices = R::get(_dices);
  edges = R::get(_edges);
  return *this;
}
