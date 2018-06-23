#include "lss/game/item.hpp"

Item::Item(ItemSpec t) : Object(), type(t){};
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

std::string Item::getTitle() {
  std::vector<std::string> effectNames;
  std::vector<std::string> specialNames;
  if (effects.size() != 0) {
    for (auto e : effects) {
      if (e->special)
        continue;
      effectNames.push_back(e->getTitle());
    }
    for (auto e : effects) {
      if (!e->special)
        continue;
      specialNames.push_back(e->getTitle());
    }
  }
  return fmt::format("{}{}{}", type.name,
                     specialNames.size() == 0
                         ? ""
                         : fmt::format(" {}", join_e(specialNames, " ")),
                     effectNames.size() == 0
                         ? ""
                         : fmt::format(" {{{}}}", join_e(effectNames, " ,")));
}
