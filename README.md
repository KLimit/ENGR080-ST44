# E80-AUV
Arduino project for the Teensy microcontroller used for the HMC E80 AUV

COPY THIS LIBRARIES FOLDER INTO THE ARDUINO FOLDER -- IT DOES NOTHING ANYWHERE ELSE

### LOOP_INTERVAL, LOOP_OFFSET, lastLoopTime
These variables control when each set of functions is called by the main loop.  They come up in the loopTime() function included in most of the libraries.  The loop interval controls how often the functions are called, and the loop offset decides where in the loop this call happens.
By default, the offsets are:
00 - printer
10 - IMU
20 - GPS
30 - ADC
40 - StateEstimator
50 - PControl
55 - Write (in logger) // largest buffer for long call
95 - Log (in logger)
These function to space out the calls so that nothing trips on anything else as it loops.


## Main Classes

### LED
A template class without much functionality.  Useful for copying to create new more functional libraries of your own.

### Pinouts
Very short library to control which pins are designated for what on the teensy.  Called/used by most other libraries.

### Printer
This is a really useful library that should stop your having to use any "Serial.println()" calls, except for debugging.  The init() for printer opens the Serial port, so this is especially important.  The printValue and printMessage functions allow you to print repeated values or one-time messages easily.  Finally, the printToSerial call actually sends all of the information to your computer (if available).

printValue takes in a row number, which is the row the value String will be displayed on, and a string.  This is useful for values that you plan on displaying over and over, where you will simply want to update the value itself as you get new information.

printMessage takes in a String and an integer representing the amount of times you want to display the message.  By default, printToSerial is called 10 times per second, so 10 corresponds to 1 second, and 100 to 10 seconds, etc.  This is useful for error messages and notifications that you want to appear once but don't want to constantly update.
PROTIP: A time value of 0 will stay displayed indefinitely, until the teensy is reset or until other messages force it off the screen.  This is used in many libraries for printing error messages that will stay visible over time.

### MotorDriver
This is the class that actually drives each of the motor pins.  init() initializes each of the pins for PWM signals (where the pins are decided by Pinouts.h).  apply() writes PWM signals to the pins according to the magnitude of the "left", "right", and "vertical" variables stored publicly in the class.  Changing these variables and then calling apply() allows you to change the power directed to each motor.
Additionally, some simple motor directions are provided publicly in the class for more blunt control than that used by PControl.  These functions allow you to easily test the motors (with driveAll or driveAllBack), as well as move in any of the main directions.

### PControl
Controls the movement of default_robot.  The init() call stores the desired waypoints -- to change the waypoints, add more, or set up a more complicated system (such as adding a new dimensions like heading or height to the state), go to the init() call in the Initialize section of code.
The control() function is the heart of PControl.  It uses updatePoint() to check whether it should change which point it is targeting, sorts out how it should get there using proportional control of yaw, then sends this information to the motors with driveMotors().  Some of this may be left to you to code.

** Note: To get the coordiates of a wayPoint, you should use getWayPoint.  The short answer is just because; use 0 for x and 1 for y of the current coordinate.  The long one is: though wayPoints is initially created as a two dimensional array, it's stored in PControl as a pointer to a double.  While you can often treat pointers as arrays, two dimensional arrays are more complicated.  The call wayPoints[i] really translates to *(wayPoints + i), or the data value stored wherever wayPoints points to plus i*sizeof(double) bytes over. Since it isn't stored in PControl as a two dimensional array, a call to wayPoints[i][j] would return *(*(wayPoints + i) + j) which doesn't make any sense to the compiler, because the inner value is a double, not a pointer.  Therefore getWayPoint only uses one bracket, and more manually increments by factors of stateDims if totalWayPoints != 0. Hope that made any sense. **

### ADCSampler
Controls the analog pin sampling of the teensy.  Uses the dataSource interface to log the measured voltages of each pin.

### SensorIMU
This class is a wrapper for the IMU sensor, offering functions to poll the IMU for new data, and keeping internal state variables holding the latest received data. Calls are made using functions from the IMU-libraries folder, which contains libraries Adafruit made to work with the IMU.  This class implements the DataSource interface.  Read calls for the IMU typically take about 3 ms.

