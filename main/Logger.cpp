#include "Logger.h"
#include <stdio.h>
#include "Printer.h"
#include "MicrophoneADC.h"
extern Printer printer;
String message;

Logger::Logger(void)
	: num_datasources(0)
{
}

void Logger::include(DataSource * source_p) {
	sources[num_datasources] = source_p;
	++num_datasources;
}

void Logger::padding(int number, byte width, String & str) {
	int currentMax = 10;
	for (byte i = 1; i < width; i++) {
		if (number < currentMax) {
			str.concat("0");
		}
		currentMax *= 10;
	}
	str.concat(number);
}

void Logger::init(void) {
	Serial.print("Initializing SD Card... ");
  if (!SD.begin()) {
    Serial.println("failed!");
    return;
  }
  Serial.println("done!");

	unsigned int number = 0;
	String numstr = "";
	padding(number, 3, numstr);
	String finalname = LOG_FILENAME_BASE + numstr + ".bin";
	finalname.toCharArray(logfilename, LOG_FILENAME_BUFFERLEN);

	while(SD.exists(logfilename)) {
		number++;
		numstr = "";
		padding(number, 3, numstr);
		finalname = LOG_FILENAME_BASE + numstr + ".bin";
		finalname.toCharArray(logfilename, LOG_FILENAME_BUFFERLEN);
	}

	finalname = HEADINGS_FILENAME_BASE + numstr + ".txt";
	finalname.toCharArray(headingfilename, LOG_FILENAME_BUFFERLEN);

	message = "Logger: Using log file name " + String(logfilename);
	printer.printMessage(message,30);

	String headingStr = sources[0]->csvVarNames;
	String dataTypeStr = sources[0]->csvDataTypes;
	for(size_t i = 1; i < num_datasources; ++i) {
		headingStr += ",";
		headingStr += sources[i]->csvVarNames;
		dataTypeStr += ",";
		dataTypeStr += sources[i]->csvDataTypes;
	}
	headingStr += "\n"+dataTypeStr;

	file = SD.open(headingfilename, FILE_WRITE);

	// if the file exists, use it:
  if (file) {
    file.println(headingStr);
    file.close();
	}

	printer.printMessage("Creating log file",10);
	file = SD.open(logfilename, FILE_WRITE);
	if(!file) {
		message = "Logger: error creating " + String(logfilename);
		printer.printMessage(message,0);
	} else {
		file.close();
	}
}

bool Logger::log(void){
	// record data from sources
	size_t idx = 0;
	unsigned char buffer[BYTES_PER_BLOCK]; //edited
	for(size_t i = 0; i < num_datasources; ++i) {

		idx = sources[i]->writeDataBytes(buffer, idx);
		if (idx >= BYTES_PER_BLOCK) {
			printer.printMessage("Too much data per log. Increase BYTES_PER_BLOCK or reduce data", 2);
		}
	}

	// write data to SD
	if (writtenBlocks >= FILE_BLOCK_COUNT) {
		printer.printMessage("Current file size limit reached. Change FILE_BLOCK_COUNT to fix. Stopping logging for now.",0);
		return false;
	}

	file = SD.open(logfilename, FILE_WRITE);
	if (file) {
		int bytes = file.write(&buffer[0],BYTES_PER_BLOCK);
		if (!bytes) {
			printer.printMessage("Logger: Error printing to SD",0);
		}
	}
	file.close();

	writtenBlocks++;
	printer.printValue(2,"Logger: Just logged buffer " + String(writtenBlocks) + " to SD.");
	return true;
}

bool Logger::micLog(MicrophoneADC mic){

	size_t idx = 0;
	unsigned long timeBuffer[sizetbd]; //sizetbd should be as long as it takes to record 30 ms
	unsigned char analogBuffer[sizetbd];
	unsigned long envelopeTime;

	mic->writeDataBytes(timeBuffer, analogBuffer); //sources[0] must be a microphone object for this to work.

	file = SD.open(logfilename, FILE_WRITE);
	if(file){
		int bytes = file.write(&timeBuffer[0], sizetbd);
		int bytes2 = file.write(&analogBuffer[0], sizetbd);
		int bytes3 = file.write((int) &envelopeTime);

		if(!bytes || !bytes2){
			printer.printMessage("Logger: Error printing to SD",0);
		}
	}
	file.close;
	return true;
}



bool Logger::loopTime(int loopStartTime) {
  int currentTime = millis();
  if (lastLoopTime == -1) {
    lastLoopTime = loopStartTime-LOG_LOOP_INTERVAL+LOG_LOOP_OFFSET;
  }
  if (currentTime - lastLoopTime >= LOG_LOOP_INTERVAL) {
    lastLoopTime = currentTime;
    return true;
  }
  return false;
}
