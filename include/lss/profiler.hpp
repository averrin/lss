#ifndef __PROFILER_H_
#define __PROFILER_H_
#include "lss/utils.hpp"
#include <chrono>
#include <fmt/format.h>
#include <limits>
#include <map>
#include <rang.hpp>
#include <string>

using milliseconds = std::chrono::duration<double, std::milli>;

class Profiler {
  std::map<std::string, std::chrono::time_point<std::chrono::system_clock>>
      _start;
  std::map<std::string, std::string> _aliases;

  std::string getOffset() {
    std::string offset = "";
    for (auto n = 0; n < _start.size() - 1; n++) {
      offset += "| ";
    }
    return offset;
  }
  std::string getName(std::string label) {

    auto pName = name;
    if (_aliases.find(label) != _aliases.end()) {
      pName = _aliases[label];
    }
    return utils::cyan(fmt::format(" {:6}", pName));
  }

public:
  std::string name;
  Profiler(std::string n) : name(n) {}
  void start(std::string alias, std::string label, bool silent = false) {
    _aliases[label] = alias;
    start(label, silent);
  }
  void start(std::string label, bool silent = false) {
    _start[label] = std::chrono::system_clock::now();
    if (silent)
      return;
    std::cout << getName(label) << " >> " << getOffset() << "["
              << utils::blue(label) << "] : " << utils::yellow("started")
              << std::endl;
  }
  void stop(std::string label, float b = 0) {
    stop(label, utils::blue(label), b);
  }
  void stop(std::string label, std::string msg, float b = 0) {
    auto start = _start[label];
    milliseconds ms = std::chrono::system_clock::now() - start;
    if (ms.count() < b) {
      _start.erase(label);
      return;
    }
    std::cout << getName(label) << " >> " << getOffset() << "[" << msg
              << "] : " << rang::fg::green << rang::style::bold << ms.count()
              << rang::style::reset << utils::yellow("ms") << std::endl;
    _start.erase(label);
  }
};

const auto T = std::make_unique<Profiler>("ROOT");

#endif // __PROFILER_H_
