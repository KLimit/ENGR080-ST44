/* DUMMY CLASS! USE AS A TEMPLATE FOR NEW LIBRARIES! */

// only define this class if it hasn't been before
#ifndef __LED_H__
#define __LED_H__

// include relevant libraries, always good to have these two
#include <Arduino.h>
#include "Pinouts.h"

// controls how often and when in the loop this class's functions run
#define LED_LOOP_INTERVAL 500 // ms
#define LED_LOOP_OFFSET 50 // ms

class LED {
public: // for functions outside code might call
  LED(void);

  void init(void);

  void flashLED(void);

  bool loopTime(int loopStartTime);

private: // for helper functions and hidden variables
  bool LED_State = false;

  int lastLoopTime = -1;
};

#endif
