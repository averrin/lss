#include "lss/game/door.hpp"

Door::Door() {
    passThrough = false;
    seeThrough = false;
};

bool Door::interact() {
    if (!opened) {
        opened = true;
        passThrough = true;
        seeThrough = true;
        return true;
    }
    return false;
}
