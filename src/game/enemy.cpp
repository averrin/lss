#include <chrono>
#include <rang.hpp>

#include "EventBus.hpp"
#include "lss/game/costs.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/player.hpp"
#include "lss/generator/room.hpp"
#include "lss/utils.hpp"

Enemy::Enemy(EnemySpec t) : Creature(), type(t) {
  zIndex = 2;

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

std::optional<AiAction> Enemy::execAiNone(int ap) { return WaitAction(ap); }

std::optional<AiAction> Enemy::execAiPassive(int ap) { return WaitAction(ap); }

// TODO: add magic cast and pause mode
// TODO: use traits wisely
std::optional<AiAction> Enemy::execAiAggressive(int ap) {
  auto stepCost = ap_cost::STEP / speed;
  auto attackCost = ap_cost::ATTACK / speed;
  auto waitCost = ap_cost::WAIT;
  auto throwCost = ap_cost::THROW / speed;

  auto s = lastAiState;
  viewField = s.viewField;
  for (auto c : viewField) {
    c->lightSources.insert(shared_from_this());
  }

  if (s.exit) {
    return IdleAction(ap);
  }
  path = s.path;

  if (ap >= attackCost) {
    if (s.nearTarget) {
      auto directionToTarget = getDirFromCell(currentCell, s.targetCell.get());
      attack(directionToTarget);
      return AttackAction(attackCost);
    }
  }
  if (ap >= throwCost) {
    if (!s.nearTarget && s.targetInTargetCell && s.canThrow) {
      auto t = std::find_if(inventory.begin(), inventory.end(), [](auto i) {
        return i->type.category == ItemCategories::THROWABLE;
      });
      throwItem(*t, s.targetCell);
      return ThrowAction(throwCost);
    }
  }

  if (ap >= stepCost) {
    if (!s.nearTarget && s.canReachTarget) {
      auto nextCell = path[1];
      auto direction = getDirFromCell(currentCell, nextCell.get());
      move(direction);
      return MoveAction(stepCost);
    }
  }

  if (ap >= waitCost) {
    return WaitAction(waitCost);
  }
  return std::nullopt;
}

std::optional<AiAction> Enemy::execAction() {
  auto hero = currentLocation->player;
  if (actionPoints == 0 || HP(this) <= 0 || HP(hero.get()) <= 0)
    return std::nullopt;

  std::string label = fmt::format("react [{}]", getId());
  log.start(lu::red("ENEMY"), label, true);
  auto action = execAction(actionPoints);
  if (action) {
    log.info(lu::red("ENEMY"),
             fmt::format("action: {}, cost: {}", action->name,
             lu::yellow(fmt::format("{}", action->cost))));
    commit(action->cost);
  }
  log.stop(label);
  return action;
}

std::optional<AiAction> Enemy::execAction(int ap) {
  switch (type.aiType) {
  case AIType::NONE:
    return execAiNone(ap);
  case AIType::PASSIVE:
    return execAiPassive(ap);
  case AIType::AGGRESSIVE:
    return execAiAggressive(ap);
  }

  if (ap >= ap_cost::WAIT) {
    return WaitAction(ap_cost::WAIT);
  }
  return std::nullopt;
}

void Enemy::prepareAiState() {
  if (type.aiType == AIType::AGGRESSIVE) {
    auto target = currentLocation->player;
    if (lastTarget == nullptr || lastTarget != target) {
      lastTarget = target;
    }
    lastAiState = getAiState(target);
  }
}

void Enemy::onEvent(CommitEvent &e) {}

bool Enemy::randomPath() { return true; }

EnemyTakeDamageEvent::EnemyTakeDamageEvent(eb::ObjectPtr s,
                                           std::shared_ptr<Damage> d)
    : eb::Event(s), damage(d) {}
EnemyDiedEvent::EnemyDiedEvent(eb::ObjectPtr s) : eb::Event(s) {}
