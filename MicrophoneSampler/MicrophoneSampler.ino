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
#include<MicrophoneADC.h>
#define mySerial Serial1


/* Global Variables */

// Microphone
MicrophoneADC mic;





// Logger
Logger generalLogger;  // gps, imu, etc.
Logger micLogger;  // microphone analog and time ONLY
bool keepLogging = true;

// Printer
Printer printer;

// Led
LED led;

// loop start recorder
int loopStartTime;
int current_way_point = 0;



void setup() {
  printer.init();

  /* Initialize the Logger */
  logger.include(&mic);
  logger.init();



  mySerial.begin(9600);


  /* Done initializing, turn on LED */
  led.init();

  /* Keep track of time */
  printer.printMessage("Starting the microphone", 10);

  mic.updateSample();
  logger.log();

  printer.printMessage("Data recorded!")
}





void loop() {





  // uses the LED library to flash LED -- use this as a template for new libraries!
  // if (led.loopTime(loopStartTime)) {
  //   led.flashLED();
  // }

  // while (logger.loopTime(loopStartTime) && keepLogging) {
  //   mic.updateSample();
  //   keepLogging = logger.log();
  // }

}


void sampleMic() {
	// eventually will be latency time + expected travel time + pulse time +
	// headspace
	micLogger.init();
	int sampleLength = 2000;
	int startTime = millis();
	while (loopTime <= sampleLength) {
		mic.updateSample();
		micLogger.log(); // can't do this it takes too long
	}
}
