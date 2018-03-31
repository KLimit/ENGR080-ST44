/********
Default E80 Lab 01
Current Author: Christopher McElroy (cmcelroy@g.hmc.edu) '19 (contributed in 2017)
Previous Contributors:  Josephine Wong (jowong@hmc.edu) '18 (contributed in 2016)
                        Apoorva Sharma (asharma@hmc.edu) '17 (contributed in 2016)
*/

/* Libraries */

// general
#include <Arduino.h>
#include <Wire.h>
#include <Pinouts.h>

// 80-specific
#include <SensorGPS.h>
#include <SensorIMU.h>
#include <ADCSampler.h>
#include <MotorDriver.h>
#include <Logger.h>
#include <Printer.h>


// template library
#include <LED.h>


/* Global Variables */

// Motors
MotorDriver motorDriver;

// IMU
SensorIMU imu;

// Logger
Logger logger;
bool keepLogging = true;

// Printer
Printer printer;

// Led
LED led;

// loop start recorder
int loopStartTime;




void setup() {
  printer.init();

  /* Initialize the Logger */
  logger.include(&imu);
  logger.include(&motorDriver);
  logger.init();

  /* Initialise the sensors */
  imu.init();

  /* Initialize motor pins */
  motorDriver.init();

  /* Done initializing, turn on LED */
  led.init();

  /* Keep track of time */
  printer.printMessage("Starting main loop",10);
  loopStartTime = millis();
}



void loop() {

  /* set the motors to run based on time since the loop started */
  /* loopStartTime is in units of ms */
  /* The motorDriver.drive function takes in 4 inputs arguments m1_power, m2_power, m3_power, m4_power: */
  /*       void motorDriver.drive(int m1_power, int m2_power, int m3_power, int m4_power) */
  /* the value of m!_power can range from -255 to 255 */
  /* Note: we typically avoid m3, it hasn't worked well in the past */

  /* Note that motorDriver.drive() takes PWM values for motors as follows:
  /*    LEFT, RIGHT, VERTICAL, X; or m1, m2, m4, m3 */

  int currentTime = millis() - loopStartTime; // current time in the loops
  
  obsCourseV0(currentTime);

  /* JUST AN EXAMPLE. USE FUNCTIONS CREATED BELOW.
  /* Adjust motor thrust based on timing */
//  if (currentTime > 4000 && currentTime <8000)
//    motorDriver.drive(0,120,0,0);
//  if(currentTime > 8000 && currentTime < 10000)
//    motorDriver.drive(256, 0, 0, 0);
//  else
//    motorDriver.drive(0,0,0,0);

  // motorDriver.driveDown(255);
  // motorDriver.driveForward(255);
  // motorDriver.driveUp(255);



  // DONT CHANGE CODE BELOW THIS LINE
  // --------------------------------------------------------------------------
  if (printer.loopTime(loopStartTime)) {
    printer.printToSerial();  // To stop printing, just comment this line out
  }

  if (imu.loopTime(loopStartTime)) {
    imu.read(); // this is a sequence of blocking I2C read calls
    imu.printState(); // a lot of random information
  }

  // uses the LED library to flash LED -- use this as a template for new libraries!
  if (led.loopTime(loopStartTime)) {
    led.flashLED();
  }

  if (logger.loopTime(loopStartTime) && keepLogging) {
    keepLogging = logger.log();
  }
}

void motorTest(int cTime) {
  // nothing right now
}



bool waiting(int st, int et, int ct) {
  /*
  Takes the current time (ct) and returns true when ct is between given start
  and end times (st, et).
  NOTE: TAKES TIME IN SECONDS, NOT MILLISECONDS.
  */
  return st < ct && ct < et;
}



void obsCourseV0(int cTime){
  /*
  Takes currentTime from loop() to time when motors should be on or off.
  Takes cTime in milliseconds and converts to seconds as ct
  */
  // NOTE: FOR NOW, WE USE drive(0, 0, 0, PWM) FOR VERTICAL THRUSTER.
  // WE WILL HAVE TO FIGURE OUT WHAT IS WRONG WITH OUR PINS LATER
  int ct = cTime / 1000; // converts the ms from loop() to s for waiting()
  if (waiting(0, 60, ct)) {
    motorDriver.stopDriving();
  } else if (waiting(60, 67, ct)) {
    motorDriver.drive(0, 0, 0, -255);
  } else if (waiting(67, 82, ct)) {
    motorDriver.drive(255, 255, 0, 0);
  } else if (waiting(82, 83, ct)) {
    motorDriver.drive(-180, -180, 0, 255);
  } else if (waiting(83, 88, ct)) {
    motorDriver.drive(0, 0, 0, 255);
  } else if (waiting(88, 89, ct)) {
    motorDriver.stopDriving();
  }
}
