#include <chrono>
#include <rang.hpp>

#include "EventBus.hpp"
#include "lss/game/costs.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/player.hpp"
#include "lss/generator/room.hpp"
#include "lss/logger.hpp"
#include "lss/utils.hpp"

Enemy::Enemy(EnemySpec t) : Creature(), type(t) {
  hp = type.baseHP;
  hp_max = type.baseHP;
  mp = type.baseMP;
  mp_max = type.baseMP;
  speed = type.baseSpeed;
  dmgSpec = type.dmgSpec;

  name = type.name;
  traits = type.traits;
  defense = type.defense;
  activeEffects = type.activeEffects;

  equipment = std::make_shared<Equipment>();
  equipment->slots = {
      std::make_shared<Slot>(
          "Hand", std::vector<WearableType>{WEAPON, WEAPON_LIGHT,
                                            WEAPON_TWOHANDED, SHIELD}),
      std::make_shared<Slot>(
          "Hand", std::vector<WearableType>{WEAPON, WEAPON_LIGHT,
                                            WEAPON_TWOHANDED, SHIELD}),
      std::make_shared<Slot>("Body", std::vector<WearableType>{BODY}),
      std::make_shared<Slot>("Light", std::vector<WearableType>{LIGHT}),
      std::make_shared<Slot>("Special", std::vector<WearableType>{ENEMY}),
      std::make_shared<Slot>("Special", std::vector<WearableType>{ENEMY}),
      std::make_shared<Slot>("Special", std::vector<WearableType>{ENEMY}),
  };
  for (auto i : type.equipped) {
    inventory.push_back(i);
    for (auto s : equipment->slots) {
      if (std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                    i->type.wearableType) != s->acceptTypes.end()) {
        equipment->equip(s, i->roll());
        break;
      }
    }
  }
}

Enemy::~Enemy() { clearHandlers(); }

// TODO: add probability for drops
std::optional<Items> Enemy::drop() {
  auto loot = type.loot.open();
  if (loot.size() == 0)
    return std::nullopt;
  return loot;
}

// TODO: move to creature
bool Enemy::interact(std::shared_ptr<Object> actor) {
  if (hasTrait(Traits::INVULNERABLE))
    return true;
  auto hero = std::dynamic_pointer_cast<Player>(actor);
  for (auto s : equipment->slots) {
    if (s->item == nullptr ||
        std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                  s->item->type.wearableType) == s->acceptTypes.end()) {
      continue;
    }
    for (auto e : s->item->effects) {
      if (auto ohe = std::dynamic_pointer_cast<OnHitEffect>(e)) {
        if (R::R() < ohe->probability) {
          hero->activeEffects.push_back(ohe->effect);
        }
      }
    }
  }
  auto damage = hero->getDamage(shared_from_this());
  hero->increaseStrength(damage->damage);
  applyDamage(hero, damage);
  return hp > 0;
}

void Enemy::onDamage(std::shared_ptr<Creature> attacker,
                     std::shared_ptr<Damage> damage) {
  auto ptr = shared_from_this();
  EnemyTakeDamageEvent e(ptr, damage);
  eb::EventBus::FireEvent(e);
}

void Enemy::onDie() {
  auto ptr = shared_from_this();
  passThrough = true;
  EnemyDiedEvent e2(ptr);
  eb::EventBus::FireEvent(e2);
}

Direction getDirFromCell(std::shared_ptr<Cell> c, Cell *nc) {
  if (nc->y < c->y) {
    if (nc->x < c->x) {
      return NW;
    } else if (nc->x == c->x) {
      return N;
    } else {
      return NE;
    }
  }
  if (nc->y == c->y) {
    if (nc->x < c->x) {
      return W;
    } else {
      return E;
    }
  }
  if (nc->x < c->x) {
    return SW;
  } else if (nc->x == c->x) {
    return S;
  } else {
    return SE;
  }
};

void Enemy::commit(int ap) { actionPoints -= ap; }

