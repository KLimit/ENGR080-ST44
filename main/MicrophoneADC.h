#include <Arduino.h>
#include "MotorDriver.h"
#include "DataSource.h"
#include "Pinouts.h"

/*
 * ADCSampler implements SD logging for the ADC channels
 */

 /* Pinmap, for reference, implemented in ADSampler.cpp
 Analog input name       Teensy Pin
 A0                      14
 A1                      15
 A7                      21
 A9                      23
 A10                     24
 A11                     25
 A12                     26
 */

#define NUM_PINS 2
#define BUFFER_SIZE 40960 //stores 5000 unsigned longs and 5000 unsigned chars
#define NUM_SAMPLES 8192
#define ADC_LOOP_INTERVAL 100 // ms
#define ADC_LOOP_OFFSET 0 // ms  I have ignored this part of the code because it sucks

class MicrophoneADC : public DataSource("time, voltage, envelopeStart", "unsigned long, unsigned char, unsigned char")
{
public:
  ADCSampler(void);

  // Managing state
  unsigned long times [NUM_SAMPLES];
  unsigned char analog [NUM_SAMPLES];
  unsigned long envelope;
  void updateSample(void);
  void printSample(void);

  // Write out
  size_t writeDataBytes(unsigned char * buffer, size_t idx);

  bool loopTime(int loopStartTime);

private:
  int lastLoopTime = -1;
};
