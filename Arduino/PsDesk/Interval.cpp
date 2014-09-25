#include "Interval.h"

Interval::Interval(unsigned long milliseconds, unsigned long divisor) {
  this->milliseconds = milliseconds;
  this->divisor = divisor;
  this->lastMilliseconds = millis() / divisor;
}

bool Interval::elapsed() {
  unsigned long now = millis() / divisor;
  if ((now - lastMilliseconds) >= milliseconds) {
    reset();
    return true;
  }
  
  return false;
}

void Interval::reset() {
  lastMilliseconds = millis() / divisor;
}
