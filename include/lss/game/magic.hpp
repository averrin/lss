#ifndef __MAGIC_H_
#define __MAGIC_H_
#include "EventBus.hpp"
#include "EventHandler.hpp"

#include "lss/game/events.hpp"
#include "lss/game/object.hpp"
#include "lss/game/player.hpp"
#include "lss/game/spell.hpp"

class Magic : public Object, public eb::EventHandler<ZapCommandEvent> {
public:
  Magic(std::shared_ptr<Player>);
  ~Magic();
  std::shared_ptr<Player> hero;
  void castSpell(std::shared_ptr<Creature>, std::shared_ptr<Spell> spell);
  void castTargetSpell(std::shared_ptr<Creature>,
                       std::shared_ptr<TargetSpell> spell);
  void castLineSpell(std::shared_ptr<Creature>,
                     std::shared_ptr<LineSpell> spell);
  void pauseAndEraseFireballs();
  void applySpellOnCells(std::shared_ptr<Creature> caster,
                         std::shared_ptr<Spell> spell,
                         std::vector<std::shared_ptr<Cell>> cells);

  virtual void onEvent(ZapCommandEvent &e) override;
};

#endif // __MAGIC_H_
