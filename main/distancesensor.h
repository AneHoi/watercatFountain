#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include <Arduino.h>

class DistanceSensor {
  
  private:
    byte echopin;
    byte trigpin;
    
  public:
    // Setup pins and call init()
    DistanceSensor(byte echopin, byte trigpin);
    void init();
    float measureDistanceInCM();
};

#endif