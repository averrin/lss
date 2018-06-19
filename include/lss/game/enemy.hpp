#ifndef __ENEMY_H_
#define __ENEMY_H_

#include "Event.hpp"
#include "EventBus.hpp"
#include "lss/game/creature.hpp"
#include "lss/game/item.hpp"


enum EnemyType {
  GOBLIN, ORK, PIXI,
};

class Enemy : public Creature,
              public eb::EventHandler<CommitEvent>
{
public:
  Enemy(EnemyType);
  ~Enemy();
  eb::HandlerRegistrationPtr registration;
  bool interact();
  std::shared_ptr<Item> drop();

    Direction cd = Direction::W;
    int actionPoints = 0;
    EnemyType type;

  virtual void onEvent(CommitEvent &e) override;
};

#endif // __ENEMY_H_
