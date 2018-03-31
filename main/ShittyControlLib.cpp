#include "ShittyControlLib.h"

ShittyControlLib::ShittyControlLib(void) {
}

void ShittyControlLib::gunTheMotorsForever(void) {
  for (int m = 0; m < NUM_MOTORS; m++) {
    motorDriver.motorValues[m] = 255;
    if (m == 1) {
      motorDriver.motorValues[m] = -255;
    }
  }
  motorDriver.apply();
  motorDriver.printState();
}
