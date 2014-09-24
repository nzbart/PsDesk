#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <Arduino.h>

class MotorDriver {
  private:
    int enablePin1;
    int enablePin2;
    int currentSensePin1;
    int currentSensePin2;
    int PWMPin1;
    int PWMPin2;
    bool reversed;
    
    unsigned long currentSum;
    unsigned long currentSampleCount;
    int currentAverage;
    
  public:
    MotorDriver(int leftEnablePin, int rightEnablePin, int leftCurrentSensePin, int rightCurrentSensePin, int leftPwmPin, int rightPwmPin, bool reversed);
    bool IsMoving();
    void Move(int direction, byte speed);
    void StopMove();
    void Sense();
};

#endif
