#ifndef __LOCATION_H_
#define __LOCATION_H_
#include "lss/game/cell.hpp"

class Creature;
class Location {
public:
  Location();
  Cells cells;  
  std::vector<std::shared_ptr<Creature>> creatures;
};


#endif // __LOCATION_H_
