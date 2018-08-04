#include "lss/game/damage.hpp"
#include "lss/game/damageSpec.hpp"


  std::shared_ptr<Damage> DamageSpec::getDamage(bool critical = false) {
    auto damage = std::make_shared<Damage>(this);
    damage->damage = hitRoll(modifier, dices, edges);
    return damage;
  }

