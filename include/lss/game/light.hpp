#ifndef __LIGHT_H_
#define __LIGHT_H_

enum class LightType { NONE, CLEAR, FIRE, MAGIC, ACID, FROST };

struct LightSpec {
    float distance;
    LightType type;
    bool stable = false;

};

namespace Glow {
    const LightSpec NONE = {0, LightType::NONE};
}

#endif // __LIGHT_H_