### SensorGPS
This class is a wrapper for the GPS sensor, using the TinyGPS libraries included in a separate folder.  As with the IMU library, internal state variables hold the latest results, which are then stored into the logger through the DataSource interface.  Read calls again take about 3 ms.
There is currently very little checking done whether the data is correct or not before it is displayed, stored, or sent to the stateEstimator.  However, this check is easily done.  Typically, before there's a fix, the lat and lon are reported as 0.  A check could easily be added in init that waits until there's actually a fix and non-zero or near-origin values are reported.  Additionally, the f_get_position function has an optional third argument that reports the time since the last fix.  The GPS could wait until the fix has been updated by watching this variable through repeated calls.  All of these checks add time, however.  There's a *lot* of leeway in the current main loop, but checks like these you may only want to implement if the robot is depending heavily on the GPS to function or report data.  Definitely take note of call times and calibrate the loop offsets to keep everything running smoothly!

### StateEstimator
The 'StateEstimator' class defines an object which handles all the state estimation of the robot. It keeps an internal 'state' struct which stores the current estimate of the state. It is 'init'ialized with the origin gps coordinates.

The main "updateState" function incorporates data from the IMU and GPS and converts them to states to be used by PControl or any other driving library.

It also implements the 'DataSource' interface to plug into the 'Logger' framework.

### Logger
The 'Logger' class handles all the logging. It is written in a way to hide away the intricacies of logging the heterogenous set of variables a user may want to log.

In the main program's setup function, the 'Logger' object's 'include' function is run, passing pointers to any 'DataSource' objects that should be logged.

Next, the 'Logger' object's 'init' function is run, which automatically finds an unused filename. Each log creates two files, 'INFXXX.txt' and 'LOGXXX.bin', where the 'XXX' is the smallest number for which a file with the same name doesn't already exist on the SD card.

The 'INF' file describes the schema of the data being logged. It's first line contains comma separated column names, and its second line contains comma separated column data types. The 'LOG' file contains the raw binary values of each variable being logged, with each row of data written in the order described by the 'INF' file.

The logging actually happens through two functions: 'log' and 'write';

The 'log' function samples all the 'DataSource' objects that were registered to the 'Logger' during setup by the 'include' function. It calls the 'writeDataBytes' function of each 'DataSource', to write the binary values of the variables to be logged to a buffer.  This buffer is composed of 16 blocks stored in the "blocks" variable--on the Teensy, not the SD.  Internally, the 'Logger' object keeps track of which of these blocks have been logged and which have been written to the sd. The 'log' function grabs a blank block and writes the data to it.  This transition is done no matter how full the previous block was, typically adding about 120 bytes of buffer into each log.  This blank space stops any strange writing hiccups from throwing off later data values.

** Note: the 'log' function does NOT actually write anything to the SD card. The fact that this function only writes to a buffer means it takes less than a ms typically. **

** Note: the 'log' function does decide what data is recorded.  If it is called more often than data is updated, data will appear duplicated on the SD card.  If it is called less often, measured data will be lost.  'write' has no effect on what data is kept, unless it is called so infrequently that the buffer blocks overflow. **

The actual writing to the SD card is handled by the 'write' function. This function loops as long as there is space in the LOG file on the SD card available to write. To add space, increase the FILE_BLOCK_COUNT variable.  Upon each loop, it checks if there are any newly logged blocks to write to the SD card.  With the standard times, there should be 4 blocks logged, which are then transferred over to the SD card.

** Note: logger.write() is the longest function in the main loop by a factor of ~8.  It takes around 23 ms in my experience -- but verify that for your teensy and data if you're concerned about it.  Take this time into account when deciding how to offset calls for the main loop. **


### DataSource
'DataSource' is an abstract class representing a source of data to be logged. 'DataSource' objects must have a 'varNames' String member which contains the names of the columns, a 'varTypes' String member which contains the data types of all the columns and a 'writeDataBytes' function which writes the raw binary data of each variable to the passed in buffer.

### TOF
This library allows for time of flight testing to be easily integrated into default robot.

Vai Viswanathan (vviswanathan@hmc.edu),
*         based on code written by Aaron Lutzker
Eyassu Shimelis (eshimelis@hmc.edu),
