/* DUMMY CLASS! USE AS A TEMPLATE FOR NEW LIBRARIES! */

// this includes everything from the header file
#include "LED.h"

// this allows you to use print calls
// (see IMU or ADC libraries for good examples)
#include "Printer.h"
extern Printer printer;

// constructor for class objects
LED::LED(void) {
}

void LED::init(void) {
  pinMode(LOOP_LED,OUTPUT); // LOOP_LED is defined in pinouts

  // this lets you print messages
  // the string gets displayed in the messages area
  // the number counts the number of times it is displayed at 10 Hz
  printer.printMessage("Initialized LED at " + String(millis()), 10);
}

void LED::flashLED(void) {
  if (LED_State) {
    digitalWrite(LOOP_LED,HIGH);  // pulls the LED pin high
  } else {
    digitalWrite(LOOP_LED,LOW);   // or low
  }
  LED_State = !LED_State;
}

// tells the main loop when to activate
bool LED::loopTime(int loopStartTime) {
  int currentTime = millis();
  if (lastLoopTime == -1) {
    lastLoopTime = loopStartTime-LED_LOOP_INTERVAL+LED_LOOP_OFFSET;
  }
  if (currentTime - lastLoopTime >= LED_LOOP_INTERVAL) {
    lastLoopTime = currentTime;
    return true;
  }
  return false;
}
