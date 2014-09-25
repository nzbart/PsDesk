#ifndef SONAR_H
#define SONAR_H

#include <Arduino.h>

class Sonar {
  private:
    int triggerPin;
    int measurePin;
    
  public:
    Sonar(int triggerPin, int measurePin);
    void Run();
    bool IsReady();
    void Trigger();
    bool MeasurementAvailable();
    int GetLastMeasurement();
};

#endif
