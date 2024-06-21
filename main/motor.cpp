#include "motor.h"

Motor::Motor(byte pin) {
  this->pin = pin;
  init();
}

void Motor::init() {
  pinMode(pin, OUTPUT);
  off();
  this->isMotorOn = false;
}

bool Motor::ison(){
  return this->isMotorOn;
}

void Motor::on() {
  digitalWrite(pin, LOW);
  this->isMotorOn = true;
}

void Motor::off() {
  digitalWrite(pin, HIGH);
  this->isMotorOn = false;
}