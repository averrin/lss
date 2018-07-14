#include "lss/game/effect.hpp"
#include "lss/game/player.hpp"

std::string SpeedModifier::getTitle() {
  return fmt::format("speed {:+d}", R::get(modifier));
}

std::string HPModifier::getTitle() {
  return fmt::format("hp {:+d}", R::get(modifier));
}

std::string MeleeDamage::getTitle() {
  return fmt::format("({:+d}, {}d{})", R::get(modifier), R::get(dices),
                     R::get(edges));
}

std::string VisibilityModifier::getTitle() {
  return fmt::format("increase visibility");
}

std::string CritModifier::getTitle() {
  return fmt::format("crit chance {:+d}", R::get(modifier));
}

std::string ArmorValue::getTitle() {
  return fmt::format("[{:+d}]", R::get(modifier));
}
