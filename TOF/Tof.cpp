#include "Tof.h"
#include <Printer.h>
extern Printer printer;

Tof::Tof(void)
  : DataSource(String("time_of_flight,distance"), String("int,int"))
{}

void Tof::init(void) {
  pinMode(SPEAKER_PIN,OUTPUT);
  pinMode(MIC_PIN,INPUT);
}

void Tof::updateTOF(void) {
  int timer = millis();
  reset();  // reset variables
  printer.printValue(4,"Reset: " + String(millis()-timer));
  Tx();     // send signal
  printer.printValue(5,"Tx: " + String(millis()-timer));
  Rx();     // receive signal
  printer.printValue(6,"Rx: " + String(millis()-timer));
  //getTOF();  // find return signal
  printer.printValue(7,"Get TOF: " + String(millis()-timer));
  getDistance(); // determine distance
  String tofValue = "TOF: Flight time = " + String(flightTime) + "µs." +
  " Dist from obj = " + String(distance,3) + "m.";
  printer.printValue(3,tofValue);
}

void Tof::reset(void) {
  currentDist = MAX_DIST; // can change to make call faster
  currentWaitTime = (int) MICROS_PER_S * currentDist / SPEED_OF_SOUND_W_; // µs
  flightTime = -1;
  digitalWrite(SPEAKER_PIN,LOW);

  for (int i = 0; i < maxWaitTime; i++) {
    noData[i] = true;
  }
}

void Tof::Tx(void) {
  // Transmit pulses until finished
  int startTime;
  int elapsedTime = 0;
  startTime = micros();
  while (elapsedTime < signalTime) {
    if (elapsedTime % toggleTime == 0) {
      togglePin();
    }
    elapsedTime = micros() - startTime;
  }
}

void Tof::Rx(void) {
  int startTime;
  int elapsedTime = 0;
  startTime = micros();
  while(elapsedTime < currentWaitTime) {
    // if (elapsedTime % toggleTime == 0 && elapsedTime < signalTime) {
    //   togglePin();
    // }
    if (noData[elapsedTime]) {
      micData[elapsedTime] = (digitalRead(MIC_PIN) == HIGH);
      noData[elapsedTime] = false;
    }
    elapsedTime = micros() - startTime;
  }
}

void Tof::getTOF(void) {
  // this is where the recorded data is parsed through for a signal


  // print all the recorded information
  // bool currentBool = !micData[0];
  // for (int i = 0; i < currentWaitTime; i++) {
  //   if (micData[i] != currentBool) {
  //     Serial.println(String(i) + " " + String(micData[i]));
  //     currentBool = micData[i];
  //   }
  // }

  int currentTime = -1;
  bool foundSignal = false;
  bool lastSignal = true;
  while(!foundSignal && currentTime < currentWaitTime) {
    currentTime++;
    newSignal = micData[currentTime];
    if (newSignal && !lastSignal && !noData[currentTime]) {
      foundSignal = (signalStrenghAt(currentTime) > worstSignalStrengh);
    }
    lastSignal = newSignal;
  }
  if (foundSignal) { // get best signal and correct for offset
    flightTime = findBestSignal(currentTime) + signalTime;
  }
}

float Tof::signalStrenghAt(int currentTime) {
  // returns a float 0->1 grading the signal quality at currentTime
  int rightValues = 0;
  int totalValues = 0;
  int highTime, lowTime;
  for (int checkTime = 0; checkTime < pointsToCheck; checkTime++) {
    for (int cycle = 0; cycle < cyclesToCheck; cycle++) {
      highTime = 2*cycle*toggleTime+checkTime+currentTime;
      lowTime = (2*cycle+1)*toggleTime+checkTime+currentTime;
      if (!noData[highTime]) {
        totalValues++;
        if (micData[highTime]) {
          rightValues++;
        }
      }
      if (!noData[lowTime]) {
        totalValues++;
        if (!micData[lowTime]) {
          rightValues++;
        }
      }
    }
  }
  return ((float) rightValues)/totalValues;
}

int Tof::findBestSignal(int currentTime) {
  // look at nearby times for slightly better signals
  // (including earlier ones as they may not have been checked)
  int possTime;
  float possSignal;
  int bestTime = -1; // should cause an error if something is wrong
  float bestSignal = 0;
  for (int i = -nearbyRange; i <= nearbyRange; i++) {
    possTime = currentTime+i;
    if (possTime >= 0 && possTime < currentWaitTime) {
      possSignal = signalStrenghAt(possTime);
      if (possSignal > bestSignal) {
        bestSignal = possSignal;
        bestTime = possTime;
      }
    }
  }
  return bestTime;
}

void Tof::getDistance(void) {
  if (flightTime == -1) {
    distance = -1;
  } else { // distance = rate*time.
    distance = flightTime*SPEED_OF_SOUND_W_/MICROS_PER_S;
  }
}

void Tof::togglePin(void) {
  digitalWrite(SPEAKER_PIN, !digitalRead(SPEAKER_PIN));
}

size_t Tof::writeDataBytes(unsigned char * buffer, size_t idx)
{
  int * int_slot = (int *) (buffer + idx);
  int_slot[0] = flightTime;
  idx += sizeof(int);

  float * float_slot = (float *) (buffer + idx);
  float_slot[0] = distance;
  idx += sizeof(float);

  return idx;
}

bool Tof::loopTime(int loopStartTime) {
  int currentTime = millis();
  if (lastLoopTime == -1) {
    lastLoopTime = loopStartTime-TOF_LOOP_INTERVAL+TOF_LOOP_OFFSET;
  }
  if (currentTime - lastLoopTime >= TOF_LOOP_INTERVAL) {
    lastLoopTime = currentTime;
    return true;
  }
  return false;
}
