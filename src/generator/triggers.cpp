#include "EventBus.hpp"
#include "lss/game/cell.hpp"
#include "lss/game/location.hpp"
#include "lss/game/events.hpp"
#include "lss/game/player.hpp"

bool testTrigger(std::shared_ptr<Cell> c, std::shared_ptr<Location> location) {
    MessageEvent me(nullptr, "Suddenly, bloody runes enter the walls and floor slabs.");

    for (auto n : location->getNeighbors(c)) {
        n->features.push_back(CellFeature::BLOOD);
        n->invalidate();
        location->player->commit("trigger effect", 0);
    }
    eb::EventBus::FireEvent(me);
    return false;
}
