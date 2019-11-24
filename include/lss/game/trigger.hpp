#ifndef __TRIGGER_H_
#define __TRIGGER_H_
#include <functional>
#include <memory>

#include "lss/game/itemSpec.hpp"
#include "EventBus.hpp"
#include "lss/game/cell.hpp"
#include "lss/game/location.hpp"
#include "lss/game/events.hpp"
#include "lss/game/player.hpp"
#include "lss/game/content/items.hpp"

typedef std::function<bool()> TriggerCallback;
class Trigger {
    TriggerCallback callback;
public:
        Trigger(TriggerCallback cb) : callback(cb) {}
        bool enabled = true;
        void activate() {
            if (enabled) {
                enabled = callback();
            }
        }
    virtual ~Trigger() = default;
};

class EnterTrigger : public Trigger {
    public: EnterTrigger(TriggerCallback cb) : Trigger(cb) {}
};
class SearchTrigger : public Trigger {
    public: SearchTrigger(TriggerCallback cb) : Trigger(cb) {}
};
class SeenTrigger : public Trigger {
    public: SeenTrigger(TriggerCallback cb) : Trigger(cb) {}
};
class UseTrigger : public Trigger {
    public:
        UseTrigger(ItemSpec i, TriggerCallback cb) : Trigger(cb), item(i) {}
        ItemSpec item;
};


namespace Triggers {
    const auto BUSH_TRIGGER = [](std::shared_ptr<Cell> c, std::shared_ptr<Location> location) {
        for (auto t : utils::castObjects<Terrain>(location->objects)) {
            if (t->type == TerrainType::BUSH && c == t->currentCell) {
            location->removeObject(t);
            auto grass = Prototype::GRASS->clone();
            grass->setCurrentCell(c);
            location->addObject(grass);
            return false;
            }
        }
        return true;
    };

    const auto TEST_ENTER_TRIGGER = [](std::shared_ptr<Cell> c, std::shared_ptr<Location> location) {
        MessageEvent me(nullptr, "Whoops, blood pools everywhere.");
        eb::EventBus::FireEvent(me);

        return true;
    };

    const auto ALTAR_TRIGGER = [](std::shared_ptr<Cell> c, std::shared_ptr<Location> location) {
        auto statue = *location->getObject<Terrain>("statue");
        auto rock = Prototype::ROCK->clone();
        rock->setCurrentCell(statue->currentCell);
        location->removeObject(statue);
        location->addObject<Item>(rock);
        MessageEvent me(nullptr, "Suddenly, bloody runes enter the walls and floor slabs.");
        eb::EventBus::FireEvent(me);

        c->features.push_back(CellFeature::BLOOD);
        c->invalidate();
        for (auto n : location->getNeighbors(c)) {
            n->features.push_back(CellFeature::BLOOD);
            n->invalidate();

            n->triggers.push_back(std::make_shared<EnterTrigger>([=]{
                return TEST_ENTER_TRIGGER(n, location);
            }));
        }
        location->player->commit("trigger effect", 0);
        return false;
    };
}

#endif // __TRIGGER_H_
