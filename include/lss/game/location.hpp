#ifndef __LOCATION_H_
#define __LOCATION_H_
#include "lss/game/cell.hpp"
#include "lss/game/events.hpp"
#include "lss/game/object.hpp"

#include "EventHandler.hpp"

class Player;
class Location : public eb::EventHandler<EnemyDiedEvent>,
                 public eb::EventHandler<ItemTakenEvent>,
                 public eb::EventHandler<DigEvent>,
                 public eb::EventHandler<EnterCellEvent> {
public:
  Location();
  Cells cells;
  Objects objects;

  void updateView(std::shared_ptr<Player>);

  virtual void onEvent(EnemyDiedEvent &e) override;
  virtual void onEvent(ItemTakenEvent &e) override;
  virtual void onEvent(EnterCellEvent &e) override;
  virtual void onEvent(DigEvent &e) override;
};

#endif // __LOCATION_H_
