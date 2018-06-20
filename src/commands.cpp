#include <sstream>
#include <utility>

#include "lss/command.hpp"
#include "lss/game/events.hpp"

#include "EventBus.hpp"

using namespace std::string_literals;

// TODO: to utils
std::vector<std::string> split(std::string strToSplit, char delimeter) {
  std::stringstream ss(strToSplit);
  std::string item;
  std::vector<std::string> splittedStrings;
  while (std::getline(ss, item, delimeter)) {
    splittedStrings.push_back(item);
  }
  return splittedStrings;
}

// TODO: to utils
std::optional<Direction> getDirection(std::string dirString) {
  if (dirString == "n"s) {
    return Direction::N;
  } else if (dirString == "e"s) {
    return Direction::E;
  } else if (dirString == "s"s) {
    return Direction::S;
  } else if (dirString == "w"s) {
    return Direction::W;
  } else if (dirString == "nw"s) {
    return Direction::NW;
  } else if (dirString == "ne"s) {
    return Direction::NE;
  } else if (dirString == "sw"s) {
    return Direction::SW;
  } else if (dirString == "se"s) {
    return Direction::SE;
  }
  return std::nullopt;
}

Command::Command(std::vector<std::string> a) : aliases(a) {}
CommandEvent::CommandEvent(eb::ObjectPtr s) : eb::Event(s) {}

MoveCommand::MoveCommand()
    : Command({"move", "m"s, "n"s, "e"s, "s"s, "w"s, "nw", "ne", "se", "sw"}) {}
QuitCommand::QuitCommand() : Command({"quit", "q"s}) {}
PickCommand::PickCommand() : Command({"pick", "p"s}) {}
DigCommand::DigCommand() : Command({"dig", "d"s}) {}
AttackCommand::AttackCommand() : Command({"attack", "a"s}) {}

WalkCommandEvent::WalkCommandEvent(Direction d) : CommandEvent(nullptr), direction(d) {}
WalkCommand::WalkCommand() : Command({"walk"}) {}
std::optional<std::shared_ptr<CommandEvent>> WalkCommand::getEvent(std::string cmd) {
  auto tokens = split(cmd, ' ');
  std::string dirString = tokens.front();
  if (tokens.size() > 1) {
    dirString = tokens[1];
  }
  auto direction = getDirection(dirString);
  if (direction == std::nullopt) {
    MessageEvent me(nullptr, "Please specify direction: n, e, s, w, nw, ne, se, sw.");
    eb::EventBus::FireEvent(me);
    return std::nullopt;
  }
  return std::make_shared<WalkCommandEvent>(*direction);
}

DigCommandEvent::DigCommandEvent(Direction d) : CommandEvent(nullptr), direction(d) {}
std::optional<std::shared_ptr<CommandEvent>> DigCommand::getEvent(std::string cmd) {
  auto tokens = split(cmd, ' ');
  std::string dirString = tokens.front();
  if (tokens.size() > 1) {
    dirString = tokens[1];
  }
  auto direction = getDirection(dirString);
  if (direction == std::nullopt) {
    MessageEvent me(nullptr, "Please specify direction: n, e, s, w, nw, ne, se, sw.");
    eb::EventBus::FireEvent(me);
    return std::nullopt;
  }
  return std::make_shared<DigCommandEvent>(*direction);
}

std::optional<std::shared_ptr<CommandEvent>> MoveCommand::getEvent(std::string cmd) {
  auto tokens = split(cmd, ' ');
  std::string dirString = tokens.front();
  if (tokens.size() > 1) {
    dirString = tokens[1];
  }
  auto direction = getDirection(dirString);
  if (direction == std::nullopt) {
    MessageEvent me(nullptr, "Please specify direction: n, e, s, w, nw, ne, se, sw.");
    eb::EventBus::FireEvent(me);
    return std::nullopt;
  }
  return std::make_shared<MoveCommandEvent>(*direction);
}

AttackCommandEvent::AttackCommandEvent(Direction d) : CommandEvent(nullptr), direction(d) {}
std::optional<std::shared_ptr<CommandEvent>> AttackCommand::getEvent(std::string cmd) {
  auto tokens = split(cmd, ' ');
  std::string dirString = tokens.front();
  if (tokens.size() > 1) {
    dirString = tokens[1];
  }
  auto direction = getDirection(dirString);
  if (direction == std::nullopt) {
    MessageEvent me(nullptr, "Please specify direction: n, e, s, w, nw, ne, se, sw.");
    eb::EventBus::FireEvent(me);
    return std::nullopt;
  }
  return std::make_shared<AttackCommandEvent>(*direction);
}
