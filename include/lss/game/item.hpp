#ifndef __ITEM_H_
#define __ITEM_H_
#include "lss/game/object.hpp"

enum ItemType {
    CORPSE,
};

class Item : public Object {
public:
    Item(ItemType);
    ItemType type;

    bool interact();
};


#endif // __ITEM_H_
