#ifndef INTERVAL_H
#define INTERVAL_H

#include <Arduino.h>

class Interval {
  private:
    unsigned long milliseconds;
    unsigned long divisor;
    unsigned long lastMilliseconds;
    
  public:
    Interval(unsigned long milliseconds, unsigned long divisor);
    bool elapsed();
    void reset();
};

#endif
