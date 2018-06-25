#include "lss/game/enemy.hpp"
#include "EventBus.hpp"
#include "lss/game/player.hpp"
#include "lss/utils.hpp"

Enemy::Enemy(EnemySpec t) : Creature(), type(t) {
  hp = type.baseHP;
  hp_max = type.baseHP;
  speed = type.baseSpeed;
  damage_dices = type.baseDamage_dices;
  damage_edges = type.baseDamage_edges;
  damage_modifier = type.baseDamage_modifier;
}

Enemy::~Enemy() { registration->removeHandler(); }

std::optional<std::shared_ptr<Item>> Enemy::drop() {
  // auto item = std::make_shared<Item>(ItemType::CORPSE);
  if (type.loot.size() == 0)
    return std::nullopt;
  auto item = type.loot.front();
  item->currentCell = currentCell;
  return item;
}

bool Enemy::interact(std::shared_ptr<Object> actor) {
  auto hero = std::dynamic_pointer_cast<Player>(actor);
  auto ptr = shared_from_this();
  if (hp > 0) {
    auto damage = hero->getDamage(shared_from_this());
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
  } else if (nc->y == c->y) {
    if (nc->x < c->x) {
      return W;
    } else {
      return E;
    }
  } else {
    if (nc->x < c->x) {
      return SW;
    } else if (nc->x == c->x) {
      return S;
    } else {
      return SE;
    }
  }
};

void Enemy::onEvent(CommitEvent &e) {
  if (e.actionPoints == 0)
    return;
  actionPoints += e.actionPoints;

  // TODO: add lastheropoint and attack hero if its near
  auto stepCost = 1000 / speed;
  auto attackCost = 500 / speed;

  auto pather = new micropather::MicroPather(currentLocation.get());
  float totalCost = 0;
  pather->Reset();
  auto hero = std::dynamic_pointer_cast<Player>(e.getSender());
  int result = pather->Solve(currentCell.get(), hero->currentCell.get(), &path,
                             &totalCost);
  delete pather;
  if (result != micropather::MicroPather::SOLVED) {
    fmt::print("cannot find path\n");
    actionPoints = 0;
    return;
  }

  auto i = 1;
  auto nptr = path[i];
  auto nc = static_cast<Cell *>(nptr);
  auto cd = getDirFromCell(currentCell, nc);

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

EnemyTakeDamageEvent::EnemyTakeDamageEvent(eb::ObjectPtr s, int d)
    : eb::Event(s), damage(d) {}
EnemyDiedEvent::EnemyDiedEvent(eb::ObjectPtr s) : eb::Event(s) {}
