#ifndef __PCONTROL_H__
#define __PCONTROL_H__

/* OLD CONTROL MODEL */
// [a b; c d]*[rpwm; lpwm] = [v; w]
// #define FWD_MODEL_A 0.0068
// #define FWD_MODEL_B 0.0063
// #define FWD_MODEL_C 0.0019
// #define FWD_MODEL_D -0.0042
// // [a b; c d]*[v; w] = [rpwm; lpwm]
// // this matrix should be inv(FWD_MODEL matrix)
// #define INV_MODEL_A 103.5546
// #define INV_MODEL_B 156.1537
// #define INV_MODEL_C 46.8461
// #define INV_MODEL_D -167.4543
//
// // maximum linear velocity in m/s
// #define MAX_LIN_VEL 1.0
//
// // maximum rotational velocity in rad/s
// #define MAX_ROT_VEL 0.2

#define SUCCESS_RADIUS 2.0 // success radius in meters

#define PC_LOOP_INTERVAL 100 // ms
#define PC_LOOP_OFFSET 45 // ms

#include <Arduino.h>
#include "MotorDriver.h"
#include "StateEstimator.h"
extern MotorDriver motorDriver;

class PControl {
public:
  PControl(void);

  // defines the waypoints used for pControl
  void init(const int totalWayPoints_in, const int stateDims_in, double * wayPoints_in);

  // sets the motor speeds using P-Control
  void control(state_t * state);

  bool loopTime(int loopStartTime);

private:
  // sets motor powers based on P-Control
  void driveMotors(float u);

  // updates the current waypoint if necessary
  void updatePoint(float x, float y);

  int getWayPoint(int dim);

  int totalWayPoints, stateDims;
  double * wayPoints;
  int currentWayPoint = 0;
  int lastLoopTime = -1;
};

#endif