std::optional<int> Enemy::execAiNone(int ap) {
  std::optional<int> cost;
  if (ap >= ap_cost::WAIT) {
    cost = ap_cost::WAIT;
  }
  return cost;
}

std::optional<int> Enemy::execAiPassive(int ap) {
  std::optional<int> cost;
  if (ap >= ap_cost::WAIT) {
    cost = ap_cost::WAIT;
  }
  return cost;
}

// TODO: add magic cast and pause mode
// TODO: use traits wisely
std::optional<int> Enemy::execAiAggressive(int ap) {
  std::string label = "exec action";
  std::optional<int> cost;
  auto stepCost = ap_cost::STEP / speed;
  auto attackCost = ap_cost::ATTACK / speed;
  auto waitCost = ap_cost::WAIT;
  auto throwCost = ap_cost::THROW / speed;

  auto target = currentLocation->player;
  if (lastTarget == nullptr || lastTarget != target) {
    lastTarget = target;
  }
  auto s = getAiState(target);
  if (s.exit) {
    return cost;
  }
  L().start(utils::red("ENEMY"), label);
  path = s.path;

  if (!cost && ap >= attackCost) {
    if (s.nearTarget) {
      auto directionToTarget = getDirFromCell(currentCell, s.targetCell.get());
      attack(directionToTarget);
      cost = attackCost;
      L().info(utils::red("ENEMY"), "attack");
    }
  }
  if (!cost && ap >= throwCost) {
    if (!s.nearTarget && s.targetInTargetCell && s.canThrow) {
      L().info(utils::red("ENEMY"), "throw");
      auto t = std::find_if(inventory.begin(), inventory.end(), [](auto i) {
        return i->type.category == ItemCategories::THROWABLE;
      });
      throwItem(*t, s.targetCell);
    }
  }

  if (!cost && ap >= stepCost) {
    if (!s.nearTarget && s.canReachTarget) {
      auto nextCell = path[1];
      auto direction = getDirFromCell(currentCell, nextCell.get());
      move(direction);
      cost = stepCost;
      L().info(utils::red("ENEMY"), "move");
    }
  }

  if (!cost && ap >= waitCost) {
    cost = waitCost;
      L().info(utils::red("ENEMY"), "wait");
  }
  L().stop(label, 20.f);
  return cost;
}

std::optional<int> Enemy::execAction(int ap) {
  std::optional<int> cost;

  switch (type.aiType) {
  case AIType::NONE:
    return execAiNone(ap);
  case AIType::PASSIVE:
    return execAiPassive(ap);
  case AIType::AGGRESSIVE:
    return execAiAggressive(ap);
  }

  if (ap >= ap_cost::WAIT) {
    cost = ap_cost::WAIT;
  }
  return cost;
}

// TODO: refactor. Divide by actions;
void Enemy::onEvent(CommitEvent &e) {
  auto hero = std::dynamic_pointer_cast<Player>(e.getSender());
  if (HP(this) <= 0 || HP(hero.get()) <= 0)
    return;

  std::string label = fmt::format("react [{}@{}.{}]", utils::magenta(name),
                                  currentCell->x, currentCell->y);
  L().start(utils::red("ENEMY"), label, true);
  calcViewField();
  if (e.actionPoints == 0) {
    L().stop(label, 5.f);
    return;
  }

  auto ap = actionPoints + e.actionPoints;
  auto spent = 0;

  while (auto cost = execAction(ap)) {
    ap -= *cost;
    spent += *cost;
  }
  commit(spent);
  if (spent > 0) {
    actionPoints = ap;
  }
  L().stop(label, 5.f);
}

bool Enemy::randomPath() { return true; }

EnemyTakeDamageEvent::EnemyTakeDamageEvent(eb::ObjectPtr s,
                                           std::shared_ptr<Damage> d)
    : eb::Event(s), damage(d) {}
EnemyDiedEvent::EnemyDiedEvent(eb::ObjectPtr s) : eb::Event(s) {}
