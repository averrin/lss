#ifndef __COMMAND_H_
#define __COMMAND_H_

#include <string>
#include <vector>

#include "Event.hpp"
#include "lss/game/events.hpp"

class Command {
public:
  Command(std::vector<std::string>);
  std::vector<std::string> aliases;
  virtual std::shared_ptr<CommandEvent> getEvent(std::string) = 0;
};

class MoveCommand : public Command {
public:
  MoveCommand();
  std::shared_ptr<CommandEvent> getEvent(std::string);
};

class QuitCommand : public Command {
public:
  QuitCommand();
  std::shared_ptr<CommandEvent> getEvent(std::string);
};

class PickCommand : public Command {
public:
  PickCommand();
  std::shared_ptr<CommandEvent> getEvent(std::string);
};

#endif // __COMMAND_H_
