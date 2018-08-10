#ifndef __WEAPONS_H_
#define __WEAPONS_H_

#include "lss/game/item.hpp"
#include "lss/game/content/specs.hpp"
#include "lss/game/content/spells.hpp"

namespace Prototype {
const auto SWORD = std::make_shared<Item>(
    ItemType::ONE_HAND,
    Effects{std::make_shared<MeleeDamage>(
        DamageSpec(R::I(-2, 2), R::I(2, 4), R::I(2, 4), DamageType::WEAPON))});
const auto GREAT_AXE = std::make_shared<Item>(
    ItemType::TWO_HAND, Effects{std::make_shared<MeleeDamage>(
                                     DamageSpec(-1, 6, 7, DamageType::WEAPON)),
                                 std::make_shared<SpeedModifier>(-0.3f)});
const auto DAGGER = std::make_shared<Item>(
    ItemType::LIGHT_WEAPON, Effects{std::make_shared<MeleeDamage>(DamageSpec(
                          R::I(-2, 2), 1, R::I(1, 3), DamageType::WEAPON))});
const auto THROWING_KNIVES =
    std::make_shared<Item>(ItemType::THROWING_KNIVES, 1,
                           Effects{std::make_shared<MeleeDamage>(
                               DamageSpec(0, 1, 3, DamageType::WEAPON))});
const auto POISON_DAGGER = std::make_shared<Item>(
    ItemType::LIGHT_WEAPON,
    Effects{
        std::make_shared<MeleeDamage>(DamageSpec(2, 3, 3, DamageType::WEAPON)),
        std::make_shared<CritModifier>(0.2),
        std::make_shared<OnHitEffect>(
            std::make_shared<LastingEffect>(OverTimeEffects::POISON, 5000),
            0.3)});





const auto ORK_SWORD = std::make_shared<Item>(
    ItemType::ONE_HAND, Effects{std::make_shared<MeleeDamage>(
                                 DamageSpec(5, 4, 6, DamageType::WEAPON)),
                             std::make_shared<CritModifier>(0.4)});
const auto GOBLIN_DAGGER = std::make_shared<Item>(
    ItemType::LIGHT_WEAPON, Effects{std::make_shared<MeleeDamage>(
                                  DamageSpec(2, 3, 3, DamageType::WEAPON)),
                              std::make_shared<CritModifier>(0.2)});
const auto POISON_FANG = std::make_shared<Item>(
    ItemType::ENEMY,
    Effects{
        std::make_shared<MeleeDamage>(DamageSpec(2, 3, 3, DamageType::WEAPON)),
        std::make_shared<CritModifier>(0.2),
        std::make_shared<OnHitEffect>(
            std::make_shared<LastingEffect>(OverTimeEffects::POISON, 5000),
            0.3)});
}


#endif // __WEAPONS_H_
