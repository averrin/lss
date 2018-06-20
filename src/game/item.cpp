#include "lss/game/item.hpp"

Item::Item(ItemSpec t): Object(), type(t) {};

bool Item::interact() {
    return false;
}
