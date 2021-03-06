#ifndef __TERRAIN_H_
#define __TERRAIN_H_
#include "lss/game/object.hpp"

class Creature;
struct TerrainSpec {
  std::string name;
  bool seeThrough;
  bool passThrough;

  int apLeft = -1;
  bool destructable = true;

  LightSpec light;

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
    light = t.light;
    apLeft = t.apLeft;
    destructable = t.destructable;
  }
  Terrain(TerrainSpec t, std::vector<std::shared_ptr<Trigger>> ts) : Object(ts), type(t) {
    seeThrough = t.seeThrough;
    passThrough = t.passThrough;
    zIndex = 2;
    light = t.light;
    apLeft = t.apLeft;
    destructable = t.destructable;
  }
  Terrain(TerrainSpec t, int z) : Object(), type(t) {
    seeThrough = t.seeThrough;
    passThrough = t.passThrough;
    zIndex = z;
    light = t.light;
    apLeft = t.apLeft;
    destructable = t.destructable;
  }
  TerrainSpec type;

  bool interact(std::shared_ptr<Creature> actor);

  std::optional<LightSpec> getGlow() override {
    if (light.distance <= 0) {
      return std::nullopt;
    }
    return std::make_optional<LightSpec>(light);
  };
};

class UsableTerrain: public Terrain {
  public:
    UsableTerrain(TerrainSpec t) : Terrain(t) {}
};

namespace TerrainType {
const auto DRILL = TerrainSpec{
    "drill", true, true, 1, false, LightSpec{TORCH_DISTANCE, LightType::ACID}};
const auto DARKNESS = TerrainSpec{"darkness", false, true, 5000, false};
const auto TORCH_STAND =
    TerrainSpec{"torch stand", false,
                true,          -1,
                true,          LightSpec{TORCH_DISTANCE, LightType::FIRE}};
const auto STATUE = TerrainSpec{
    "statue", false, false, -1, true, LightSpec{1.5, LightType::MAGIC}};
const auto ALTAR = TerrainSpec{"altar", true, true, -1, false};
const auto CRATE = TerrainSpec{"crate", true, false};
const auto BUSH = TerrainSpec{"bush", false, true};
const auto FIREBALL =
    TerrainSpec{"fireball", true,  true,
                1,          false, LightSpec{TORCH_DISTANCE, LightType::FIRE}};
const auto FROSTBALL = TerrainSpec{
    "frostball", true,  true,
    1,           false, LightSpec{TORCH_DISTANCE, LightType::FROST}};
const auto ACIDBALL =
    TerrainSpec{"acidball", true,  true,
                1,          false, LightSpec{TORCH_DISTANCE, LightType::ACID}};
const auto ACIDPOOL =
    TerrainSpec{"acidpool", true,  true,
                -1,          false, LightSpec{1.5, LightType::ACID}};
const auto MAGIC_LIGHT = TerrainSpec{
    "light", true, true, 1, false, LightSpec{2.5, LightType::MAGIC, true}};
const auto MAGIC_LIGHT_LONG = TerrainSpec{
    "light", true, true, 2500, false, LightSpec{2.5, LightType::MAGIC, true}};
const auto MAGIC_LIGHT_FOREVER = TerrainSpec{
    "magic_light_forever", true, true, -1, false, LightSpec{2.5, LightType::MAGIC, true}};
const auto ACID_LIGHT_FOREVER = TerrainSpec{
    "acid_light_forever", true, true, -1, false, LightSpec{2.5, LightType::ACID, true}};
}; // namespace TerrainType

#endif // __TERRAIN_H_
