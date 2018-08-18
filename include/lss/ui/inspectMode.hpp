#ifndef __INSPECTMODE_H_
#define __INSPECTMODE_H_
#include "lss/modes.hpp"
#include "lss/keyEvent.hpp"

class InspectMode : public Mode {
public:
  InspectMode(LSSApp *app) : Mode(app){};
  bool processKey(KeyEvent e);
  void render();
};



#endif // __INSPECTMODE_H_
