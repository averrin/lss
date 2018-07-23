#include "lss/game/effect.hpp"
#include "lss/game/player.hpp"

#include <fmt/format.h>

std::string SpeedModifier::getTitle() {
  return fmt::format("speed {:+d}", R::get(modifier));
}
std::string SpeedModifier::getSign() {
  return "\u1f5f2"s;
}

std::string HPModifier::getTitle() {
  return fmt::format("hp {:+d}", R::get(modifier));
}
std::string HPModifier::getSign() {
  return "\u1f5f2"s;
}

std::string MeleeDamage::getTitle() {
  return fmt::format("({:+d}, {}d{})", R::get(modifier), R::get(dices),
                     R::get(edges));
}
std::string MeleeDamage::getSign() {
  return ""s;
}

std::string VisibilityModifier::getTitle() {
  return fmt::format("increase visibility");
}
std::string VisibilityModifier::getSign() {
  return "V"s;
}

std::string CritModifier::getTitle() {
  return fmt::format("crit chance {:+d}", R::get(modifier));
}
std::string CritModifier::getSign() {
  return "⚔"s;
}

std::string LastingEffect::getTitle() {
  return fmt::format("{} for {}ap", effect->getTitle(), duration);
}
std::string LastingEffect::getSign() {
  return effect->getSign();
}

std::string ArmorValue::getTitle() {
  return fmt::format("[{:+d}]", R::get(modifier));
}
std::string ArmorValue::getSign() {
  return ""s;
}

// TODO: get name in constructor
std::string OverTimeEffect::getTitle() {
  return fmt::format("not implemented");
}
std::string OverTimeEffect::getSign() {
  switch (type) {
    case EoT::HEAL:
      return "H"s;
      break;
    case EoT::POISON:
      return "P"s;
      break;
  }
  return ""s;
}

std::string TraitEffect::getTitle() { return fmt::format(trait.name); }
std::string TraitEffect::getSign() {
  return std::string{trait.name.front()};
}
std::string OnHitEffect::getTitle() { return fmt::format("on hit {}", effect->getTitle()); }
std::string OnHitEffect::getSign() {
  return ""s;
}
