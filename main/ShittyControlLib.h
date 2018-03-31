#ifndef __THINGY__
#define __THINGY__

#include <Arduino.h>
#include "MotorDriver.h"
extern MotorDriver motorDriver;

class ShittyControlLib {
public:
  ShittyControlLib(void);

  void gunTheMotorsForever(void);
};

#endif
