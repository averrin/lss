#ifndef __LOCATION_H_
#define __LOCATION_H_
#include "lss/game/cell.hpp"

class Object;
class Location {
public:
  Location();
  Cells cells;  
  std::vector<std::shared_ptr<Object>> objects;
};


#endif // __LOCATION_H_
