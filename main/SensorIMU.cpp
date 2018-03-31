#include "SensorIMU.h"
#include "Printer.h"
extern Printer printer;

SensorIMU::SensorIMU(void)
  : DataSource("rollIMU,pitchIMU,headingIMU,accelX,accelY,accelZ","float,float,float,float,float,float") {
}

void SensorIMU::init(void) {
  Serial.print("Initializing IMU... ");

  // Initialize the sensors.
  if(!gyro.begin()) {
    printer.printMessage("No gyro detected (IMU failed to initialize)",0);
  }

  if(!accelmag.begin(ACCEL_RANGE_4G)) {
    printer.printMessage("No FXOS8700 detected (IMU failed to initialize)",0);
  }
  // Filter expects 10 samples per second, adjust as necessary
  filter.begin(10);

  Serial.println("done!");
}

void SensorIMU::read(void) {
  sensors_event_t gyro_event;
  sensors_event_t accel_event;
  sensors_event_t mag_event;

  // Get new data samples
  gyro.getEvent(&gyro_event);
  accelmag.getEvent(&accel_event, &mag_event);

  // Apply mag offset compensation (base values in uTesla)
  float x = mag_event.magnetic.x - mag_offsets[0];
  float y = mag_event.magnetic.y - mag_offsets[1];
  float z = mag_event.magnetic.z - mag_offsets[2];

  // Apply mag soft iron error compensation
  float mx = x * mag_ironcomp[0][0] + y * mag_ironcomp[0][1] + z * mag_ironcomp[0][2];
  float my = x * mag_ironcomp[1][0] + y * mag_ironcomp[1][1] + z * mag_ironcomp[1][2];
  float mz = x * mag_ironcomp[2][0] + y * mag_ironcomp[2][1] + z * mag_ironcomp[2][2];

  // Apply gyro zero-rate error compensation, convert to degrees/s from rad/s
  float gx = (gyro_event.gyro.x + gyro_zero_offsets[0]) * 57.2958F;
  float gy = (gyro_event.gyro.y + gyro_zero_offsets[1]) * 57.2958F;
  float gz = (gyro_event.gyro.z + gyro_zero_offsets[2]) * 57.2958F;

  // Update the filter
  filter.update(gx, gy, gz,
                accel_event.acceleration.x, accel_event.acceleration.y, accel_event.acceleration.z,
                mx, my, mz);

  // update the orientation data
  state.roll = filter.getRoll();
  state.pitch = filter.getPitch();
  state.heading = filter.getYaw();
  
  // udpate the acceleration data
  acceleration.x = accel_event.acceleration.x;
  acceleration.y = accel_event.acceleration.y;
  acceleration.z = accel_event.acceleration.z;
}

void SensorIMU::printState(void) {
  String printString = "IMU:"; 
  printString += " roll: ";
  printString += String(state.roll);
  printString += " pitch: "; 
  printString += String(state.pitch); 
  printString += " heading: ";
  printString += String(state.heading);
  printer.printValue(5, printString);
  
  printString = "IMU:";
  printString += " accel X: ";
  printString += String(acceleration.x);
  printString += " accel Y: ";
  printString += String(acceleration.y);
  printString += " accel Z: ";
  printString += String(acceleration.z);
  printer.printValue(6, printString);
}

size_t SensorIMU::writeDataBytes(unsigned char * buffer, size_t idx) {
  float * data_slot = (float *) &buffer[idx];
  data_slot[0] = state.roll;
  data_slot[1] = state.pitch;
  data_slot[2] = state.heading;
  data_slot[3] = acceleration.x;
  data_slot[4] = acceleration.y;
  data_slot[5] = acceleration.z;
  return idx + 6*sizeof(float);
}

bool SensorIMU::loopTime(int loopStartTime) {
  int currentTime = millis();
  if (lastLoopTime == -1) {
    lastLoopTime = loopStartTime-IMU_LOOP_INTERVAL+IMU_LOOP_OFFSET;
  }
  if (currentTime - lastLoopTime >= IMU_LOOP_INTERVAL) {
    lastLoopTime = currentTime;
    return true;
  }
  return false;
}
