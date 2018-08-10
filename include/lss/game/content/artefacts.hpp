#ifndef __ARTEFACTS_H_
#define __ARTEFACTS_H_

#include "lss/game/item.hpp"
#include "lss/game/content/specs.hpp"
#include "lss/game/content/spells.hpp"

namespace Prototype {
const auto BOOTS_OF_FLY =
    std::make_shared<Item>("flying boots", ItemType::BOOTS,
                           Effects{std::make_shared<TraitEffect>(Traits::FLY)});


const auto SPEED_RING =
    std::make_shared<Item>("ring of lightning", ItemType::RING,
                           Effects{std::make_shared<SpeedModifier>(0.3)});


const auto SWORD_OF_DUALWIELD = std::make_shared<Item>(
    "sword of dualwield", ItemType::ONE_HAND,
    Effects{std::make_shared<MeleeDamage>(DamageSpec(
                R::I(-2, 2), R::I(2, 4), R::I(2, 4), DamageType::WEAPON)),
            std::make_shared<TraitEffect>(Traits::DUAL_WIELD)});
}


#endif // __ARTEFACTS_H_
