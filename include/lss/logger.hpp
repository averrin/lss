#ifndef __LOGGER_H_
#define __LOGGER_H_
#include "lss/utils.hpp"
#include <chrono>
#include <fmt/format.h>
#include <limits>
#include <map>
#include <rang.hpp>
#include <regex>
#include <string>

using milliseconds = std::chrono::duration<double, std::milli>;

class Logger {
  std::map<std::string, std::chrono::time_point<std::chrono::system_clock>>
      _start;
  std::map<std::string, std::string> _aliases;

  std::string getOffset(int d = 0) {
    std::string offset = "";
    for (auto n = 0; n < d; n++) {
      offset += "| ";
    }
    if (_start.size() == 0) {
      return offset;
    }
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
    return fill(pName, 6);
  }

  std::string fill(std::string m, int c);

public:
  static Logger &getInstance() {
    static Logger instance("ROOT");
    return instance;
  }

private:
  Logger(std::string n) : name(n) {}

public:
  Logger(Logger const &) = delete;
  void operator=(Logger const &) = delete;

public:
  std::string name;
  void warn(std::string msg) { warn(name, msg); }
  void warn(std::string alias, std::string msg) {
    fmt::print(" {} ≫ {}[{}] ⊸  {}\n", fill(alias, 6), getOffset(1),
               utils::yellowBg(utils::black("WARN")), msg);
  }
  void info(std::string msg) { info(name, msg); }
  void info(std::string alias, std::string msg) {
    fmt::print(" {} ≫ {}[{}] ⊸  {}\n", fill(alias, 6), getOffset(1),
               utils::blueBg(utils::black("INFO")), msg);
  }
  void error(std::string msg) { error(name, msg); }
  void error(std::string alias, std::string msg) {
    fmt::print(" {} ≫ [{}] ⊸  {}{}\n", fill(alias, 6), getOffset(1),
               utils::redBg("!ERR"), msg);
  }
  void start(std::string alias, std::string label, bool silent = false) {
    _aliases[label] = alias;
    start(label, silent);
  }
  void start(std::string label, bool silent = false) {
    _start[label] = std::chrono::system_clock::now();
    if (silent)
      return;
    fmt::print(" {} ≫ {}[{}] ⊷  {}\n", getName(label), getOffset(), label,
               utils::yellow("start"));
  }
  void stop(std::string label, float b = 0) { stop(label, label, b); }
  void stop(std::string label, std::string msg, float b = 0);
};

Logger &L();

#endif // __LOGGER_H_
