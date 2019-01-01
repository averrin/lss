#ifndef __GENERATOR_H_
#define __GENERATOR_H_
#include "lss/game/cell.hpp"
#include "lss/game/location.hpp"

class Generator {
  LibLog::Logger &log = LibLog::Logger::getInstance();
public:
  Generator();
  std::shared_ptr<Location> getLocation(LocationSpec);
  std::shared_ptr<Location>
  getRandomLocation(std::shared_ptr<Player>, int depth = -1,
                    std::shared_ptr<Cell> enter = nullptr);
};

#endif // __GENERATOR_H_
