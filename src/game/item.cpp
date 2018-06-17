#include "lss/game/item.hpp"

Item::Item(ItemType t, std::string n): Object(), type(t), name(n) {};

bool Item::interact() {
    return false;
}
