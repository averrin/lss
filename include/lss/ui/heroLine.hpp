#ifndef __HEROLINE_H_
#define __HEROLINE_H_

#include <string>

#include "lss/fragment.hpp"
#include "lss/game/events.hpp"
#include "lss/state.hpp"

#include "EventHandler.hpp"

class HeroLine : public eb::EventHandler<CommitEvent> {
public:
  HeroLine(std::shared_ptr<State> state);
  void setContent(Fragments content);
  void clear();

  static const int HEIGHT = 30;

  virtual void onEvent(CommitEvent &e) override;

private:
  std::shared_ptr<State> state;
};

#endif // __HEROLINE_H_
