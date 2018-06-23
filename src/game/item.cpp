#include "lss/game/item.hpp"

Item::Item(ItemSpec t) : Object(), type(t){};
Item::Item(ItemSpec t, Effects e) : Object(), type(t), effects(e){};

bool Item::interact(std::shared_ptr<Object> actor) { return false; }
