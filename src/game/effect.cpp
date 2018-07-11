#include "lss/game/effect.hpp"
#include "lss/game/player.hpp"

std::string SpeedModifier::getTitle() {
  return fmt::format("speed {:+g}", R::get(modifier));
}

std::string HPModifier::getTitle() {
  return fmt::format("hp {:+g}", R::get(modifier));
}

std::string MeleeDamage::getTitle() {
  return fmt::format("({:+g}, {}d{})", R::get(modifier), R::get(dices),
                     R::get(edges));
}

std::string VisibilityModifier::getTitle() {
  return fmt::format("increase visibility");
}

std::string CritModifier::getTitle() {
  return fmt::format("crit chance {:+g}", R::get(modifier));
}

std::string ArmorValue::getTitle() {
  return fmt::format("[{:+g}]", R::get(modifier));
}
