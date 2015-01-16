#ifndef ADXL345ACCELEROMETER_H
#define ADXL345ACCELEROMETER_H

#include <Arduino.h>

class Adxl345Accelerometer {
  private:
      int _chipSelectPin;
    
  public:
    Adxl345Accelerometer(int chipSelectPin);
    int GetX();
};

#endif

