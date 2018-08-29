#include "lss/logger.hpp"

void Logger::stop(std::string label, std::string msg, float b) {
  auto start = _start.at(label);
  milliseconds ms = std::chrono::system_clock::now() - start;
  if (ms.count() < b) {
    _start.erase(label);
    return;
  }
  auto time = utils::green(fmt::format("{}", ms.count()));
  if (ms.count() > 50) {
    time = utils::style(rang::style::bold,
                        utils::redBg(fmt::format("{}", ms.count())));
  }
  fmt::print(" {} ≫ {}[{}] ⊶  {}{}\n", getName(label), getOffset(), msg, time,
             utils::yellow("ms"));
  _start.erase(label);
  _aliases.erase(label);
}

std::string Logger::fill(std::string m, int c) {
  std::regex esc_re("\033\\[\\d{1,2}m", std::regex::ECMAScript);
  std::string stripped = std::regex_replace(m, esc_re, "");
  int d = int(c) - int(stripped.size());
  if (d <= 0) {
    return m;
  }
  std::string offset = "";
  for (auto n = 0; n < d; n++) {
    offset += " ";
  }
  return m + offset;
}

Logger &L() { return Logger::getInstance(); }
