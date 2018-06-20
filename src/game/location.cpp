#include <algorithm>
#include <memory>

#include "lss/game/location.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/player.hpp"
#include "EventBus.hpp"

Location::Location() {}

void Location::onEvent(EnemyDiedEvent & e) {
    auto sender = e.getSender();
    if (auto enemy = std::dynamic_pointer_cast<Enemy>(sender)) {
        objects.push_back(enemy->drop());
    }
    objects.erase(std::remove(objects.begin(), objects.end(), sender), objects.end());   
}

void Location::onEvent(ItemTakenEvent & e) {
    objects.erase(std::remove(objects.begin(), objects.end(), e.item), objects.end());   
}

void Location::onEvent(DigEvent & e) {
    e.cell->type = CellType::FLOOR;
    e.cell->passThrough = true;
    e.cell->seeThrough = true;

    auto rock = std::make_shared<Item>(ItemType::ROCK);
    rock->currentCell = e.cell;
    objects.push_back(rock);

    if(auto cell = cells[e.cell->y-1][e.cell->x]; cell->type == CellType::UNKNOWN_CELL) cell->type = CellType::WALL;
    if(auto cell = cells[e.cell->y][e.cell->x+1]; cell->type == CellType::UNKNOWN_CELL) cell->type = CellType::WALL;
    if(auto cell = cells[e.cell->y+1][e.cell->x]; cell->type == CellType::UNKNOWN_CELL) cell->type = CellType::WALL;
    if(auto cell = cells[e.cell->y][e.cell->x-1]; cell->type == CellType::UNKNOWN_CELL) cell->type = CellType::WALL;
    if(auto cell = cells[e.cell->y-1][e.cell->x-1]; cell->type == CellType::UNKNOWN_CELL) cell->type = CellType::WALL;
    if(auto cell = cells[e.cell->y+1][e.cell->x+1]; cell->type == CellType::UNKNOWN_CELL) cell->type = CellType::WALL;
    if(auto cell = cells[e.cell->y+1][e.cell->x-1]; cell->type == CellType::UNKNOWN_CELL) cell->type = CellType::WALL;
    if(auto cell = cells[e.cell->y+1][e.cell->x+1]; cell->type == CellType::UNKNOWN_CELL) cell->type = CellType::WALL;
}

ItemsFoundEvent::ItemsFoundEvent(eb::ObjectPtr s, Objects i): eb::Event(s), items(i) {}
void Location::onEvent(EnterCellEvent & e) {
    Objects items(objects.size());
    auto it = std::copy_if(objects.begin(), objects.end(), items.begin(), [e](std::shared_ptr<Object> o){
        return std::dynamic_pointer_cast<Item>(o) && o->currentCell == e.cell;
    });
    items.resize(std::distance(items.begin(), it));
    if (items.size() > 0) {
        ItemsFoundEvent ie(nullptr, items);
        eb::EventBus::FireEvent(ie);
    }
    auto hero = std::dynamic_pointer_cast<Player>(e.getSender());
    updateView(hero);
}

void Location::updateView(std::shared_ptr<Player> hero) {
    for (auto r : cells) {
        for (auto c : r) {
            if (hero->canSee(c)) {
                c->visibilityState = VisibilityState::VISIBLE;
            } else if (c->visibilityState == VisibilityState::VISIBLE) {
                c->visibilityState = VisibilityState::SEEN;
            }
        }
    }
}
