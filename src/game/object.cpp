#include "lss/game/object.hpp"

int objCount = 0;
Object::Object() {
    objCount++;
    id = objCount;
}
Object::Object(std::vector<std::shared_ptr<Trigger>> ts): triggers(ts) {
    objCount++;
    id = objCount;
}
Object::Object(std::string n) : name(n) {
    objCount++;
    id = objCount;
}
