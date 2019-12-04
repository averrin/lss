#ifndef __NORMALMODE_H_
#define __NORMALMODE_H_
#include "ui/keyEvent.hpp"
#include "ui/modes.hpp"

class NormalMode : public Mode {
public:
  NormalMode(std::shared_ptr<LSSApp> app) : Mode(app){};
  bool processKey(KeyEvent e);
};

#endif // __NORMALMODE_H_
