#include "lss/ui/statusLine.hpp"
#include "format.h"

StatusLine::StatusLine(std::shared_ptr<State> s) : state(s){};
MessageEvent::MessageEvent(eb::ObjectPtr s, std::string m): eb::Event(s), message(m) {}

void StatusLine::setContent(Fragments content) {
  state->statusFragments = content;
};

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

void StatusLine::onEvent(DoorOpenedEvent &e) {
  setContent({std::make_shared<Fragment>("Door opened")});
}
void StatusLine::onEvent(EnemyTakeDamageEvent &e) {
  setContent({F(fmt::format("You hit enemy: {} dmg", e.damage))});
}
void StatusLine::onEvent(EnemyDiedEvent &e) { setContent({F("Enemy died")}); }

void StatusLine::onEvent(ItemTakenEvent &e) {
  setContent({F(fmt::format("You take {}", e.item->name))});
}

void StatusLine::onEvent(ItemsFoundEvent &e) {
  setContent({F(fmt::format("Here lies {} items", e.items.size()))});
}

void StatusLine::onEvent(MessageEvent &e) {
  setContent({F(e.message)});
}
