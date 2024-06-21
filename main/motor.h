#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor {
  
  private:
    byte pin;
    bool isMotorOn;
    
  public:
    // Setup pin motor and call init()
    Motor(byte pin);

    // Setup the pin motor as OUTPUT
    // and power off the motor - default state
    void init();
    
    bool ison();

    // Power on the motor
    void on();

    // Power off the motor
    void off();
};

#endif