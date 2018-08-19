#ifndef __NORMALMODE_H_
#define __NORMALMODE_H_
#include "lss/modes.hpp"
#include "lss/keyEvent.hpp"

class NormalMode : public Mode {
public:
  NormalMode(LSSApp *app) : Mode(app){};
  bool processKey(KeyEvent e);
};



#endif // __NORMALMODE_H_
