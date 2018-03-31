#include "ADCSampler.h"
#include <math.h>
#include "Printer.h"
extern Printer printer;

ADCSampler::ADCSampler(void)
  : DataSource("A00,A01,A07,A09,A10,A11,A12",
               "int,int,int,int,int,int,int")
{}


void ADCSampler::updateSample(void)
{
  const int pinmap[NUM_PINS] = {14,15,21,23,24,25,26}; // see pin map in ADCSampler.h
  for (int i=0; i<NUM_PINS; i++){
    sample[i] = analogRead(pinmap[i]);
  }
}

void ADCSampler::printSample(void)
{
  String printString = "ADC:";
  for (int i=0; i<NUM_PINS; i++) {
    printString += " ";
    printString += String(sample[i]);
  }
  printer.printValue(0, printString);
}

size_t ADCSampler::writeDataBytes(unsigned char * buffer, size_t idx)
{
  int * data_slot = (int *) &buffer[idx];
  for (int i=0; i<NUM_PINS; i++) {
    data_slot[i] = sample[i];
  }
  return idx + NUM_PINS*sizeof(int);
}

bool ADCSampler::loopTime(int loopStartTime) {
  int currentTime = millis();
  if (lastLoopTime == -1) {
    lastLoopTime = loopStartTime-ADC_LOOP_INTERVAL+ADC_LOOP_OFFSET;
  }
  if (currentTime - lastLoopTime >= ADC_LOOP_INTERVAL) {
    lastLoopTime = currentTime;
    return true;
  }
  return false;
}
