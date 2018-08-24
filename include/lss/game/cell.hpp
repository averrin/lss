#ifndef __CELL_H_
#define __CELL_H_
#include "lss/game/content/traits.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <set>
#include <vector>

const float TORCH_DISTANCE = 4.5f;

class Room;
class Object;
struct CellSpec {
  std::string name;

  bool passThrough = false;
  bool seeThrough = false;

  std::vector<Trait> canPassTraits;

  friend bool operator==(CellSpec &t1, const CellSpec &t2) {
    return t1.name == t2.name;
  }
  friend bool operator!=(CellSpec &t1, const CellSpec &t2) {
    return t1.name != t2.name;
  }
  friend bool operator<(const CellSpec &t1, const CellSpec &t2) {
    return t1.name < t2.name;
  }
};

namespace CellType {
const CellSpec UNKNOWN = CellSpec{"unknown", false, false};
const CellSpec FLOOR = CellSpec{"floor", true, true};
const CellSpec WALL = CellSpec{"wall", false, false};
const CellSpec DOWNSTAIRS = CellSpec{"downstairs", true, true};
const CellSpec UPSTAIRS = CellSpec{"upstairs", true, false};
const CellSpec WATER =
    CellSpec{"water", false, true, {Traits::FLY, Traits::CAN_SWIM}};
const CellSpec VOID = CellSpec{"void", false, true, {Traits::FLY}};
}; // namespace CellType

enum class VisibilityState { UNKNOWN, SEEN, VISIBLE };
enum class CellFeature { BLOOD, CAVE, FROST, MARK1, MARK2 };

class Cell {
public:
  Cell(CellSpec t) : type(t) {}
  Cell(int _x, int _y, CellSpec t)
      : x(_x), y(_y), type(t), passThrough(type.passThrough),
        seeThrough(type.seeThrough) {}
  Cell(int _x, int _y, CellSpec t, std::vector<CellFeature> f)
      : x(_x), y(_y), type(t), features(f), passThrough(type.passThrough),
        seeThrough(type.seeThrough) {}
  CellSpec type;
  VisibilityState visibilityState = VisibilityState::UNKNOWN;
  bool illuminated = false;
  std::vector<CellFeature> features;

  std::set<std::shared_ptr<Object>> lightSources;
  std::shared_ptr<Object> nearestLightEmitter;
  std::shared_ptr<Room> room;

  int x;
  int y;

  bool passThrough = false;
  bool seeThrough = false;

  bool damaged = true;
  void invalidate() { damaged = true; }
  void setVisibilityState(VisibilityState vs) {
    if (visibilityState != vs) {
      visibilityState = vs;
      invalidate();
    }
  }

  bool canPass(std::vector<Trait> traits) {
    if (passThrough)
      return true;
    std::sort(traits.begin(), traits.end());
    std::sort(type.canPassTraits.begin(), type.canPassTraits.end());

    std::vector<Trait> v_intersection;

    std::set_intersection(traits.begin(), traits.end(),
                          type.canPassTraits.begin(), type.canPassTraits.end(),
                          std::back_inserter(v_intersection));
    return v_intersection.size() != 0;
  }

  bool hasFeature(CellFeature f) {
    return std::find(features.begin(), features.end(), f) != features.end();
  }
  void addFeature(CellFeature f) { features.push_back(f); }
  void removeFeature(CellFeature f) {
    if (std::find(features.begin(), features.end(), f) != features.end()) {
      features.erase(std::remove(features.begin(), features.end(), f));
    }
  }
  static const int DEFAULT_LIGHT = 60;
  static const int MINIMUM_LIGHT = 5;
  int illumination = DEFAULT_LIGHT;
};

typedef std::vector<std::shared_ptr<Cell>> CellRow;
typedef std::vector<CellRow> Cells;

#endif // __CELL_H_
