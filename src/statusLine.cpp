#include "format.h"
#include "lss/ui/statusLine.hpp"

StatusLine::StatusLine(std::shared_ptr<State> s): state(s) {};

void StatusLine::setContent(Fragments content) {
    state->statusFragments = content;
};

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

void StatusLine::onEvent(DoorOpenedEvent & e) {
    setContent({std::make_shared<Fragment>("Door opened")});
}
void StatusLine::onEvent(EnemyTakeDamageEvent & e) {
    setContent({F(fmt::format("You hit enemy: {} dmg", e.damage))});
}
void StatusLine::onEvent(EnemyDiedEvent & e) {
    setContent({F("Enemy died")});
}

void StatusLine::onEvent(ItemTakenEvent & e) {
    auto itemName = e.item->type == ItemType::CORPSE ? "enemy corpse" : "unknown thing";
    setContent({F(fmt::format("You take {}", itemName))});
}

