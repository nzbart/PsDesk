#include <Wire.h>
#include "MotorDriver.h"

#define LEFT_MOTOR_REVERSED false
#define RIGHT_MOTOR_REVERSED true

MotorDriver leftMotor(8, 9, 2, 3, 11, 10, LEFT_MOTOR_REVERSED);
MotorDriver rightMotor(4, 3, 0, 1, 5, 6, RIGHT_MOTOR_REVERSED);

byte minSpeed = 127, maxSpeed = 200;
int moveDirection = 0;

void setup()
{   
  // Set PWM frequency to 62.5kHz
  TCCR0B = TCCR0B & B11111000 | B00000001;
  TCCR1B = TCCR1B & B11111000 | B00000001;
  TCCR2B = TCCR2B & B11111000 | B00000001;

  Serial.begin(19200);
  
  stopMove();
}

void stopMove() {
  leftMotor.StopMove();
  rightMotor.StopMove();
}

bool moving = false;
void setMove(int direction) {
  leftMotor.Move(direction, maxSpeed);
  rightMotor.Move(direction, maxSpeed);
  delay(200);
  moving = true;
}

void loop()
{
  while (Serial.available() > 0) {
    byte cmd = Serial.read();
    
    switch (cmd) {
      case '!': // STOP!
        stopMove();
        Serial.println("OK: Stop");
        break;
        
      case 'U': // Move up
        setMove(1);
        Serial.println("OK: MoveUp");
        break;
        
      case 'D': // Move down
        setMove(-1);
        Serial.println("OK: MoveDown");
        break;
        
      default:
        Serial.println("WTF?");
        break;
      }
  }
  
  heartBeat();
  
  if (!leftMotor.IsMoving() && !rightMotor.IsMoving() && moving) {
    moving = false;
    Serial.println("MOVE-COMPLETE");
  }
}

unsigned long prevMillis = millis();
void heartBeat(){
  unsigned long nowMillis = millis();
  if ((nowMillis - prevMillis) > 50000) {
    Serial.println("PSDESK-HEARTBEAT");
    prevMillis = nowMillis;
  }
}

