#include "lss/ui/statusLine.hpp"
#include "fmt/format.h"

StatusLine::StatusLine(std::shared_ptr<State> s) : state(s){};
MessageEvent::MessageEvent(eb::ObjectPtr s, std::string m)
    : eb::Event(s), message(m) {}

template <typename T>
std::string join(const T &array, const std::string &delimiter) {
  std::string res;
  for (auto &element : array) {
    if (!res.empty()) {
      res += delimiter;
    }

    res += element;
  }

  return res;
}

void StatusLine::setContent(Fragments content) { state->setContent(content); };

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

void StatusLine::onEvent(DoorOpenedEvent &e) {
  setContent({std::make_shared<Fragment>("Door opened")});
}
void StatusLine::onEvent(EnemyTakeDamageEvent &e) {
  auto enemy = std::dynamic_pointer_cast<Enemy>(e.getSender());
  setContent(
      {F(fmt::format("You hit {}: {} dmg", enemy->type.name, e.damage))});
}
void StatusLine::onEvent(EnemyDiedEvent &e) { setContent({F("Enemy died")}); }

void StatusLine::onEvent(ItemTakenEvent &e) {
  setContent({F(fmt::format("You take {}", e.item->type.name))});
}

void StatusLine::onEvent(ItemsFoundEvent &e) {
  std::vector<std::string> itemNames;
  for (auto o : e.items) {
    auto item = std::dynamic_pointer_cast<Item>(o);
    itemNames.push_back(item->type.name);
  }
  setContent({F(fmt::format("Here lies {} items: {}", e.items.size(),
                            join(itemNames, std::string(", "))))});
}

void StatusLine::onEvent(MessageEvent &e) { setContent({F(e.message)}); }
