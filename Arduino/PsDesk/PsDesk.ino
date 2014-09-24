#include <Wire.h>
#include <PID_v1.h>
#include <EEPROM.h>
#include "MotorDriver.h"
#include "Interval.h"

#define LEFT_MOTOR_REVERSED false
#define RIGHT_MOTOR_REVERSED true

#define LEFT_MOTOR_ENABLE_PIN_A 8
#define LEFT_MOTOR_ENABLE_PIN_B 9
#define LEFT_MOTOR_ISENSE_PIN_A 2
#define LEFT_MOTOR_ISENSE_PIN_B 3
#define LEFT_MOTOR_PWM_PIN_A 11
#define LEFT_MOTOR_PWM_PIN_B 10

#define RIGHT_MOTOR_ENABLE_PIN_A 4
#define RIGHT_MOTOR_ENABLE_PIN_B 3
#define RIGHT_MOTOR_ISENSE_PIN_A 0
#define RIGHT_MOTOR_ISENSE_PIN_B 1
#define RIGHT_MOTOR_PWN_PIN_A 5
#define RIGHT_MOTOR_PWN_PIN_B 6

#define ACCELEROMETER_SENSE_PIN 5

#define SONAR_TRIGGER_PIN 13
#define SONAR_ECHO_PIN 12

#define TIMER_DIVISOR 64
#define K_P 1.5
#define K_I 0.8
#define K_D 0.05
#define OUT_OF_LEVEL_TRIP 7

MotorDriver leftMotor(LEFT_MOTOR_ENABLE_PIN_A, LEFT_MOTOR_ENABLE_PIN_B,
                      LEFT_MOTOR_ISENSE_PIN_A, LEFT_MOTOR_ISENSE_PIN_B,
                      LEFT_MOTOR_PWM_PIN_A, LEFT_MOTOR_PWM_PIN_B,
                      LEFT_MOTOR_REVERSED);
                      
MotorDriver rightMotor(RIGHT_MOTOR_ENABLE_PIN_A, RIGHT_MOTOR_ENABLE_PIN_B, 
                       RIGHT_MOTOR_ISENSE_PIN_A, RIGHT_MOTOR_ISENSE_PIN_B, 
                       RIGHT_MOTOR_PWN_PIN_A, RIGHT_MOTOR_PWN_PIN_B, 
                       RIGHT_MOTOR_REVERSED);

byte minSpeed = 127, maxSpeed = 200;
int moveDirection = 0;
bool moving = false;
double currentLevel = 0;
double desiredLevel = readDesiredLevelFromEeprom();
double leftLevelBias = 0;
double rightLevelBias = 0;
double levelBias;
Interval pidInterval(50, TIMER_DIVISOR);

PID leftPID(&currentLevel, &leftLevelBias, &desiredLevel, K_P, K_I, K_D, DIRECT);
PID rightPID(&currentLevel, &rightLevelBias, &desiredLevel, K_P, K_I, K_D, REVERSE);

void setup()
{   
  // Set PWM frequency to 62.5kHz
  TCCR0B = TCCR0B & B11111000 | B00000001;
  TCCR1B = TCCR1B & B11111000 | B00000001;
  TCCR2B = TCCR2B & B11111000 | B00000001;
  
  stopMove();

  Serial.begin(19200);
  analogReference(EXTERNAL);
  
  leftPID.SetOutputLimits(0, 40);
  leftPID.SetMode(AUTOMATIC); 
  rightPID.SetOutputLimits(0, 40);
  rightPID.SetMode(AUTOMATIC); 
}

void stopMove() {
  leftMotor.StopMove();
  rightMotor.StopMove();
  moving = false;
  moveDirection = 0;
}

void setMove(int direction) {
  moveDirection = direction;
  leftMotor.Move(direction, maxSpeed);
  rightMotor.Move(direction, maxSpeed);
  delay(200);
  moving = true;
}

bool writeLevelInfo = false;

