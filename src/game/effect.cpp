#include "lss/game/effect.hpp"
#include "lss/game/player.hpp"

std::string SpeedModifier::getTitle() {
  return fmt::format("speed {:+g}", modifier);
}

std::string HPModifier::getTitle() { return fmt::format("hp {:+g}", modifier); }

std::string MeleeDamage::getTitle() {
  return fmt::format("({:+g}, {}d{})", modifier, dices, edges);
}

std::string VisibilityModifier::getTitle() {
  return fmt::format("increase visibility", modifier);
}

std::string ArmorValue::getTitle() { return fmt::format("[{:+g}]", modifier); }
