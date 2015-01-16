#include <Wire.h>
#include <SPI.h>  //I have no idea why Adxl345Accelerometer.cpp won't compile unless I put this include here!
#include "MotorDriver.h"
#include "Adxl345Accelerometer.h"

#define LEFT_MOTOR_REVERSED true
#define RIGHT_MOTOR_REVERSED true

#define LEFT_MOTOR_ENABLE_PIN_A 8  //any digital pin
#define LEFT_MOTOR_ENABLE_PIN_B 7  //any digital pin
#define LEFT_MOTOR_ISENSE_PIN_A 2  //analog
#define LEFT_MOTOR_ISENSE_PIN_B 3  //analog
#define LEFT_MOTOR_PWM_PIN_A 6     //PWM capable digital; usually has a tilde next to the pin on the PCB
#define LEFT_MOTOR_PWM_PIN_B 9     //PWM capable digital; usually has a tilde next to the pin on the PCB

#define RIGHT_MOTOR_ENABLE_PIN_A 4  //any digital pin
#define RIGHT_MOTOR_ENABLE_PIN_B 2  //any digital pin
#define RIGHT_MOTOR_ISENSE_PIN_A 0  //analog
#define RIGHT_MOTOR_ISENSE_PIN_B 1  //analog
#define RIGHT_MOTOR_PWM_PIN_A 5     //PWM capable digital; usually has a tilde next to the pin on the PCB
#define RIGHT_MOTOR_PWM_PIN_B 3     //PWM capable digital; usually has a tilde next to the pin on the PCB

int ACCELEROMETER_CS_PIN=10;        //any digital pin

#define OUT_OF_LEVEL_TRIPLEVEL 20

MotorDriver leftMotor(LEFT_MOTOR_ENABLE_PIN_A, LEFT_MOTOR_ENABLE_PIN_B,
LEFT_MOTOR_ISENSE_PIN_A, LEFT_MOTOR_ISENSE_PIN_B,
LEFT_MOTOR_PWM_PIN_A, LEFT_MOTOR_PWM_PIN_B,
LEFT_MOTOR_REVERSED);

MotorDriver rightMotor(RIGHT_MOTOR_ENABLE_PIN_A, RIGHT_MOTOR_ENABLE_PIN_B, 
RIGHT_MOTOR_ISENSE_PIN_A, RIGHT_MOTOR_ISENSE_PIN_B, 
RIGHT_MOTOR_PWM_PIN_A, RIGHT_MOTOR_PWM_PIN_B, 
RIGHT_MOTOR_REVERSED);

#define MAX_INT 32767
int accelerometerLevel = MAX_INT;
Adxl345Accelerometer accelerometer(ACCELEROMETER_CS_PIN);

void setup()
{   
  // Set PWM frequency to 62.5kHz
  TCCR0B = TCCR0B & B11111000 | B00000001;
  TCCR1B = TCCR1B & B11111000 | B00000001;
  TCCR2B = TCCR2B & B11111000 | B00000001;

  stopMove();

  Serial.begin(19200);
}

void stopMove() {
  leftMotor.StopMove();
  rightMotor.StopMove();
}

void loop()
{
  while (Serial.available() > 0) {
    byte cmd = Serial.read();
    String s;

    switch (cmd) {
    case '!': // STOP!
      stopMove();
      Serial.println("Stopped.");
      break;

    case 'u': // Move up
      Serial.println("Moving up...");
      accelerometerLevel = MAX_INT;
      leftMotor.Move(1, 255);
      rightMotor.Move(1, 255);
      break;

    case 'd': // Move down
      Serial.println("Moving down...");
      accelerometerLevel = MAX_INT;     
      leftMotor.Move(-1, 255);
      rightMotor.Move(-1, 255);
      break;

    default:
      Serial.println("Unrecognised user command.");
      break;
    }
  }

  const int x = accelerometer.GetX();

  if(accelerometerLevel == MAX_INT)
    accelerometerLevel = x;  //calibrate

  if(abs(accelerometerLevel - x) > OUT_OF_LEVEL_TRIPLEVEL)
  {
   stopMove(); 
   Serial.println("EMERGENCY STOP INITIATED BY ACCELEROMETER!");
  }
}