void loop()
{
  leftMotor.Sense();
  rightMotor.Sense();
  
  while (Serial.available() > 0) {
    byte cmd = Serial.read();
    String s;
    double newDesiredLevel;
    
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
        
      case 'F': // Feed level data continuously
        writeLevelInfo = true;
        Serial.println("OK: LevelDataOutputOn");
        break;
        
      case 'G': // Stop feeding level data
        writeLevelInfo = false;
        Serial.println("OK: LevelDataOutputOff");
        break;
        
      case 'H': // Write desired level to EEPROM
        newDesiredLevel = Serial.parseFloat();
        desiredLevel = writeDesiredLevelToEeprom(newDesiredLevel);
        s = String("OK: SetDesiredLevel=") + desiredLevel;
        Serial.println(s);       
        break;

      case 'I': // Get desired level from EEPROM and output
        desiredLevel = readDesiredLevelFromEeprom();
        s = String("OK: DesiredLevel=") + desiredLevel;
        Serial.println(s);       
        break;
        
      default:
        Serial.println("WTF?");
        break;
      }
  }
  
  heartbeat(writeLevelInfo);
  
  if (!leftMotor.IsMoving() && !rightMotor.IsMoving() && moving) {
    //stopMove();
    //Serial.println("MOVE-COMPLETE");
  }
  
  if (moving && pidInterval.elapsed()) {
    leftPID.Compute();
    rightPID.Compute();
    
    levelBias = leftLevelBias - rightLevelBias;
    
      int avgSpeed = (minSpeed + maxSpeed) / 2;
      int leftSpeed = constrain(avgSpeed + (moveDirection * levelBias), minSpeed, maxSpeed);
      int rightSpeed = constrain(avgSpeed - (moveDirection * levelBias), minSpeed, maxSpeed);
      leftMotor.Move(moveDirection, leftSpeed);
      rightMotor.Move(moveDirection, rightSpeed);
      
      String s = "l=";
      s += (int)leftSpeed;
      s += ", r=";
      s += (int)rightSpeed;
      s += ", bias=";
      s += (int)levelBias;
      Serial.println(s);
    
  }
  
  if (moving) {
    if (abs(currentLevel - desiredLevel) > OUT_OF_LEVEL_TRIP) {
      stopMove();
      Serial.println("OUT_OF_LEVEL");
    }
  }
}

unsigned long accValues = 0;
unsigned long accCount = 0;
Interval accReadInterval(5, TIMER_DIVISOR);
Interval accCalcInterval(100, TIMER_DIVISOR);
Interval heartbeatInterval(1000, TIMER_DIVISOR);

void heartbeat(bool writeLevelInfo) {
  if (accReadInterval.elapsed()) {
    int val = analogRead(ACCELEROMETER_SENSE_PIN);
    accCount++;
    accValues += val;
  }
  
  if (accCalcInterval.elapsed()) {
    currentLevel = (double)accValues / (double)accCount;
    accCount = 0;
    accValues = 0;
    
    if (writeLevelInfo) {
      String s("LEVEL=");
      s += currentLevel;
      Serial.println(s);
    }
  }
  
  if (heartbeatInterval.elapsed()) {
    String s("PSDESK level=");
    s += currentLevel;
    s += " d_level=";
    s += desiredLevel;
    s += " left_bias=";
    s += leftLevelBias;
    s += " right_bias=";
    s += rightLevelBias;
    s += " sonar=N/A";
    Serial.println(s);    
  }
}

double readDesiredLevelFromEeprom() {
  return EEPROM_readDouble(0);
}

double writeDesiredLevelToEeprom(double newValue) {
  EEPROM_writeDouble(0, newValue);
  return EEPROM_readDouble(0);
}

void EEPROM_writeDouble(int ee, double value)
{
    byte* p = (byte*)(void*)&value;
    for (int i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
}

double EEPROM_readDouble(int ee)
{
    double value = 0.0;
    byte* p = (byte*)(void*)&value;
    for (int i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);
    return value;
}

