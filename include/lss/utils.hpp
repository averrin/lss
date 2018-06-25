#ifndef __UTILS_H_
#define __UTILS_H_
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

using namespace std::string_literals;

class utils {
public:
  template <typename T>
  static std::string join(const T &array, const std::string &delimiter) {
    std::string res;
    for (auto &element : array) {
      if (!res.empty()) {
        res += delimiter;
      }

      res += element;
    }

    return res;
  }

  template <typename rT, typename iT>
  static std::vector<std::shared_ptr<rT>>
  castObjects(std::vector<std::shared_ptr<iT>> input) {
    std::vector<std::shared_ptr<rT>> result;
    for (auto input_object : input) {
      if (auto casted_object = std::dynamic_pointer_cast<rT>(input_object)) {
        result.push_back(casted_object);
      }
    }
    return result;
  }

  static std::vector<std::string> split(std::string strToSplit,
                                        char delimeter) {
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter)) {
      splittedStrings.push_back(item);
    }
    return splittedStrings;
  }

  static std::optional<Direction>
  getDirectionByName(const std::string &dirString) {
    if (dirString == "n"s) {
      return Direction::N;
    } else if (dirString == "e"s) {
      return Direction::E;
    } else if (dirString == "s"s) {
      return Direction::S;
    } else if (dirString == "w"s) {
      return Direction::W;
    } else if (dirString == "nw"s) {
      return Direction::NW;
    } else if (dirString == "ne"s) {
      return Direction::NE;
    } else if (dirString == "sw"s) {
      return Direction::SW;
    } else if (dirString == "se"s) {
      return Direction::SE;
    }
    return std::nullopt;
  }

  static std::string getDirectionName(Direction dir) {
    switch (dir) {
    case N:
      return "n"s;
      break;
    case E:
      return "e"s;
      break;
    case S:
      return "s"s;
      break;
    case W:
      return "w"s;
      break;
    case NW:
      return "nw"s;
      break;
    case NE:
      return "ne"s;
      break;
    case SW:
      return "sw"s;
      break;
    case SE:
      return "se"s;
      break;
    }
    return "";
  }
};

#endif // __UTILS_H_