#include "lss/game/cell.hpp"

void Cell::invalidate(std::string reason) {
  log.info(lu::yellow("CELL"),
           fmt::format("cell ({}.{}) invalidate [{}]",
                       lu::green(fmt::format("{}", x)),
                       lu::green(fmt::format("{}", y)),
                       lu::magenta(reason)));
  invalidate();
}
