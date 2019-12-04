#ifndef __STATUSLINE_H_
#define __STATUSLINE_H_

#include <string>

#include "ui/actions.hpp"
#include "ui/fragment.hpp"
#include "ui/state.hpp"
#include "lss/game/events.hpp"

#include "EventHandler.hpp"

class StatusLine : public eb::EventHandler<DoorOpenedEvent>,
                   public eb::EventHandler<EnemyTakeDamageEvent>,
                   public eb::EventHandler<HeroTakeDamageEvent>,
                   public eb::EventHandler<EnemyDiedEvent>,
                   public eb::EventHandler<ItemTakenEvent>,
                   public eb::EventHandler<ItemsFoundEvent>,
                   public eb::EventHandler<MessageEvent> {
public:
  StatusLine(std::shared_ptr<State> state);
  ~StatusLine();
  void setContent(Fragments content);
  void setModeLine(Modes::ModeName);
  std::vector<eb::HandlerRegistrationPtr> handlers;
  void clear();

  static const int HEIGHT = 30;

  virtual void onEvent(DoorOpenedEvent &e) override;
  virtual void onEvent(EnemyTakeDamageEvent &e) override;
  virtual void onEvent(HeroTakeDamageEvent &e) override;
  virtual void onEvent(EnemyDiedEvent &e) override;
  virtual void onEvent(ItemTakenEvent &e) override;
  virtual void onEvent(ItemsFoundEvent &e) override;
  virtual void onEvent(MessageEvent &e) override;

private:
  std::shared_ptr<State> state;
};

#endif // __STATUSLINE_H_
