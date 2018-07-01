#include "lss/game/effect.hpp"
#include "lss/game/player.hpp"

std::string SpeedModifier::getTitle() {
  return fmt::format("speed +{}", modifier);
}

std::string HPModifier::getTitle() { return fmt::format("hp +{}", modifier); }

std::string MeleeDamage::getTitle() {
  return fmt::format("(+{}, {}d{})", modifier, dices, edges);
}

std::string VisibilityModifier::getTitle() {
  return fmt::format("increase visibility", modifier);
}

std::string ArmorValue::getTitle() {
  return fmt::format("[+{}]", modifier);
}
