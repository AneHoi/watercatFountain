#include "Arduino.h"
#include "distancesensor.h"

#define SOUND_SPEED 0.034

float distanceCm;
long duration;

DistanceSensor::DistanceSensor(byte echopin, byte trigpin) {
  this->echopin = echopin;
  this->trigpin = trigpin;
  init();
}
void DistanceSensor::init(){
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
}

float DistanceSensor::measureDistanceInCM() {
  // Clears the trigPin
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);

  duration = pulseIn(echopin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;
  return distanceCm;
}