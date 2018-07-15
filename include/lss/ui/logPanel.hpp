#ifndef __LOGPANEL_H_
#define __LOGPANEL_H_

#include <deque>
#include <string>

#include "lss/fragment.hpp"
#include "lss/game/events.hpp"
#include "lss/state.hpp"

#include "EventHandler.hpp"

class LogLine {
public:
  LogLine(Fragments c) : content(c) {}
  Fragments content;
  int count = 1;
};

class LogPanel : public eb::EventHandler<DoorOpenedEvent>,
                 public eb::EventHandler<EnemyTakeDamageEvent>,
                 public eb::EventHandler<HeroTakeDamageEvent>,
                 public eb::EventHandler<EnemyDiedEvent>,
                 public eb::EventHandler<ItemTakenEvent>,
                 public eb::EventHandler<CommitEvent>,
                 public eb::EventHandler<DigEvent>,
                 public eb::EventHandler<DropEvent>,
                 public eb::EventHandler<HeroDiedEvent>,
                 public eb::EventHandler<LeaveCellEvent>,
                 public eb::EventHandler<EnterCellEvent>,
                 public eb::EventHandler<ItemsFoundEvent>,
                 public eb::EventHandler<MessageEvent> {
public:
  LogPanel(std::shared_ptr<State> state, std::shared_ptr<Player> hero);
  ~LogPanel();
  std::vector<eb::HandlerRegistrationPtr> handlers;
  void setContent(Fragments content);

  std::deque<std::shared_ptr<LogLine>> lines;
  void appendLine(Fragments content);
  void clear();

  virtual void onEvent(HeroDiedEvent &e) override;
  virtual void onEvent(DoorOpenedEvent &e) override;
  virtual void onEvent(EnemyTakeDamageEvent &e) override;
  virtual void onEvent(HeroTakeDamageEvent &e) override;
  virtual void onEvent(EnemyDiedEvent &e) override;
  virtual void onEvent(ItemTakenEvent &e) override;
  virtual void onEvent(ItemsFoundEvent &e) override;
  virtual void onEvent(MessageEvent &e) override;
  virtual void onEvent(EnterCellEvent &e) override;
  virtual void onEvent(LeaveCellEvent &e) override;
  virtual void onEvent(DigEvent &e) override;
  virtual void onEvent(DropEvent &e) override;
  virtual void onEvent(CommitEvent &e) override;

private:
  std::shared_ptr<State> state;
};

#endif // __LOGPANEL_H_
