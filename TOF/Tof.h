#ifndef __TOF_H__
#define __TOF_H__

#include <Wire.h>
#include <Pinouts.h>

#define PULSE_MAX 3
#define TX_FREQUENCY 12500
#define SPEED_OF_SOUND_W_ 1484.0 // this changes with temp!
#define MICROS_PER_S 1000000.0
#define MAX_DIST 5.0

#define TOF_LOOP_INTERVAL 100 // ms
#define TOF_LOOP_OFFSET 10 // ms

#include "DataSource.h"
#include "Pinouts.h"

class Tof : public DataSource
{
public:
  Tof(void);

  void init(void);

  void updateTOF(void);

  size_t writeDataBytes(unsigned char * buffer, size_t idx);

  bool loopTime(int loopStartTime);

private:
  // Signal processing functions
  void reset(void);
  void Tx(void); // send
  void Rx(void); // recieve
  void getTOF(void);  // process
  void getDistance(void);

  // helper helper functions
  void togglePin(void);
  float signalStrenghAt(int currentTime);
  int findBestSignal(int currentTime);

  // Private data members
  int flightTime; // time of flight in microseconds
  float distance;   // calculated distance from object

  const static int toggleTime = (int) MICROS_PER_S/(TX_FREQUENCY*2);  // µs
  int signalTime = toggleTime*PULSE_MAX*2; // µs

  float currentDist = MAX_DIST; // m (controls how long rx will wait)
  const static int maxWaitTime = (int) MICROS_PER_S * ((float) MAX_DIST) / SPEED_OF_SOUND_W_; // µs
  int currentWaitTime = (int) MICROS_PER_S * currentDist / SPEED_OF_SOUND_W_; // µs
  bool micData[maxWaitTime]; // data storage before processing
  bool noData[maxWaitTime];

  // signal quality checks
  float worstSignalStrengh = 0.9;
  const static int cyclesToCheck = 3;
  float pointsToCheck = 3; // number of micros in the signal to check each cycle
  int nearbyRange = 4; // actually checks twice as much

  int lastLoopTime = -1;
};

#endif
