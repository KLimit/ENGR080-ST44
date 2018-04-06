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
  const int micPin = 14;
  const int envelopePin = 15;
  bool envelopeTriggered = false;
//   Creates a buffer array with the first node being time, the second node being the analog value.
  for(int i = 0; i < NUM_SAMPLES; i++){

    times[i] = micros(); //micros() should return an unsigned long
    analog[i] = (unsigned char) analogRead(pinmap);

    int high = analogRead(envelopePin);
    if(high >150 && envelopeTriggered == false){
      envelope = micros();
      envelopeTriggered = true;
    } 
  }
}


//How do i even write this for my thing
// void MicrophoneADC::printSample(void)
// {
//   String printString = "ADC:";
//   printString += " ";
//   printString += String(sample[1]);
//   printer.printValue(0, printString);
// }


// This writes data into an input buffer array.
void MicrophoneADC::writeDataBytes(unsigned long * timeBuffer, unsigned char * analogBuffer)
{
  unsigned long * data_slot1 = (unsigned long *) &timeBuffer[0];
  unsigned char * data_slot2 = (unsigned char *) &analogBuffer[0];
  for (int i=0; i< NUM_SAMPLES; i++) {
    data_slot1[i] = times[i];
    data_slot2[i] = analog[i];
  }
  // return idx + BUFFER_SIZE;
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
