#include "lss/game/cell.hpp"

    void Cell::updateIllumination() {
      auto d = lightSources.size();
      for (auto ls : lightSources) {
        d += sqrt(pow(x - ls->x, 2) + pow(y - ls->y, 2));
      }
      d /= lightSources.size();
      illumination = (TORCH_DISTANCE - d) / TORCH_DISTANCE * 110 + Cell::DEFAULT_LIGHT + 5;
      if (illumination < 1) {
        illumination = 1;
      } else if (illumination > 100){
          illumination = 100;
      }
    }
