#include "PControl.h"
#include "Printer.h"
extern Printer printer;

PControl::PControl(void) {
}

void PControl::init(const int totalWayPoints_in, const int stateDims_in, double * wayPoints_in) {
  totalWayPoints = totalWayPoints_in;
  stateDims = stateDims_in;
  wayPoints = wayPoints_in;
}

int PControl::getWayPoint(int dim) {
  return wayPoints[currentWayPoint*stateDims+dim];
}

void PControl::control(state_t * state) {
  updatePoint(state->x, state->y);
  if (currentWayPoint == totalWayPoints) return; // stops motors at final point

  // set up variables
  float K_P = 10.0;
  int x_des = getWayPoint(0);
  int y_des = getWayPoint(1);

  // determine diff between current state and des state
  float yaw_des = atan2(y_des - state->y, x_des - state->x);
  float yaw = state->heading;
  float yaw_diff = yaw_des - yaw;
  if (yaw_diff> PI) yaw_diff -= 2*PI;
  if (yaw_diff<-PI) yaw_diff += 2*PI;

  // actual PControl line
  float u = K_P*yaw_diff;
  // driveMotors(u); // this function actually sets the motor speeds

  String printString = "PControl: Yaw_Des: " + String(yaw_des)
    + " Yaw: " + String(yaw)
    + " u: " + String(u);
  printer.printValue(3,printString);

  // old control alg

  // int r = motorDriver_p->right;
  // int l = motorDriver_p->left;
  //
  // state.v = FWD_MODEL_A*r + FWD_MODEL_B*l;
  // state.w = FWD_MODEL_C*r + FWD_MODEL_D*l;
  //
  // // forward euler update
  // float vx = state.v*cos(state.heading);
  // float vy = state.v*sin(state.heading);
  //
  // state.x += vx*loop_period;
  // state.y += vy*loop_period;
  // //state.heading += state.w*loop_period;
  // state.heading = modfloat(state.heading + M_PI, 2*M_PI) - M_PI;
}


void PControl::driveMotors(float u) {
  double uL, uR;
  float avgPower = 5;
  float KL = 10.0; // allows for easy L/R motor calibration
  float KR = 10.0;

  uL = max(0.0,min(255.0,(avgPower - u)*KL));
  uR = max(0.0,min(255.0,(avgPower + u)*KR));
  motorDriver.driveForward(uL,uR);
}

void PControl::updatePoint(float x, float y) {
  if (currentWayPoint == totalWayPoints) return; // don't check if finished

  int x_des = getWayPoint(0);
  int y_des = getWayPoint(1);
  float dist = sqrt(pow(x-x_des,2) + pow(y-y_des,2));

  String wayPointUpdate = "PControl: Current Waypoint: " + String(currentWayPoint)
    + " Distance from Waypoint: " + String(dist);
  printer.printValue(2, wayPointUpdate);

  if (dist < SUCCESS_RADIUS && currentWayPoint < totalWayPoints) {
    String changingWPMessage = "Got to waypoint " + String(currentWayPoint)
      + ", now directing to next point";
    int cwpmTime = 20;
    currentWayPoint++;
    if (currentWayPoint == totalWayPoints) {
      changingWPMessage = "Congratulations! You completed the path! Stopping motors.";
      motorDriver.stopDriving();
      cwpmTime = 0;
    }
    printer.printMessage(changingWPMessage,cwpmTime);
  }
}

bool PControl::loopTime(int loopStartTime) {
  int currentTime = millis();
  if (lastLoopTime == -1) {
    lastLoopTime = loopStartTime-PC_LOOP_INTERVAL+PC_LOOP_OFFSET;
  }
  if (currentTime - lastLoopTime >= PC_LOOP_INTERVAL) {
    lastLoopTime = currentTime;
    return true;
  }
  return false;
}
