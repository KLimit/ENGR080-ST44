#ifndef __MOTOR_DRIVER_H__
#define __MOTOR_DRIVER_H__

#define NUM_MOTORS 4

#define LEFT_MOTOR_INDEX 0
#define RIGHT_MOTOR_INDEX 1
#define VERT_MOTOR_INDEX 3  //2 seems dead
#define X_MOTOR_INDEX 2
#define FORWARD_PIN 0
#define BACKWARD_PIN 1

// the minimum PWM amount that causes the motors to actually spin
#define MOTOR_DEADZONE 34

#include <Arduino.h>
#include "DataSource.h"
#include "Pinouts.h"

/*
 * MotorDriver handles the raw signals that are needed to drive the robot's
 * motors.
 */
class MotorDriver : public DataSource
{
public:
  MotorDriver();

  // initializes pins
  void init(void);

  // applies the stored values to the pins
  void apply(void);

  // prints current output to Serial port
  void printState(void);

  // simple motor directions (great for replacing or adding to pcontrol)
  void stopDriving(void);
  void driveDown(int power);
  void driveUp(int power);
  void driveAll(int power);
  void driveAllBack(int power);
  void driveForward(int leftPower, int rightPower);
  void driveBack(int leftPower, int rightPower);
  void turnRight(int power);
  void turnLeft(int power);
  void drive(int m1_power,int m2_power,int m3_power,int m4_power) ;

  // Range from -255 to +255 for full reverse or full forward
  int motorValues[NUM_MOTORS];

  // from DataSource
  size_t writeDataBytes(unsigned char * buffer, size_t idx);

private:
  // pins for the motors
  int motorPins[NUM_MOTORS][2] = {{MOTOR_L_FORWARD,MOTOR_L_REVERSE},
                                  {MOTOR_R_FORWARD,MOTOR_R_REVERSE},
                                  {MOTOR_V_FORWARD,MOTOR_V_REVERSE},
                                  {MOTOR_X_FORWARD,MOTOR_X_REVERSE}};

  // pwm data
  unsigned int pwmValues[NUM_MOTORS]; // absolute value
  bool pwmDir[NUM_MOTORS]; // direction
};

#endif
