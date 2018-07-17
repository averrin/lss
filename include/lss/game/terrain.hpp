#ifndef __TERRAIN_H_
#define __TERRAIN_H_
#include "lss/game/object.hpp"

struct TerrainSpec {
  std::string name;
  bool seeThrough;
  bool passThrough;

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
  }
  TerrainSpec type;
};

namespace TerrainType {
const auto TORCH_STAND = TerrainSpec{"torch stand", false, true};
const auto STATUE = TerrainSpec{"statue", false, false};
const auto ALTAR = TerrainSpec{"altar", true, true};
const auto BUSH = TerrainSpec{"bush", false, true};
}; // namespace TerrainType

#endif // __TERRAIN_H_
