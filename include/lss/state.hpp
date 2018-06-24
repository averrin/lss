#ifndef __STATE_H_
#define __STATE_H_
#include "CinderPango.h"
#include "lss/fragment.hpp"
#include "lss/palette.hpp"
#include <string>
#include <vector>

class State {
public:
  Palette currentPalette;
  void render(kp::pango::CinderPangoRef);
  std::string renderStatus();

  void setContent(Fragments);
  void appendContent(Fragments);
  void appendContent(std::shared_ptr<Fragment>);
  void invalidate();

  Fragments fragments;

  static const Fragments normal_mode;
  static const Fragments hints_mode;
  static const Fragments leader_mode;
  static const Fragments insert_mode;
  static const Fragments direction_mode;
  static const Fragments object_select_mode;
  static const Fragments text_mode;

  static const Fragments unknown_command;

  static const Fragments START;
  static const Fragments END;
  static const Fragments END_LINE;

  static const Fragments HELP_HEADER;
  static const Fragments HELP;
  static const Fragments INVENTORY_HEADER;

private:
  bool damaged = true;
  std::string cache;
};

#endif // __STATE_H_
