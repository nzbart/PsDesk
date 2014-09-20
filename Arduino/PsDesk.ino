#include <Wire.h>

#define LEFT_MOTOR_EN1            8  
#define LEFT_MOTOR_EN2            9  
#define LEFT_MOTOR_PWM_L          10
#define LEFT_MOTOR_PWM_R          11
#define LEFT_MOTOR_I1             0
#define LEFT_MOTOR_I2             1

void setup()
{
   // motor LEFT
   pinMode(LEFT_MOTOR_EN1, OUTPUT);
   digitalWrite(LEFT_MOTOR_EN1, LOW);
   pinMode(LEFT_MOTOR_EN2, OUTPUT);
   digitalWrite(LEFT_MOTOR_EN2, LOW);
   pinMode(LEFT_MOTOR_PWM_L, OUTPUT);
   digitalWrite(LEFT_MOTOR_PWM_L, LOW);
   pinMode(LEFT_MOTOR_PWM_R, OUTPUT);
   digitalWrite(LEFT_MOTOR_PWM_R, LOW);
   
   Serial.begin(19200);
}

bool moving = false;
int lastByte = 0;
void loop()
{
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    
    if (incomingByte == '\n') {
      if (lastByte == 'U') {
        setMove(1);
        delay(200);
        Serial.println("OK U");
      }
      else if (lastByte == 'D') {
        setMove(0);
        delay(200);
        Serial.println("OK D");
      } 
      else {
        stopMove();
        Serial.println("OK ?");
      }
    }
    
    lastByte = incomingByte;
  }
  
  int v1 = analogRead(LEFT_MOTOR_I1);
  int v2 = analogRead(LEFT_MOTOR_I2);
  
  if (moving && (v1 < 50 && v2 < 50)) {
    moving = false;
    Serial.println("DONE");
    stopMove();
  } else if (!moving && (v1 > 50 || v2 > 50)) {
    moving = true;
    Serial.println("START");
  } 
}
/*--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------*/

void setMove(byte direction) {
  digitalWrite(LEFT_MOTOR_EN1, HIGH);
  digitalWrite(LEFT_MOTOR_EN2, HIGH);  
  if (direction) {
    digitalWrite(LEFT_MOTOR_PWM_L, HIGH);
    digitalWrite(LEFT_MOTOR_PWM_R, LOW);  
  } else {
    digitalWrite(LEFT_MOTOR_PWM_R, HIGH);
    digitalWrite(LEFT_MOTOR_PWM_L, LOW);  
  }
}

void stopMove() {
  digitalWrite(LEFT_MOTOR_EN1, LOW);
  digitalWrite(LEFT_MOTOR_EN2, LOW);
  digitalWrite(LEFT_MOTOR_PWM_L, LOW);
  digitalWrite(LEFT_MOTOR_PWM_R, LOW);
}
