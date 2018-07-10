#ifndef __GENERATOR_H_
#define __GENERATOR_H_
#include "lss/game/cell.hpp"
#include "lss/game/location.hpp"

class Generator {
public:
  Generator();
  std::shared_ptr<Location> getLocation(LocationSpec);
  std::shared_ptr<Location> getRandomLocation(std::shared_ptr<Player>);
};

#endif // __GENERATOR_H_
