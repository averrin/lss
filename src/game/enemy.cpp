#include "rang.hpp"
#include <chrono>

#include "EventBus.hpp"
#include "lss/game/costs.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/player.hpp"
#include "lss/generator/room.hpp"
#include "lss/utils.hpp"

Enemy::Enemy(EnemySpec t) : Creature(), type(t) {
  hp = type.baseHP;
  hp_max = type.baseHP;
  speed = type.baseSpeed;
  damage_dices = type.baseDamage_dices;
  damage_edges = type.baseDamage_edges;
  damage_modifier = type.baseDamage_modifier;

  name = type.name;
  traits = type.traits;
  defense = type.defense;

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
        // fmt::print("{} : {}\n", s->name, i->getTitle());
        equipment->equip(s, i->roll());
        break;
      }
    }
  }
}

Enemy::~Enemy() {
  if (registration != nullptr) {
    registration->removeHandler();
  }
}

// TODO: add probability for drops
std::optional<Items> Enemy::drop() {
  auto loot = type.loot.open();
  if (loot.size() == 0)
    return std::nullopt;
  return loot;
}

bool Enemy::interact(std::shared_ptr<Object> actor) {
  if (hasTrait(Traits::INVULNERABLE))
    return true;
  auto hero = std::dynamic_pointer_cast<Player>(actor);
  auto ptr = shared_from_this();
  if (hp > 0) {
    auto damage = hero->getDamage(shared_from_this());
    auto def = DEF(this);
    if (damage - def < 0) {
      damage = 0;
    } else {
      damage -= def;
    }
    hp -= damage;
    EnemyTakeDamageEvent e(ptr, damage);
    eb::EventBus::FireEvent(e);
  }
  if (hp <= 0) {
    passThrough = true;
    EnemyDiedEvent e2(ptr);
    eb::EventBus::FireEvent(e2);
  }
  return hp > 0;
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
  if (HP(this) <= 0) return;
  auto t0 = std::chrono::system_clock::now();
  auto hero = std::dynamic_pointer_cast<Player>(e.getSender());
  calcViewField();
  if (e.actionPoints == 0 ||
      (!canSee(hero->currentCell) && !hero->canSee(currentCell))) {
    // auto t1 = std::chrono::system_clock::now();
    // using milliseconds = std::chrono::duration<double, std::milli>;
    // milliseconds ms = t1 - t0;
    // std::cout << "onCommit ZERO: " << rang::fg::red << "enemy"
    //           << rang::style::reset << ": " << rang::fg::green << ms.count()
    //           << rang::style::reset << '\n';
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
      auto pather = new micropather::MicroPather(currentLocation.get());
      float totalCost = 0;
      pather->Reset();
      int result = pather->Solve(currentCell.get(), hero->currentCell.get(),
                                 &path, &totalCost);
      delete pather;
      if (result != micropather::MicroPather::SOLVED) {
        // TODO: who print this? fix it.
        fmt::print("cannot find path to hero [{}.{} -> {}.{}]\n",
                   currentCell->x, currentCell->y, hero->currentCell->x,
                   hero->currentCell->y);
        actionPoints = 0;
        return;
      }

      auto i = 1;
      auto nptr = path[i];
      auto nc = static_cast<Cell *>(nptr);
      auto cd = getDirFromCell(currentCell, nc);
      auto inShadow = !nc->illuminated;
      if (inShadow && hasTrait(Traits::SHADOW_RUNNER)) {
        stepCost /= 2;
      }

      auto n = 0;
      while (actionPoints >= stepCost && n < 2) {
        if (nc->x != hero->currentCell->x || nc->y != hero->currentCell->y) {
          if (move(cd)) {
            if (i < path.size() - 1) {
              i++;
              nptr = path[i];
              nc = static_cast<Cell *>(nptr);
              cd = getDirFromCell(currentCell, nc);
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
    // FIXME: crashes
    if (step >= path.size() - 1 || path.size() == 0 ||
        currentCell.get() == static_cast<Cell *>(path[path.size() - 1])) {
      if (!randomPath()) {
        return;
      }
    }

    fmt::print("path created: {}/{} [{}]\n", step, path.size(),
               currentCell.get() != static_cast<Cell *>(path[path.size() - 1]));
    if (path.size() >= 1 &&
        currentCell.get() != static_cast<Cell *>(path[path.size() - 1])) {
      auto nptr = path[step];
      auto nc = static_cast<Cell *>(nptr);
      fmt::print("NC: {}.{}\n", nc->x, nc->y);
      cd = getDirFromCell(currentCell, nc);

      while (actionPoints >= stepCost) {
        if (move(cd)) {
          if (step < path.size() - 1) {
            step++;
            nptr = path[step];
            nc = static_cast<Cell *>(nptr);
            cd = getDirFromCell(currentCell, nc);
          } else {
            fmt::print("clear path 1\n");
            randomPath();
            break;
          }
          actionPoints -= stepCost;
        } else {
          fmt::print("clear path\n");
          randomPath();
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
  if (currentCell->room == nullptr) {
    fmt::print("no room here\n");
    return false;
  }
  fmt::print("R: {}.{}\n", currentCell->room->x, currentCell->room->y);
  auto target = currentCell->room->cells[rand() % currentCell->room->height]
                                        [rand() % currentCell->room->width];
  fmt::print("T: {}.{}\n", target->x, target->y);
  auto pather = new micropather::MicroPather(currentLocation.get());
  float totalCost = 0;
  pather->Reset();
  int result =
      pather->Solve(currentCell.get(), target.get(), &path, &totalCost);
  delete pather;
  step = 1;
  if (result != micropather::MicroPather::SOLVED) {
    fmt::print("cannot find path\n");
    actionPoints = 0;
    return false;
  }
  return true;
}

EnemyTakeDamageEvent::EnemyTakeDamageEvent(eb::ObjectPtr s, int d)
    : eb::Event(s), damage(d) {}
EnemyDiedEvent::EnemyDiedEvent(eb::ObjectPtr s) : eb::Event(s) {}
