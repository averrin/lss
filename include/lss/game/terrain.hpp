#ifndef __TERRAIN_H_
#define __TERRAIN_H_
#include "lss/game/object.hpp"

struct TerrainSpec {
  std::string name;
  bool seeThrough;
  bool passThrough;

  int apLeft = -1;
  bool destructable = true;

  bool emitsLight = false;
  float lightStrength = 0;
  LightType lightType;
  bool lightStable = false;

  friend bool operator==(TerrainSpec &t1, const TerrainSpec &t2) {
    return t1.name == t2.name;
  }
  friend bool operator<(TerrainSpec &t1, const TerrainSpec &t2) {
    return t1.name < t2.name;
  }
  friend bool operator<(const TerrainSpec &lhs, const TerrainSpec &rhs) {
    return lhs.name < rhs.name;
  }
};

class Terrain : public Object {
public:
  Terrain(TerrainSpec t) : Object(), type(t) {
    seeThrough = t.seeThrough;
    passThrough = t.passThrough;
    zIndex = 2;
    emitsLight = t.emitsLight;
    lightStrength = t.lightStrength;
    lightType = t.lightType;
    apLeft = t.apLeft;
    lightStable = t.lightStable;
    destructable = t.destructable;
  }
  Terrain(TerrainSpec t, int z) : Object(), type(t) {
    seeThrough = t.seeThrough;
    passThrough = t.passThrough;
    zIndex = z;
    emitsLight = t.emitsLight;
    lightStrength = t.lightStrength;
    lightType = t.lightType;
    apLeft = t.apLeft;
    lightStable = t.lightStable;
    destructable = t.destructable;
  }
  TerrainSpec type;
};

namespace TerrainType {
const auto TORCH_STAND =
    TerrainSpec{"torch stand", false, true,           -1,
                true,          true,  TORCH_DISTANCE, LightType::FIRE};
const auto STATUE =
    TerrainSpec{"statue", false, false, -1, true, true, 1.5, LightType::MAGIC};
const auto ALTAR = TerrainSpec{"altar", true, true, -1, false};
const auto BUSH = TerrainSpec{"bush", false, true};
const auto FIREBALL = TerrainSpec{
    "fireball", true, true, 1, false, true, TORCH_DISTANCE, LightType::FIRE};
const auto ACIDBALL = TerrainSpec{
    "acidball", true, true, 1, false, true, TORCH_DISTANCE, LightType::ACID};
const auto MAGIC_LIGHT = TerrainSpec{
    "light", true, true, 1, false, true, 2.5, LightType::MAGIC, true};
const auto MAGIC_LIGHT_LONG = TerrainSpec{
    "light", true, true, 2500, false, true, 2.5, LightType::MAGIC, true};
const auto MAGIC_LIGHT_FOREVER = TerrainSpec{
    "light", true, true, -1, false, true, 2.5, LightType::MAGIC, true};
const auto ACID_LIGHT_FOREVER = TerrainSpec{
    "light", true, true, -1, false, true, 2.5, LightType::ACID, true};
}; // namespace TerrainType

#endif // __TERRAIN_H_
