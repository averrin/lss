#include "lss/game/damageSpec.hpp"
#include "lss/game/damage.hpp"

std::shared_ptr<Damage> DamageSpec::getDamage(bool critical) {
  auto damage = std::make_shared<Damage>(*this);
  damage->damage = hitRoll(modifier, dices, edges);
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
