#include "Sonar.h"
#include <Wire.h>
#include "Interval.h"

Interval pingInterval(100, 64);
unsigned long measurement = 0;
bool measuring = false;
unsigned long initialMicros = 0;
bool measurementAvailable = false;

Sonar::Sonar(int triggerPin, int measurePin) {
  this->triggerPin = triggerPin;
  this->measurePin = measurePin;
  pinMode(triggerPin, OUTPUT);
  pinMode(measurePin, INPUT);

}

void Sonar::Run() {
  if (pingInterval.elapsed()) {
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10 * 64);
    digitalWrite(triggerPin, LOW);
  }

  int value = digitalRead(measurePin);
  if (!measuring && value) {
    measuring = true;
    initialMicros = micros() / 64;
    
  } else if (measuring && !value) {
    measuring = false;
    measurement = (micros() / 64) - initialMicros;
    measurementAvailable = true; 
  }
}

bool Sonar::MeasurementAvailable() {
  return measurementAvailable;
}

int Sonar::GetLastMeasurement() {
  measurementAvailable = false;
  return measurement;
}
