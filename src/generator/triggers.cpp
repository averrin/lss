#include "EventBus.hpp"
#include "lss/game/cell.hpp"
#include "lss/game/location.hpp"
#include "lss/game/events.hpp"
#include "lss/game/player.hpp"

bool testEnterTrigger(std::shared_ptr<Cell> c, std::shared_ptr<Location> location) {
    MessageEvent me(nullptr, "Whoops, blood pools everywhere.");
    eb::EventBus::FireEvent(me);

    return true;
}

bool testTrigger(std::shared_ptr<Cell> c, std::shared_ptr<Location> location) {
    MessageEvent me(nullptr, "Suddenly, bloody runes enter the walls and floor slabs.");
    eb::EventBus::FireEvent(me);

    c->features.push_back(CellFeature::BLOOD);
    c->invalidate();
    for (auto n : location->getNeighbors(c)) {
        n->features.push_back(CellFeature::BLOOD);
        n->invalidate();

        n->triggers.push_back(std::make_shared<EnterTrigger>([=]{
          return testEnterTrigger(n, location);
        }));
    }
    location->player->commit("trigger effect", 0);
    return false;
}

