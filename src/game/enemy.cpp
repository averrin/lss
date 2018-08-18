#include <chrono>
#include <rang.hpp>

#include "EventBus.hpp"
#include "lss/game/costs.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/player.hpp"
#include "lss/generator/room.hpp"
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
      std::make_shared<Slot>("--", std::vector<WearableType>{ENEMY}),
      std::make_shared<Slot>("--", std::vector<WearableType>{ENEMY}),
      std::make_shared<Slot>("--", std::vector<WearableType>{ENEMY}),
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

// TODO: refactor. Divide by actions;
void Enemy::onEvent(CommitEvent &e) {
  auto hero = std::dynamic_pointer_cast<Player>(e.getSender());
  if (HP(this) <= 0 || HP(hero.get()) <= 0)
    return;

  auto t0 = std::chrono::system_clock::now();
  calcViewField();
  if (e.actionPoints == 0 ||
      (!canSee(hero->currentCell) && !hero->canSee(currentCell))) {
    return;
  }

  actionPoints += e.actionPoints;

  // TODO: add lastheropoint and attack hero if its near
  auto stepCost = ap_cost::STEP / speed;
  auto attackCost = ap_cost::ATTACK / speed;
  auto waitCost = ap_cost::WAIT;
  fmt::print("Reaction [{}/{}]: step: {}, attack: {}, wait: {}\n",
             e.actionPoints, actionPoints, stepCost, attackCost, waitCost);

  // TODO: implement passive ai
  // fmt::print("{}\n", canSee(hero->currentCell));
  if (type.aiType == AIType::AGGRESSIVE &&
      hero->currentLocation == currentLocation && canSee(hero->currentCell)) {
    auto nbrs = currentLocation->getNeighbors(currentCell);
    if (std::find(nbrs.begin(), nbrs.end(), hero->currentCell) != nbrs.end()) {
      while (actionPoints >= attackCost) {
        auto cd = getDirFromCell(currentCell, hero->currentCell.get());
        attack(cd);
        actionPoints -= attackCost;
      }
    } else {
      path = currentLocation->getLine(currentCell, hero->currentCell);

      auto i = 1;
      auto nc = path[i];
      auto cd = getDirFromCell(currentCell, nc.get());
      auto inShadow = !nc->illuminated;
      if (inShadow && hasTrait(Traits::SHADOW_RUNNER)) {
        stepCost /= 2;
      }

      auto n = 0;
      while (actionPoints >= stepCost && n < 2) {
        if (nc->x != hero->currentCell->x || nc->y != hero->currentCell->y) {
          if (move(cd)) {
            if (i < int(path.size() - 1)) {
              i++;
              nc = path[i];
              cd = getDirFromCell(currentCell, nc.get());
            }
            actionPoints -= stepCost;
          } else {
            n++;
          }
        } else {
          attack(cd);
          actionPoints -= attackCost;
        }
      }
    }
  } else {
    if (step >= int(path.size() - 1) || path.size() == 0 ||
        currentCell == path[path.size() - 1]) {
    }
    if (path.size() >= 1 && currentCell != path[path.size() - 1]) {
      auto nc = path[step];
      cd = getDirFromCell(currentCell, nc.get());

      while (actionPoints >= stepCost) {
        if (move(cd)) {
          if (step < int(path.size() - 1)) {
            step++;
            nc = path[step];
            cd = getDirFromCell(currentCell, nc.get());
          } else {
            fmt::print("clear path 1\n");
            // randomPath();
            break;
          }
          actionPoints -= stepCost;
        } else {
          fmt::print("clear path\n");
          // randomPath();
          break;
        }
      }
    }

    while (actionPoints >= waitCost) {
      actionPoints -= waitCost;
    }
  }
  auto t1 = std::chrono::system_clock::now();
  using milliseconds = std::chrono::duration<double, std::milli>;
  milliseconds ms = t1 - t0;
  std::cout << "onCommit: " << rang::fg::red << "enemy" << rang::style::reset
            << ": " << rang::fg::green << ms.count() << rang::style::reset
            << '\n';
}

bool Enemy::randomPath() {
  // if (currentCell->room == nullptr) {
  //   fmt::print("no room here\n");
  //   return false;
  // }
  // fmt::print("R: {}.{}\n", currentCell->room->x, currentCell->room->y);
  // auto target =
  //     currentCell->room->cells[rand() % currentCell->room->cells.size()];
  // fmt::print("T: {}.{}\n", target->x, target->y);
  // auto pather = new micropather::MicroPather(currentLocation.get());
  // float totalCost = 0;
  // pather->Reset();
  // int result =
  //     pather->Solve(currentCell.get(), target.get(), &path, &totalCost);
  // delete pather;
  // step = 1;
  // if (result != micropather::MicroPather::SOLVED) {
  //   fmt::print("cannot find path\n");
  //   actionPoints = 0;
  //   return false;
  // }
  return true;
}

EnemyTakeDamageEvent::EnemyTakeDamageEvent(eb::ObjectPtr s,
                                           std::shared_ptr<Damage> d)
    : eb::Event(s), damage(d) {}
EnemyDiedEvent::EnemyDiedEvent(eb::ObjectPtr s) : eb::Event(s) {}
