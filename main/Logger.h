#ifndef __LOGGER_H__
#define __LOGGER_H__
#include <stdio.H>

#define LOG_FILENAME_BASE "log"
#define LOG_FILENAME_BUFFERLEN 20
#define HEADINGS_FILENAME_BASE "inf"
// buffered logging
// number of 512B blocks in the log file
#define FILE_BLOCK_COUNT 8192 // should last over 10 min
#define BYTES_PER_BLOCK 256
// number of blocks in the buffer
#define BUFFER_BLOCK_COUNT 5
#define MAX_NUM_DATASOURCES 10

#define LOG_LOOP_INTERVAL 100 // ms
#define LOG_LOOP_OFFSET 55 // ms

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "DataSource.h"
#include "Pinouts.h"

class Logger {
public:
	Logger(void);

  // include all dataSources before running init
	void include(DataSource * source_p);

  // run after all dataSources have been registered
	void init(void);

	// records all data at the time it's called to the SD
	bool log(void);

	bool loopTime(int loopStartTime);

private:
	void padding(int number, byte width, String & str);

	DataSource* sources[MAX_NUM_DATASOURCES];
	unsigned int num_datasources;
	char logfilename[LOG_FILENAME_BUFFERLEN];
	char headingfilename[LOG_FILENAME_BUFFERLEN];
	File file;

	uint32_t writtenBlocks = 0;

	int lastLoopTime = -1;
};
#endif
