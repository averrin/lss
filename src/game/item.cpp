#include "lss/game/item.hpp"

Item::Item(ItemSpec t) : Object(), type(t){};
Item::Item(ItemSpec t, int c) : Object(), type(t), count(c){};
Item::Item(ItemSpec t, Effects e) : Object(), type(t), effects(e){};

bool Item::interact(std::shared_ptr<Object> actor) { return false; }

template <typename T>
std::string join_e(const T &array, const std::string &delimiter) {
  std::string res;
  for (auto &element : array) {
    if (!res.empty()) {
      res += delimiter;
    }

    res += element;
  }

  return res;
}

std::string Item::getFullTitle() {
  return fmt::format("{}{}",
        count == 0 ? "" : fmt::format("{} ", count), getTitle()
    );
}

std::string Item::getTitle() {
  std::vector<std::string> effectNames;
  std::vector<std::string> specialPostfix;
  std::vector<std::string> specialPrefix;
  if (effects.size() != 0) {
    // TODO: extract
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
      "{}{}{}{}",
      specialPrefix.size() == 0 ? ""
                               : fmt::format("{} ", join_e(specialPrefix, " ")),
      type.name,
      specialPostfix.size() == 0 ? ""
                               : fmt::format(" {}", join_e(specialPostfix, " ")),
      effectNames.size() == 0
          ? ""
          : fmt::format(" {{{}}}", join_e(effectNames, " ,")));
}
