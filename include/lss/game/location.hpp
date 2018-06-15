#ifndef __LOCATION_H_
#define __LOCATION_H_
#include "lss/game/cell.hpp"
#include "lss/game/events.hpp"
#include "lss/game/object.hpp"

#include "EventHandler.hpp"

// class Object;
class Location : public eb::EventHandler<EnemyDiedEvent> {
public:
  Location();
  Cells cells;  
  std::vector<std::shared_ptr<Object>> objects;

  virtual void onEvent(EnemyDiedEvent & e) override;
};


#endif // __LOCATION_H_
