#include "lss/game/item.hpp"
#include "lss/utils.hpp"

bool Item::interact(std::shared_ptr<Object> actor) { return false; }

std::string Item::getFullTitle() {
  return fmt::format("{}{}", count == 0 ? "" : fmt::format("{} ", count),
                     getTitle());
}

std::string Item::getTitle(bool force) {
  std::vector<std::string> effectNames;
  std::vector<std::string> specialPostfix;
  std::vector<std::string> specialPrefix;
  for (auto e : effects) {
    if (e->special) {
      specialPostfix.push_back(e->getTitle());
    } else {
      effectNames.push_back(e->getTitle());
    }
  }
  return fmt::format(
      "{}{}{}{}", (identified || force) ? name : unidName,
      specialPostfix.size() == 0
          ? ""
          : fmt::format(" {}", utils::join(specialPostfix, " ")),
      effectNames.size() == 0 || (!identified && !force)
          ? ""
          : fmt::format(" {{{}}}", utils::join(effectNames, " ,")),
      durability == -1
          ? ""
          : fmt::format(" &lt;{}&gt;", durability != 0
                                           ? fmt::format("{}", durability)
                                           : "broken"));
}
