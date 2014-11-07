#include "MotorDriver.h"
#include <Wire.h>
#include "Interval.h"

MotorDriver::MotorDriver(int leftEnablePin, int rightEnablePin, int leftCurrentSensePin, int rightCurrentSensePin, int leftPwmPin, int rightPwmPin, bool reversed) {
  this->enablePin1 = leftEnablePin;
  this->enablePin2 = rightEnablePin;
  this->currentSensePin1 = leftCurrentSensePin;
  this->currentSensePin2 = rightCurrentSensePin;
  this->PWMPin1 = leftPwmPin;
  this->PWMPin2 = rightPwmPin;
  this->reversed = reversed;
  
  pinMode(enablePin1, OUTPUT);
  pinMode(enablePin2, OUTPUT);
  pinMode(PWMPin1, OUTPUT);
  pinMode(PWMPin2, OUTPUT);
  
  digitalWrite(enablePin1, LOW);
  digitalWrite(enablePin2, LOW);
  analogWrite(PWMPin1, 0);
  analogWrite(PWMPin2, 0);
}

bool MotorDriver::IsMoving() {
  return (currentAverage > 50);
}

void MotorDriver::Move(int direction, byte speed) {
  if (direction == 0) {
    StopMove();
    return;
  }
  
  if (reversed) {
    direction *= -1;
  }
  
  digitalWrite(enablePin1, HIGH);
  digitalWrite(enablePin2, HIGH);
  if (direction < 0) {
    analogWrite(PWMPin1, speed);
    analogWrite(PWMPin2, 0);
  } else {
    analogWrite(PWMPin1, 0);
    analogWrite(PWMPin2, speed);
  }
}

void MotorDriver::StopMove() {
  digitalWrite(enablePin1, LOW);
  digitalWrite(enablePin2, LOW);
  analogWrite(PWMPin1, 0);
  analogWrite(PWMPin2, 0);
}

Interval motorSenseInterval(5, 64);
void MotorDriver::Sense() {
  currentSum += analogRead(currentSensePin1);
  currentSum += analogRead(currentSensePin2);
  currentSampleCount++;
  
  if (motorSenseInterval.elapsed()) {
    currentAverage = ((double)currentSum / (double)currentSampleCount);
    currentSampleCount = 0;
    currentSum = 0;
  }
}

