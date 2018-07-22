#ifndef __ATTRIBUTE_H_
#define __ATTRIBUTE_H_
#include "lss/game/object.hpp"

enum AttributeType {
  NONE,
  HP,
  HP_MAX,
  MP,
  MP_MAX,
  SPEED,
  VISIBILITY_DISTANCE,
  DAMAGE,
  DEFENSE,
  CRIT_CHANCE,
  TRAITS,
};

class Creature;
class Attribute : public Object {
public:
  Attribute(AttributeType t) : type(t) {}
  AttributeType type;
  float operator()(Creature *);
};

#endif // __ATTRIBUTE_H_
