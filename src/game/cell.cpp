#include "lss/game/cell.hpp"
#include "lss/logger.hpp"

void Cell::invalidate(std::string reason) {
  L().info(utils::yellow("CELL"),
           fmt::format("cell ({}.{}) invalidate [{}]",
                       utils::green(fmt::format("{}", x)),
                       utils::green(fmt::format("{}", y)),
                       utils::magenta(reason)));
  invalidate();
}
