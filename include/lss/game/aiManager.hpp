#ifndef __AIMANAGER_H_
#define __AIMANAGER_H_
#include "lss/utils.hpp"
#include "lss/game/creature.hpp"

class Location;
class AiManager {
  private:
  LibLog::Logger &log = LibLog::Logger::getInstance();
  public:

std::shared_ptr<Location> currentLocation;
  AiManager(std::shared_ptr<Location>);
  void processCommit(std::vector<std::shared_ptr<Creature>> creatures, int ap);
};


#endif // __AIMANAGER_H_
