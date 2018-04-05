 // This is a test script for recording audio. It cannot be used with the logger yet.

 // Recordings are made using the teensyduino Audio library.

#incude <Audio.h>

micpin = 2;

AudioRecordQueue queue1;

int mode = 0;  // 0: stopped, 1: recording

void loop() {
}


void startRecording() {
	Serial.println("Recording has started")
	queue1.begin();
	mode = 1;
}

void continueRecording() {
	if (queue1.available() >= 2) {
		byte buffer[512];

		// Fetch two blocks from the audio library and copy
		// them into a 512 byte buffer. The SD library that the
		// example code uses (Arduino SD) is most efficient when
		// full 512 byte sector size writes are used.
		// memcpy() copies from the second argument to the first,
		// with the third argument as number of bytes to copy.
		memcpy(buffer, queue1.readBuffer(), 256);
		queue1.freeBuffer();
		memcpy(buffer+256, queue1.readBuffer(), 256);
		queue1.freeBuffer();

		// Write all 512 bytes to the SD card via memcpy(). Use
		// something else for our SD setup.
		elapsedMicros usec = 0;
		frec.write(buffer, 512);  // written to file frec
	}
}

void stopRecording() {
	Serial.println("Recording has stopped")

	queue1.end;

	mode = 0;
}
