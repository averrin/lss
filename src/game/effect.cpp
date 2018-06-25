#include "lss/game/effect.hpp"
#include "lss/game/player.hpp"

void SpeedModifier::apply(Player *hero) { hero->speed += modifier; }

void SpeedModifier::undo(Player *hero) { hero->speed -= modifier; }

std::string SpeedModifier::getTitle() {
  return fmt::format("speed +{}", modifier);
}

void HPModifier::apply(Player *hero) { hero->hp_max += modifier; }
void HPModifier::undo(Player *hero) { hero->hp_max -= modifier; }
std::string HPModifier::getTitle() { return fmt::format("hp +{}", modifier); }

void MeleeDamage::apply(Player *hero) {
  hero->damage_dices = dices;
  hero->damage_edges = edges;
  hero->damage_modifier = modifier;
}

void MeleeDamage::undo(Player *hero) {
  hero->damage_dices = 1;
  hero->damage_edges = 1;
  hero->damage_modifier = 0;
}

std::string MeleeDamage::getTitle() {
  return fmt::format("(+{}, {}d{})", modifier, dices, edges);
}

void VisibilityModifier::apply(Player *hero) { hero->visibilityDistance += modifier; hero->calcViewField(); }
void VisibilityModifier::undo(Player *hero) { hero->visibilityDistance -= modifier; hero->calcViewField(); }
std::string VisibilityModifier::getTitle() { return fmt::format("increase visibility", modifier); }
