#include <sstream>
#include <utility>

#include "lss/commands.hpp"
#include "lss/game/events.hpp"
#include "lss/utils.hpp"

#include "EventBus.hpp"

using namespace std::string_literals;

Command::Command(std::vector<std::string> a) : aliases(a) {}
CommandEvent::CommandEvent(eb::ObjectPtr s) : eb::Event(s) {}

MoveCommand::MoveCommand()
    : Command({"move", "m"s, "n"s, "e"s, "s"s, "w"s, "nw", "ne", "se", "sw"}) {}
QuitCommand::QuitCommand() : Command({"quit", "q"s}) {}
PickCommand::PickCommand() : Command({"pick", "p"s}) {}
DigCommand::DigCommand() : Command({"dig", "d"s}) {}
AttackCommand::AttackCommand() : Command({"attack", "a"s}) {}
EquipCommand::EquipCommand() : Command({"equip", "eq"s}) {}
HelpCommand::HelpCommand() : Command({"help", "h"s}) {}
InventoryCommand::InventoryCommand() : Command({"inventory", "i"s}) {}
DropCommand::DropCommand() : Command({"drop", "dr"s}) {}
WaitCommand::WaitCommand() : Command({"wait"}) {}
ZapCommand::ZapCommand() : Command({"zap", "z"s}) {}
UpCommand::UpCommand() : Command({"up"}) {}
DownCommand::DownCommand() : Command({"down"}) {}
UseCommand::UseCommand() : Command({"use", "u"s}) {}

WalkCommandEvent::WalkCommandEvent(Direction d)
    : CommandEvent(nullptr), direction(d) {}
WalkCommand::WalkCommand() : Command({"walk"}) {}
std::optional<std::shared_ptr<CommandEvent>>
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
std::optional<std::shared_ptr<CommandEvent>>
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
std::optional<std::shared_ptr<CommandEvent>>
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
std::optional<std::shared_ptr<CommandEvent>>
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
std::optional<std::shared_ptr<CommandEvent>>
PickCommand::getEvent(std::string s) {
  return std::make_shared<PickCommandEvent>();
}

EquipCommandEvent::EquipCommandEvent() : CommandEvent(nullptr) {}
EquipCommandEvent::EquipCommandEvent(std::shared_ptr<Slot> s,
                                     std::shared_ptr<Item> i)
    : CommandEvent(nullptr), item(i), slot(s) {}
std::optional<std::shared_ptr<CommandEvent>>
EquipCommand::getEvent(std::string s) {
  return std::make_shared<EquipCommandEvent>();
}

UnEquipCommandEvent::UnEquipCommandEvent(std::shared_ptr<Slot> s)
    : CommandEvent(nullptr), slot(s) {}

HelpCommandEvent::HelpCommandEvent() : CommandEvent(nullptr) {}
InventoryCommandEvent::InventoryCommandEvent() : CommandEvent(nullptr) {}

std::optional<std::shared_ptr<CommandEvent>>
HelpCommand::getEvent(std::string s) {
  return std::make_shared<HelpCommandEvent>();
}

std::optional<std::shared_ptr<CommandEvent>>
InventoryCommand::getEvent(std::string s) {
  return std::make_shared<InventoryCommandEvent>();
}

DropCommandEvent::DropCommandEvent() : CommandEvent(nullptr) {}
DropCommandEvent::DropCommandEvent(std::shared_ptr<Item> i)
    : CommandEvent(nullptr), item(i) {}
std::optional<std::shared_ptr<CommandEvent>>
DropCommand::getEvent(std::string s) {
  return std::make_shared<DropCommandEvent>();
}

ZapCommandEvent::ZapCommandEvent() : CommandEvent(nullptr) {}
ZapCommandEvent::ZapCommandEvent(eb::ObjectPtr c, std::shared_ptr<Spell> i)
    : CommandEvent(c), spell(i) {}
std::optional<std::shared_ptr<CommandEvent>>
ZapCommand::getEvent(std::string s) {
  return std::make_shared<ZapCommandEvent>();
}

WaitCommandEvent::WaitCommandEvent() : CommandEvent(nullptr) {}
std::optional<std::shared_ptr<CommandEvent>>
WaitCommand::getEvent(std::string s) {
  return std::make_shared<WaitCommandEvent>();
}

StairEvent::StairEvent(StairType st) : CommandEvent(nullptr), dir(st) {}
std::optional<std::shared_ptr<CommandEvent>>
UpCommand::getEvent(std::string s) {
  return std::make_shared<StairEvent>(StairType::UP);
}

std::optional<std::shared_ptr<CommandEvent>>
DownCommand::getEvent(std::string s) {
  return std::make_shared<StairEvent>(StairType::DOWN);
}

UseCommandEvent::UseCommandEvent() : CommandEvent(nullptr) {}
UseCommandEvent::UseCommandEvent(std::shared_ptr<Item> i)
    : CommandEvent(nullptr), item(i) {}
std::optional<std::shared_ptr<CommandEvent>>
UseCommand::getEvent(std::string s) {
  return std::make_shared<UseCommandEvent>();
}
