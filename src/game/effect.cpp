#include "lss/game/effect.hpp"
#include "lss/game/player.hpp"

#include <fmt/format.h>

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

std::string LastingEffect::getTitle() {
  return fmt::format("{} for {}ap", effect->getTitle(), duration);
}

std::string ArmorValue::getTitle() {
  return fmt::format("[{:+d}]", R::get(modifier));
}

//TODO: get name in constructor
std::string OverTimeEffect::getTitle() {
  return fmt::format("not implemented");
}

std::string Poison::getTitle() { return fmt::format("poisoned"); }
std::string Vampire::getTitle() { return fmt::format("life steal"); }
std::string TraitEffect::getTitle() { return fmt::format(trait.name); }


void OverTimeEffect::apply(std::shared_ptr<Creature> c, int ap) {
    accomulator += ap;
    while (accomulator >= tick) {
        c->hp += 5;
        accomulator -= tick;
    }
}
