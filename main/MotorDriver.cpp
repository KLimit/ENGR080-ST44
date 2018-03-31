#include "MotorDriver.h"
#include "Printer.h"
extern Printer printer;


MotorDriver::MotorDriver()
: DataSource("left,right,vertical","int,int,int")
{
  for (int m = 0; m < NUM_MOTORS; m++) {
    motorValues[m] = 0;
  }
}

void MotorDriver::init(void) {
  for (int m = 0; m < NUM_MOTORS; m++) {
    pinMode(motorPins[m][BACKWARD_PIN],OUTPUT);
    pinMode(motorPins[m][FORWARD_PIN],OUTPUT);
  }
}

void MotorDriver::apply(void)
{
  // determine direction and magnitude of spin required:
  for (int m = 0; m < NUM_MOTORS; m++) {
    pwmValues[m] = (motorValues[m] < 0) ? -motorValues[m] : motorValues[m];
    if (motorValues[m]) { // correct for deadzone if not zero
      pwmValues[m] = pwmValues[m] - (MOTOR_DEADZONE*pwmValues[m])/255 + MOTOR_DEADZONE;
    }
    pwmDir[m] = (motorValues[m] >= 0); // true if motor goes forward
  }

  // write this information to motors
  for (int m = 0; m < NUM_MOTORS; m++) { // using pwmDir as 0 or 1
    analogWrite(motorPins[m][FORWARD_PIN], pwmDir[m]*pwmValues[m]);
    analogWrite(motorPins[m][BACKWARD_PIN], (!pwmDir[m])*pwmValues[m]);
  }
}

void MotorDriver::printState(void) {
  String printString =
    "Motors: M1: " + String(pwmDir[LEFT_MOTOR_INDEX]*pwmValues[LEFT_MOTOR_INDEX])
    + " M2: " + String(pwmDir[RIGHT_MOTOR_INDEX]*pwmValues[RIGHT_MOTOR_INDEX])
    + " M3: " + String(pwmDir[X_MOTOR_INDEX]*pwmValues[X_MOTOR_INDEX])
    + " M4: " + String(pwmDir[VERT_MOTOR_INDEX]*pwmValues[VERT_MOTOR_INDEX]);

  printer.printValue(4,printString);
}

size_t MotorDriver::writeDataBytes(unsigned char * buffer, size_t idx) {
  int * data_slot = (int *) &buffer[idx];
  for (int i = 0; i < NUM_MOTORS; i++) {
    data_slot[i] = pwmDir[i]*pwmValues[i];
  }
  return idx + NUM_MOTORS*sizeof(int);
}

// fire everything forward (for testing)
void MotorDriver::driveAll(int power) {
  for (int m = 0; m < NUM_MOTORS; m++) {
    motorValues[m] = power;
  }
  apply();
  printState();
}

// stopDriving(): stops movement
void MotorDriver::stopDriving(void) {
  driveAll(0);
}

// power vertical motors up
void MotorDriver::driveUp(int power) {
  motorValues[LEFT_MOTOR_INDEX] = 0; //M1
  motorValues[RIGHT_MOTOR_INDEX] =0; //M2
  motorValues[VERT_MOTOR_INDEX] = 0; //M4
  motorValues[X_MOTOR_INDEX] = -power; //M3
  apply();
  printState();
}

// power vertical motors down
void MotorDriver::driveDown(int power) {
  driveUp(-power);
}

// power horizontal motors forward
void MotorDriver::driveForward(int leftPower, int rightPower) {
  motorValues[LEFT_MOTOR_INDEX] = leftPower;
  motorValues[RIGHT_MOTOR_INDEX] = rightPower;
  motorValues[VERT_MOTOR_INDEX] = 0;
  apply();
  printState();
}

// power horizontal motors backwards
void MotorDriver::driveBack(int leftPower, int rightPower) {
  driveForward(-leftPower,-rightPower);
}

// power horizontal motors oppositely
void MotorDriver::turnRight(int power) {
  motorValues[LEFT_MOTOR_INDEX] = power;
  motorValues[RIGHT_MOTOR_INDEX] = -power;
  motorValues[VERT_MOTOR_INDEX] = 0;
  apply();
  printState();
}

// power horizontal motors oppositely the other way
void MotorDriver::turnLeft(int power) {
  turnRight(-power);
}

// power vertical motors up
void MotorDriver::drive(int m1_power,int m2_power,int m3_power,int m4_power) {
  motorValues[LEFT_MOTOR_INDEX] = m1_power; //M1
  motorValues[RIGHT_MOTOR_INDEX] = m2_power; //M2
  motorValues[VERT_MOTOR_INDEX] = m4_power; //M4
  motorValues[X_MOTOR_INDEX] = m3_power; //M3
  apply();
  printState();
}
