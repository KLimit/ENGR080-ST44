#ifndef __STATE_ESTIMATOR_H__
#define __STATE_ESTIMATOR_H__

#include <Arduino.h>

#include <SensorGPS.h>
#include <SensorIMU.h>
#include "DataSource.h"
#include "Pinouts.h"


#define SE_LOOP_INTERVAL 100 // ms
#define SE_LOOP_OFFSET 40 // ms

typedef struct {
  float x = 0; // x position in global frame
  float y = 0; // y position in global frame
  float heading = 0; // heading in global frame
  // float v = 0; // linear velocity in robot frame
  // float w = 0; // rotational velocity in robot frame
} state_t;

/*
 * StateEstimator class keeps track of the robot's state, incorporating
 * measurements of the system outputs from the various sensors like IMU or
 * GPS as well as the control inputs to the system.
 */
class StateEstimator : public DataSource
{
public:
  StateEstimator(void);

  // init
  void init(double orig_lat, double orig_lon);

  // State Access
  state_t state;
  void updateState(sensors_vec_t * imu_state_p, gps_state_t * gps_state_p);
  void printState(void);

  void latlonToXY(double lat, double lon, float* x_p, float* y_p);

  // from DataSource
  size_t writeDataBytes(unsigned char * buffer, size_t idx);

  bool loopTime(int loopStartTime);

private:
  double loop_period;
  double orig_lat;
  double orig_lon;
  float cosOrigLat;

  int lastLoopTime = -1;
};

#endif
