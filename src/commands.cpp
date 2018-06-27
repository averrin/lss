#include <sstream>
#include <utility>

#include "lss/command.hpp"
#include "lss/game/events.hpp"
#include "lss/utils.hpp"

#include "EventBus.hpp"

using namespace std::string_literals;

CommandEvent::CommandEvent(eb::ObjectPtr s) : eb::Event(s) {}

WalkCommandEvent::WalkCommandEvent(Direction d)
    : CommandEvent(nullptr), direction(d) {}
std::optional<std::shared_ptr<WalkCommandEvent>>
WalkCommand::getEvent(std::string cmd) {
  auto tokens = utils::split(cmd, ' ');
  std::string dirString = tokens.front();
  if (tokens.size() > 1) {
    dirString = tokens[1];
  }
  auto direction = utils::getDirectionByName(dirString);
  if (direction == std::nullopt) {
    MessageEvent me(nullptr,
                    "Please specify direction: n, e, s, w, nw, ne, se, sw.");
    eb::EventBus::FireEvent(me);
    return std::nullopt;
  }
  return std::make_shared<WalkCommandEvent>(*direction);
}

DigCommandEvent::DigCommandEvent(Direction d)
    : CommandEvent(nullptr), direction(d) {}
std::optional<std::shared_ptr<DigCommandEvent>>
DigCommand::getEvent(std::string cmd) {
  auto tokens = utils::split(cmd, ' ');
  std::string dirString = tokens.front();
  if (tokens.size() > 1) {
    dirString = tokens[1];
  }
  auto direction = utils::getDirectionByName(dirString);
  if (direction == std::nullopt) {
    MessageEvent me(nullptr,
                    "Please specify direction: n, e, s, w, nw, ne, se, sw.");
    eb::EventBus::FireEvent(me);
    return std::nullopt;
  }
  return std::make_shared<DigCommandEvent>(*direction);
}

MoveCommandEvent::MoveCommandEvent(Direction d)
    : CommandEvent(nullptr), direction(d) {}
std::optional<std::shared_ptr<MoveCommandEvent>>
MoveCommand::getEvent(std::string cmd) {
  auto tokens = utils::split(cmd, ' ');
  std::string dirString = tokens.front();
  if (tokens.size() > 1) {
    dirString = tokens[1];
  }
  auto direction = utils::getDirectionByName(dirString);
  if (direction == std::nullopt) {
    MessageEvent me(nullptr,
                    "Please specify direction: n, e, s, w, nw, ne, se, sw.");
    eb::EventBus::FireEvent(me);
    return std::nullopt;
  }
  return std::make_shared<MoveCommandEvent>(*direction);
}

AttackCommandEvent::AttackCommandEvent(Direction d)
    : CommandEvent(nullptr), direction(d) {}
std::optional<std::shared_ptr<AttackCommandEvent>>
AttackCommand::getEvent(std::string cmd) {
  auto tokens = utils::split(cmd, ' ');
  std::string dirString = tokens.front();
  if (tokens.size() > 1) {
    dirString = tokens[1];
  }
  auto direction = utils::getDirectionByName(dirString);
  if (direction == std::nullopt) {
    MessageEvent me(nullptr,
                    "Please specify direction: n, e, s, w, nw, ne, se, sw.");
    eb::EventBus::FireEvent(me);
    return std::nullopt;
  }
  return std::make_shared<AttackCommandEvent>(*direction);
}

PickCommandEvent::PickCommandEvent() : CommandEvent(nullptr) {}
EquipCommandEvent::EquipCommandEvent() : CommandEvent(nullptr) {}
EquipCommandEvent::EquipCommandEvent(std::shared_ptr<Slot> s,
                                     std::shared_ptr<Item> i)
    : CommandEvent(nullptr), item(i), slot(s) {}

UnEquipCommandEvent::UnEquipCommandEvent(std::shared_ptr<Slot> s)
    : CommandEvent(nullptr), slot(s) {}

HelpCommandEvent::HelpCommandEvent() : CommandEvent(nullptr) {}
InventoryCommandEvent::InventoryCommandEvent() : CommandEvent(nullptr) {}


DropCommandEvent::DropCommandEvent() : CommandEvent(nullptr) {}
DropCommandEvent::DropCommandEvent(std::shared_ptr<Item> i)
    : CommandEvent(nullptr), item(i) {}

ZapCommandEvent::ZapCommandEvent() : CommandEvent(nullptr) {}
ZapCommandEvent::ZapCommandEvent(std::shared_ptr<Spell> i)
    : CommandEvent(nullptr), spell(i) {}

WaitCommandEvent::WaitCommandEvent() : CommandEvent(nullptr) {}
