#include "lss/ui/statusLine.hpp"
#include "fmt/format.h"
#include "lss/game/events.hpp"
#include "lss/utils.hpp"

StatusLine::StatusLine(std::shared_ptr<State> s) : state(s) {
  eb::EventBus::AddHandler<ItemTakenEvent>(*this);
  eb::EventBus::AddHandler<ItemsFoundEvent>(*this);
  eb::EventBus::AddHandler<MessageEvent>(*this);
  eb::EventBus::AddHandler<DoorOpenedEvent>(*this);
  eb::EventBus::AddHandler<EnemyDiedEvent>(*this);
  eb::EventBus::AddHandler<EnemyTakeDamageEvent>(*this);
  eb::EventBus::AddHandler<HeroTakeDamageEvent>(*this);
};
MessageEvent::MessageEvent(eb::ObjectPtr s, std::string m)
    : eb::Event(s), message(m) {}

void StatusLine::setContent(Fragments content) { state->setContent(content); };

void StatusLine::setModeLine(Modes::ModeName mode) {
  switch (mode) {
  case Modes::INSERT:
    setContent(State::insert_mode);
    break;
  case Modes::DIRECTION:
    setContent(State::direction_mode);
    break;
  case Modes::OBJECTSELECT:
    setContent(State::object_select_mode);
    break;
  }
}

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

void StatusLine::onEvent(DoorOpenedEvent &e) {
  setContent({std::make_shared<Fragment>("Door opened")});
}
void StatusLine::onEvent(EnemyTakeDamageEvent &e) {
  auto enemy = std::dynamic_pointer_cast<Enemy>(e.getSender());
  setContent({F(
      fmt::format("You hit {}: {} dmg", enemy->type.name, e.damage->damage))});
}
void StatusLine::onEvent(HeroTakeDamageEvent &e) {
  auto enemy = std::dynamic_pointer_cast<Enemy>(e.getSender());
  setContent({F(fmt::format("You take <span color='red'>{}</span> dmg from {}",
                            e.damage->damage, enemy->type.name))});
}
void StatusLine::onEvent(EnemyDiedEvent &e) { setContent({F("Enemy died")}); }

void StatusLine::onEvent(ItemTakenEvent &e) {
  setContent({F(fmt::format("You take {}", e.item->getFullTitle()))});
}

void StatusLine::onEvent(ItemsFoundEvent &e) {
  std::vector<std::string> itemNames;
  for (auto o : e.items) {
    auto item = std::dynamic_pointer_cast<Item>(o);
    itemNames.push_back(item->getFullTitle());
  }
  setContent({F(fmt::format("Here lies items: {}",
                            utils::join(itemNames, std::string(", "))))});
}

void StatusLine::onEvent(MessageEvent &e) { setContent({F(e.message)}); }
