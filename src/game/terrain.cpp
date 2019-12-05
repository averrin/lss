#include "lss/game/terrain.hpp"
#include "lss/game/trigger.hpp"
#include "lss/game/creature.hpp"

bool Terrain::interact(std::shared_ptr<Creature> actor) {
    for (auto t : triggers) {
        auto trigger = std::dynamic_pointer_cast<InteractTrigger>(t);
        if (trigger) {
            trigger->activate(actor);
        }
    }

    return type.passThrough;
}
