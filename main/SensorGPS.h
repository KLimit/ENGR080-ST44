#ifndef __SENSOR_GPS_H__
#define __SENSOR_GPS_H__

#define GPS_READ_INTERVAL 3

#define GPS_LOOP_INTERVAL 100 // ms
#define GPS_LOOP_OFFSET 20 // ms

#define RADIUS_OF_EARTH_M 6371000

#include <Arduino.h>
//#include <TinyGPS.h>
#include "DataSource.h"
#include "Pinouts.h"
//#define GPSECHO  true
#include <Adafruit_GPS.h>

typedef struct {
  float lat;
  float lon;
  uint32_t age;
  uint16_t hdop;
  uint8_t num_sat;
} gps_state_t;

class SensorGPS : public DataSource {
public:
	SensorGPS(void);
  

  
  // Starts the connection to the sensor
  void init(Adafruit_GPS* GPS);

  // Reads data from the sensor, returns whether new data was received
  void read(Adafruit_GPS* GPS);

  // Latest reported data is stored here
  gps_state_t state;
  void printState(void);

  // from DataSource
  size_t writeDataBytes(unsigned char * buffer, size_t idx);

  bool loopTime(int loopStartTime);

private:
  //TinyGPS gps;


  HardwareSerial * Uart_p;

  // copies over latest data from gps object to the state struct
  void updateState(Adafruit_GPS* GPS);

  float convertDegMinToDecDeg (float degMin);

  int lastLoopTime = -1;
};

#endif
