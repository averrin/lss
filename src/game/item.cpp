#include "lss/game/item.hpp"

Item::Item(ItemType t): Object(), type(t) {};

bool Item::interact() {
    return false;
}
