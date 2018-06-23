#include "lss/game/effect.hpp"
#include "lss/game/player.hpp"


void SpeedModifier::apply(Player* hero) {
    hero->speed += modifier;
}

void SpeedModifier::undo(Player* hero) {
    hero->speed -= modifier;
}

std::string SpeedModifier::getTitle() {
    return fmt::format("speed +{}", modifier);
}
