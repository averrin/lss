#ifndef __HEROLINE_H_
#define __HEROLINE_H_

#include <string>

#include "lss/fragment.hpp"
#include "lss/game/events.hpp"
#include "lss/game/player.hpp"
#include "lss/state.hpp"

#include "EventHandler.hpp"

class HeroLine : public eb::EventHandler<CommitEvent>,
                 public eb::EventHandler<HeroTakeDamageEvent>
{
public:
  HeroLine(std::shared_ptr<State> state, std::shared_ptr<Player>);
std::shared_ptr<Player> hero;
  void setContent(Fragments content);
  void clear();
    void update();


  static const int HEIGHT = 30;

  virtual void onEvent(CommitEvent &e) override;
  virtual void onEvent(HeroTakeDamageEvent &e) override;

private:
  std::shared_ptr<State> state;
};

#endif // __HEROLINE_H_
