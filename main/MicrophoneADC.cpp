#include "MicrophoneADC.h"
#include <math.h>
#include "Printer.h"
extern Printer printer;

MicrophoneADC::MicrophoneADC(void)
  : DataSource("time, AX",
               "int,int")
{}



/*
The idea of this function is to take 256 samples of data in a single buffer before writing.
Each sample may be identified by a time in milliseconds, as well as its analogRead value, which should be a value in
between 0 to 255 volts.
*/
void MicrophoneADC::updateSample()
{
    //Edit this line below-- get correct pin, should be array size 1
  const int pinmap[NUM_PINS] = {14,15,21,23,24,25,26}; // see pin map in MicrophoneADC.h

//   Creates a buffer array with the first node being time, the second node being the analog value.
  for(int i = 0; i < 256; i++){

    sample[0][i] = millis();
    sample[1][i] = analogRead(pinmap);
  }
}

void MicrophoneADC::printSample(void)
{
  String printString = "ADC:";
  printString += " ";
  printString += String(sample[1]);
  printer.printValue(0, printString);
}


// This writes data into an input buffer array.
size_t MicrophoneADC::writeDataBytes(unsigned char * buffer, size_t idx)
{
  int * data_slot = (int *) &buffer[idx];
  int j = 0;  // index
  bufferSize = 256;
  for (int i=0; i<bufferSize; i++) {
    data_slot[0][i] = sample[0][i];
    data_slot[1][i] = sample[1][i];
  }
  return idx + BUFFER_SIZE;
}


//This function is currently not being used.
bool MicrophoneADC::loopTime(int loopStartTime) {
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
