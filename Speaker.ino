#include <Arduino.h>
#include <Wire.h>


#define mySerial Serial1


#define SPEAKER_PIN 20


void setup(){
    pinMode(SPEAKER_PIN, OUTPUT);
}

void loop(){
    digitalWrite(SPEAKER_PIN, HIGH);
    delay(1000);

}