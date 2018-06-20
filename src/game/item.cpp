#include "lss/game/item.hpp"

Item::Item(ItemSpec t) : Object(), type(t){};

bool Item::interact(std::shared_ptr<Object> actor) { return false; }
