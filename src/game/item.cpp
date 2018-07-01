#include "lss/game/item.hpp"
#include "lss/utils.hpp"

Item::Item(ItemSpec t)
    : Object(), type(t), name(t.name), durability(type.durability){};
Item::Item(ItemSpec t, int c)
    : Object(), type(t), count(c), name(t.name), durability(type.durability){};
Item::Item(ItemSpec t, Effects e)
    : Object(), type(t), effects(e), name(t.name),
      durability(type.durability){};

bool Item::interact(std::shared_ptr<Object> actor) { return false; }

std::string Item::getFullTitle() {
  return fmt::format("{}{}", count == 0 ? "" : fmt::format("{} ", count),
                     getTitle());
}

std::string Item::getTitle() {
  std::vector<std::string> effectNames;
  std::vector<std::string> specialPostfix;
  std::vector<std::string> specialPrefix;
  if (effects.size() != 0) {
    for (auto e : effects) {
      if (std::dynamic_pointer_cast<SpecialPrefix>(e)) {
        specialPrefix.push_back(e->getTitle());
      } else if (std::dynamic_pointer_cast<SpecialPostfix>(e) || e->special) {
        specialPostfix.push_back(e->getTitle());
      } else {
        if (e->special)
          continue;
        effectNames.push_back(e->getTitle());
      }
    }
  }
  return fmt::format(
      "{}{}{}{}{}",
      specialPrefix.size() == 0
          ? ""
          : fmt::format("{} ", utils::join(specialPrefix, " ")),
      type.name,
      specialPostfix.size() == 0
          ? ""
          : fmt::format(" {}", utils::join(specialPostfix, " ")),
      effectNames.size() == 0
          ? ""
          : fmt::format(" {{{}}}", utils::join(effectNames, " ,")),
      durability == -1
          ? ""
          : fmt::format(" &lt;{}&gt;", durability != 0
                                           ? fmt::format("{}", durability)
                                           : "broken"));
}
