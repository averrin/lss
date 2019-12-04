#include "lss/game/spell.hpp"
#include "lss/game/trigger.hpp"

void CellSpell::applyEffect(std::shared_ptr<Location> location,
                std::shared_ptr<Cell> c) {

if (spec == TerrainType::FROSTBALL) {
    auto fb = std::make_shared<Terrain>(spec, 8);
    fb->setCurrentCell(c);
    location->addObject(fb);
    c->addFeature(CellFeature::FROST);
    for (auto n : location->getNeighbors(c)) {
    n->addFeature(CellFeature::FROST);
    }
} else if (spec == TerrainType::FIREBALL) {
    auto fb = std::make_shared<Terrain>(spec, 8);
    fb->setCurrentCell(c);
    location->addObject(fb);
    c->removeFeature(CellFeature::FROST);
    for (auto n : location->getNeighbors(c)) {
    n->removeFeature(CellFeature::FROST);
    }
} else if (spec == TerrainType::ACIDPOOL) {
    auto fb = std::make_shared<Terrain>(spec, -5);
    fb->setCurrentCell(c);
    fb->triggers.push_back(std::make_shared<EnterTrigger>([=](std::shared_ptr<Creature> actor){
        return Triggers::ACID_POOL_TRIGGER(actor, location);
    }));
    location->addObject(fb);
    c->addFeature(CellFeature::ACID);
}

location->invalidate("apply spell effect");
}
