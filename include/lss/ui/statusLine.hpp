#ifndef __STATUSLINE_H_
#define __STATUSLINE_H_

#include <string>

#include "lss/fragment.hpp"
#include "lss/game/events.hpp"
#include "lss/state.hpp"

#include "EventHandler.hpp"

class StatusLine : public eb::EventHandler<DoorOpenedEvent>,
                   public eb::EventHandler<EnemyTakeDamageEvent>,
                   public eb::EventHandler<EnemyDiedEvent>,
                   public eb::EventHandler<ItemTakenEvent> {
public:
  StatusLine(std::shared_ptr<State> state);
  void setContent(Fragments content);
  void clear();

  static const int HEIGHT = 30;

  virtual void onEvent(DoorOpenedEvent &e) override;
  virtual void onEvent(EnemyTakeDamageEvent &e) override;
  virtual void onEvent(EnemyDiedEvent &e) override;
  virtual void onEvent(ItemTakenEvent &e) override;

private:
  std::shared_ptr<State> state;
};

#endif // __STATUSLINE_H_
