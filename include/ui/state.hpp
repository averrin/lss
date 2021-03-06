#ifndef __STATE_H_
#define __STATE_H_
#include "ui/fragment.hpp"
#include "ui/palette.hpp"
#include <SDLPango.hpp>
#include <TextGrid.hpp>
#include <string>
#include <vector>
#include <liblog/liblog.hpp>

struct Position {
  int x;
  int y;
};

struct Selection {
  Position pos;
  std::string color;
};

class State {
  LibLog::Logger &log = LibLog::Logger::getInstance();
public:
  Palette currentPalette;
  std::string render();
  std::string renderFragment(std::shared_ptr<Fragment>);
  void render(pango::SurfaceRef);
  void render(std::shared_ptr<TextGrid>);
  std::string renderStatus();

  void setContent(Fragments);
  void appendContent(Fragments);
  void appendContent(std::shared_ptr<Fragment>);
  void setFragment(int, std::shared_ptr<Fragment>);
  void invalidateSelection(std::string reason);
  void invalidate(std::string reason);
  void invalidate(std::string reason, int index);
  void invalidate();
  void setSelect(bool);
  void clear();

  Fragments fragments;
  Position cursor;
  void setCursor(Position c) {
    cursor = c;
    invalidate();
  }
  std::vector<Selection> selection;
  void setSelection(Selection s) {
    selection.push_back(s);
    invalidate();
  }
  void selectionClear() {
    invalidateSelection("clear");
    selection.clear();
  }

  int width;
  int height;
  bool select = false;

  std::shared_ptr<CellSign> US =
      std::make_shared<CellSign>(std::make_shared<Cell>(CellType::UNKNOWN));

  static const Fragments normal_mode;
  static const Fragments hints_mode;
  static const Fragments leader_mode;
  static const Fragments insert_mode;
  static const Fragments direction_mode;
  static const Fragments object_select_mode;
  static const Fragments text_mode;
  static const Fragments pause_mode;
  static const Fragments target_mode;
  static const Fragments inspect_mode;

  static const Fragments unknown_command;

  static const Fragments START;
  static const Fragments END;
  static const Fragments END_LINE;

  static const Fragments HELP_HEADER;
  static const Fragments HELP;
  static const Fragments GAMEOVER;
  static const Fragments INVENTORY_HEADER;

  bool damaged = true;

  // private:
  std::string cache;
};

#endif // __STATE_H_
