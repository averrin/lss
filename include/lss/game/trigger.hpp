#ifndef __TRIGGER_H_
#define __TRIGGER_H_
#include <functional>
#include <memory>

#include "lss/game/itemSpec.hpp"
#include "lss/game/lootBox.hpp"
#include "EventBus.hpp"
#include "lss/game/cell.hpp"
#include "lss/game/location.hpp"
#include "lss/game/events.hpp"
#include "lss/game/damage.hpp"
#include "lss/game/player.hpp"
#include "lss/game/content/items.hpp"

typedef std::function<bool(std::shared_ptr<Creature>)> TriggerCallback;
class Trigger {
    TriggerCallback callback;
public:
        Trigger(TriggerCallback cb) : callback(cb) {}
        bool enabled = true;
        void activate(std::shared_ptr<Creature> actor) {
            if (enabled) {
                enabled = callback(actor);
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
class PickTrigger : public Trigger {
    public: PickTrigger(TriggerCallback cb) : Trigger(cb) {}
};
class UseTrigger : public Trigger {
    public: UseTrigger(TriggerCallback cb) : Trigger(cb) {}
};
class InteractTrigger : public Trigger {
    public: InteractTrigger(TriggerCallback cb) : Trigger(cb) {}
};
class AttackTrigger : public Trigger {
    public: AttackTrigger(TriggerCallback cb) : Trigger(cb) {}
};
class UseItemTrigger : public Trigger {
    public:
        UseItemTrigger(ItemSpec i, TriggerCallback cb) : Trigger(cb), item(i) {}
        ItemSpec item;
};


namespace Triggers {
    const auto CRATE_TRIGGER = [](std::shared_ptr<Creature> c, std::shared_ptr<Terrain> terrain, std::shared_ptr<Location> location) {
        for (auto r : location->cells) {
            for (auto cell : r) {
                auto objects = location->getObjects(cell);
                if (std::find_if(objects.begin(), objects.end(), [=](auto o){
                    return o->id == terrain->id;
                }) != objects.end()) {
                    //TODO: change lootbox
                    auto box = LootBoxes::CLUTTER;
                    for (auto item : box.open()) {
                        item->setCurrentCell(cell);
                        location->addObject<Item>(item);
                    }
                    location->removeObject(terrain);
                    location->invalidate();
                    return false;
                }
            }
        }

            return false;
    };
    const auto CRATE_MOVE_TRIGGER = [](std::shared_ptr<Creature> actor, std::shared_ptr<Terrain> terrain, std::shared_ptr<Location> location) {
        for (auto r : location->cells) {
            for (auto cell : r) {
                auto objects = location->getObjects(cell);
            if (std::find_if(objects.begin(), objects.end(), [=](auto o){
                return o->id == terrain->id;
            }) != objects.end()) {
                auto dir = location->getDirFromCell(cell, actor->currentCell);
                auto nc = location->getCell(cell, *dir);
                if (location->getObjects(*nc).size() > 0) {
                    return true;
                }

                fmt::print("move {}: {}.{} -> {}.{}\n", utils::getDirectionName(*dir), cell->x, cell->y, (*nc)->x, (*nc)->y);
                terrain->setCurrentCell(*nc);
                location->invalidate();
                return true;

            }
            }
        }
            return true;
    };
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

            n->triggers.push_back(std::make_shared<EnterTrigger>([=](std::shared_ptr<Creature> actor){
                return TEST_ENTER_TRIGGER(n, location);
            }));
        }
        location->player->commit("trigger effect", 0);
        return false;
    };

    const auto TORCH_STAND_TRIGGER = [](std::shared_ptr<Player> hero, std::shared_ptr<Terrain> terrain) {
        hero->currentLocation->removeObject(terrain);
        hero->pick(Prototype::TORCH->clone());
        hero->currentLocation->invalidate();
        hero->currentLocation->updateLight(hero);
        hero->commit("trigger effect", 0);
        return false;
    };

    const auto HEAL_STAND_TRIGGER = [](std::shared_ptr<Player> hero, std::shared_ptr<UsableTerrain> terrain) {
        hero->heal(50, 100);

        auto light = *hero->currentLocation->getObject<Terrain>("heal_light");
        hero->currentLocation->removeObject(light);
        hero->currentLocation->invalidate();
        hero->currentLocation->updateLight(hero);
        hero->commit("trigger effect", 0);
        return false;
    };

    //TODO: fix glow from removed light source
    const auto MANA_STAND_TRIGGER = [](std::shared_ptr<Player> hero, std::shared_ptr<UsableTerrain> terrain) {
        hero->restoreMana(25, 50);

        auto light = *hero->currentLocation->getObject<Terrain>("mana_light");
        hero->currentLocation->removeObject(light);
        hero->currentLocation->invalidate();
        hero->currentLocation->updateLight(hero);
        hero->commit("trigger effect", 0);
        return false;
    };

    const auto ACID_POOL_TRIGGER = [](std::shared_ptr<Creature> c, std::shared_ptr<Location> location) {
        fmt::print("apt start\n");

        auto damage = DamageSpec(0, 2, 6, DamageType::ACID);
        c->applyDamage(
          std::dynamic_pointer_cast<Object>(location),
          damage.getDamage(1));
        location->player->commit("trigger effect", 0);
        fmt::print("apt end\n");
        return true;
    };
}


#endif // __TRIGGER_H_
