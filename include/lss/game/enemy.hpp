#ifndef __ENEMY_H_
#define __ENEMY_H_

#include "Event.hpp"
#include "EventBus.hpp"
#include "lss/game/creature.hpp"
#include "lss/game/item.hpp"

struct EnemySpec {
public:
    std::string name;
    float baseSpeed;
    int baseHP;
    int baseDamage;

	friend bool operator<(const EnemySpec& lhs, const EnemySpec& rhs)
	{
		return lhs.name < rhs.name;
	}
};

namespace EnemyType {
    EnemySpec const GOBLIN = {"goblin", 0.5, 5};
    EnemySpec const ORK =    {"ork", 1, 15};
    EnemySpec const PIXI =   {"pixi", 2, 1};
}


class Enemy : public Creature,
              public eb::EventHandler<CommitEvent>
{
public:
  Enemy(EnemySpec);
  ~Enemy();
  eb::HandlerRegistrationPtr registration;
  bool interact(std::shared_ptr<Object>) override;
  std::shared_ptr<Item> drop();

    Direction cd = Direction::W;
    int actionPoints = 0;
    EnemySpec type;

  virtual void onEvent(CommitEvent &e) override;
};

#endif // __ENEMY_H_
