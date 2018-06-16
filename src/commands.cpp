#include <sstream>
#include <utility>

#include "lss/command.hpp"
#include "lss/game/events.hpp"

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

Command::Command(std::vector<std::string> a) : aliases(a) {}
CommandEvent::CommandEvent(eb::ObjectPtr s) : eb::Event(s) {}

MoveCommand::MoveCommand()
    : Command({"move", "m"s, "n"s, "e"s, "s"s, "w"s, "nw", "ne", "se", "sw"}) {}

QuitCommand::QuitCommand() : Command({"quit", "q"s}) {}

PickCommand::PickCommand() : Command({"pick", "p"s}) {}

std::shared_ptr<CommandEvent> MoveCommand::getEvent(std::string cmd) {
  auto tokens = split(cmd, ' ');
  std::string dirString = tokens.front();
  if (tokens.size() > 1) {
    dirString = tokens[1];
  }
  Direction direction;
  if (dirString == "n"s) {
    direction = Direction::N;
  } else if (dirString == "e"s) {
    direction = Direction::E;
  } else if (dirString == "s"s) {
    direction = Direction::S;
  } else if (dirString == "w"s) {
    direction = Direction::W;
  } else if (dirString == "nw"s) {
    direction = Direction::NW;
  } else if (dirString == "ne"s) {
    direction = Direction::NE;
  } else if (dirString == "sw"s) {
    direction = Direction::SW;
  } else if (dirString == "se"s) {
    direction = Direction::SE;
  }
  return std::make_shared<MoveCommandEvent>(direction);
}
