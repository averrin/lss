#ifndef __INSPECTMODE_H_
#define __INSPECTMODE_H_
#include "lss/keyEvent.hpp"
#include "lss/modes.hpp"

class InspectMode : public Mode {
public:
  InspectMode(std::shared_ptr<LSSApp> app) : Mode(app){};
  bool processKey(KeyEvent e);
  void render();

  bool showRooms = false;
  bool showLightSources = false;
  bool highlightWhoCanSee = false;
};

#endif // __INSPECTMODE_H_
