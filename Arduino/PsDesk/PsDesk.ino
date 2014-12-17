#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>

#include <Wire.h>
#include <PID_v1.h>
#include <EEPROM.h>
#include "MotorDriver.h"
#include "Interval.h"
#include "Sonar.h"

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

#define SONAR_TRIGGER_PIN 2
#define SONAR_ECHO_PIN 7

#define TIMER_DIVISOR 64
#define OUT_OF_LEVEL_TRIPLEVEL 5
#define OUT_OF_LEVEL_TRIPTIME 500

MotorDriver leftMotor(LEFT_MOTOR_ENABLE_PIN_A, LEFT_MOTOR_ENABLE_PIN_B,
LEFT_MOTOR_ISENSE_PIN_A, LEFT_MOTOR_ISENSE_PIN_B,
LEFT_MOTOR_PWM_PIN_A, LEFT_MOTOR_PWM_PIN_B,
LEFT_MOTOR_REVERSED);

MotorDriver rightMotor(RIGHT_MOTOR_ENABLE_PIN_A, RIGHT_MOTOR_ENABLE_PIN_B, 
RIGHT_MOTOR_ISENSE_PIN_A, RIGHT_MOTOR_ISENSE_PIN_B, 
RIGHT_MOTOR_PWN_PIN_A, RIGHT_MOTOR_PWN_PIN_B, 
RIGHT_MOTOR_REVERSED);

Sonar sonar(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN);

double upSpeedFactor = 0.85, downSpeedFactor = 0.75;
int moveDirection = 0;
bool moving = false;
double currentHeight = 0;
double currentLevel = 0;
double desiredLevel = readDesiredLevelFromEeprom();

unsigned long height = 0;
unsigned long desiredHeight = 0;

Fuzzy* fuzzy = new Fuzzy();

void setup()
{   
  // Set PWM frequency to 62.5kHz
  TCCR0B = TCCR0B & B11111000 | B00000001;
  TCCR1B = TCCR1B & B11111000 | B00000001;
  TCCR2B = TCCR2B & B11111000 | B00000001;

  // Fuzzy input
  FuzzySet* tooFarLeft  = new FuzzySet(-5, -5, -4, -3);
  FuzzySet* tooLeft     = new FuzzySet(-4, -3, -1, 0);
  FuzzySet* good        = new FuzzySet(-1, 0, 0, 1);
  FuzzySet* tooRight    = new FuzzySet(0, 1, 3, 4);
  FuzzySet* tooFarRight = new FuzzySet(3, 4, 5, 5);
  FuzzyInput* level = new FuzzyInput(1);
  level->addFuzzySet(tooFarLeft);
  level->addFuzzySet(tooLeft);
  level->addFuzzySet(good);
  level->addFuzzySet(tooRight);
  level->addFuzzySet(tooFarRight);  
  fuzzy->addFuzzyInput(level);

  // Fuzzy output
  FuzzySet* off = new FuzzySet(0, 0, 127, 191);
  FuzzySet* slow = new FuzzySet(63, 127, 191, 255);
  FuzzySet* fast = new FuzzySet(127, 191, 255, 255);

  FuzzyOutput* leftMotor = new FuzzyOutput(1);
  leftMotor->addFuzzySet(off);
  leftMotor->addFuzzySet(slow);
  leftMotor->addFuzzySet(fast);
  fuzzy->addFuzzyOutput(leftMotor);

  FuzzyOutput* rightMotor = new FuzzyOutput(2);
  rightMotor->addFuzzySet(off);
  rightMotor->addFuzzySet(slow);
  rightMotor->addFuzzySet(fast);
  fuzzy->addFuzzyOutput(rightMotor);

  // Rules
  addFuzzyMotorRule(fuzzy, tooFarLeft, off, fast, 10);
  addFuzzyMotorRule(fuzzy, tooLeft, slow, fast, 20);
  addFuzzyMotorRule(fuzzy, good, fast, fast, 30);
  addFuzzyMotorRule(fuzzy, tooRight, fast, slow, 40);
  addFuzzyMotorRule(fuzzy, tooFarRight, fast, off, 50);

  stopMove();

  Serial.begin(19200);
  analogReference(EXTERNAL);
}

void addFuzzyMotorRule(Fuzzy* fuzz, FuzzySet* antecedent, FuzzySet* leftConsequent, FuzzySet* rightConsequent, int ruleSetNumber) {
  addFuzzyRule(fuzz, antecedent, leftConsequent, ruleSetNumber + 1);
  addFuzzyRule(fuzz, antecedent, rightConsequent, ruleSetNumber + 2);
}

void addFuzzyRule(Fuzzy* fuzz, FuzzySet* antecedent, FuzzySet* consequent, int ruleNumber) {
  FuzzyRuleAntecedent* ifThis = new FuzzyRuleAntecedent();
  ifThis->joinSingle(antecedent);
  FuzzyRuleConsequent* thenThat = new FuzzyRuleConsequent();
  thenThat->addOutput(consequent);
  FuzzyRule* rule = new FuzzyRule(ruleNumber, ifThis, thenThat);
  fuzz->addFuzzyRule(rule);
}

void stopMove() {
  leftMotor.StopMove();
  rightMotor.StopMove();
  moving = false;
  moveDirection = 0;
}

byte getSpeed(byte desired, int direction) {
  double speedFactor = 1;
  
  if (moveDirection > 0) {
    speedFactor = upSpeedFactor;
  } 
  else {
    speedFactor = downSpeedFactor;
  }
  
  return (byte)((double)desired * speedFactor);
}

void setMove() {
  int direction = 0;
  if (height < desiredHeight) {
    direction = 1;
  }
  else if (height > desiredHeight) {
    direction = -1;
  } 
  else {
    return;
  }

  moveDirection = direction;
  leftMotor.Move(direction, getSpeed(255, moveDirection));
  rightMotor.Move(direction, getSpeed(255, moveDirection));
  delay(200);
  moving = true;
}

bool writeLevelInfo = false;
bool writeHeightInfo = false;
Interval outOfLevelCheckInterval(OUT_OF_LEVEL_TRIPTIME, 64);

void loop()
{
  leftMotor.Sense();
  rightMotor.Sense();

  sonar.Run();
  if (sonar.MeasurementAvailable()) {
    height = sonar.GetLastMeasurement();
  }

  while (Serial.available() > 0) {
    byte cmd = Serial.read();
    String s;
    double newDesiredLevel;

    switch (cmd) {
    case '!': // STOP!
      stopMove();
      Serial.println("OK: Stop");
      break;

    case 'M': // Move
      desiredHeight = Serial.parseInt();
      setMove();
      s = "OK: MoveTo=";
      s += desiredHeight;
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
      s = String("OK: DesiredLevel=") + desiredLevel + " DesiredHeight=" + desiredHeight;
      Serial.println(s);       
      break;

    case 'J': // Feed height data continuously
      writeHeightInfo = true;
      Serial.println("OK: HeightDataOutputOn");
      break;

    case 'K': // Stop feeding height data
      writeHeightInfo = false;
      Serial.println("OK: HeightDataOutputOff");
      break;

    default:
      Serial.println("WTF?");
      break;
    }
  }

  heartbeat(writeLevelInfo);

  if (moving) {
    currentLevel;
    desiredLevel;
    double level = (desiredLevel - currentLevel);

    fuzzy->setInput(1, level);
    fuzzy->fuzzify();
    
    float leftMotorSpeed = getSpeed(fuzzy->defuzzify(1), moveDirection);
    float rightMotorSpeed = getSpeed(fuzzy->defuzzify(2), moveDirection);
    leftMotor.Move(moveDirection, leftMotorSpeed);
    rightMotor.Move(moveDirection, rightMotorSpeed);
  }

  if (moving) {
    if (moveDirection < 0 && height <= desiredHeight) {
      stopMove();
      Serial.println("MOVE-COMPLETE");

    } 
    else if (moveDirection > 0 && height >= desiredHeight) {
      stopMove();
      Serial.println("MOVE-COMPLETE");
    }

    if (abs(currentLevel - desiredLevel) > OUT_OF_LEVEL_TRIPLEVEL) {
      if (outOfLevelCheckInterval.elapsed()) {
        stopMove();
        Serial.println("OUT_OF_LEVEL");
      }
    } 
    else {
      outOfLevelCheckInterval.reset();
    }
  }
}

unsigned long accValues = 0;
unsigned long accCount = 0;
unsigned long heightValues = 0;
unsigned long heightCount = 0;
Interval accReadInterval(5, TIMER_DIVISOR);
Interval accCalcInterval(100, TIMER_DIVISOR);
Interval heartbeatInterval(1000, TIMER_DIVISOR);

void heartbeat(bool writeLevelInfo) {
  if (accReadInterval.elapsed()) {
    int val = analogRead(ACCELEROMETER_SENSE_PIN);
    accCount++;
    accValues += val;

    heightCount++;
    heightValues += height;
  }

  if (accCalcInterval.elapsed()) {
    currentLevel = (double)accValues / (double)accCount;
    accCount = 0;
    accValues = 0;
    currentHeight = (double)heightValues / (double)heightCount;
    heightValues = 0;
    heightCount = 0;

    if (writeLevelInfo) {
      String s("LEVEL=");
      s += currentLevel;
      Serial.println(s);
    }

    if (writeHeightInfo) {
      String s("HEIGHT=");
      s += currentHeight;
      Serial.println(s);
    }
  }

  if (heartbeatInterval.elapsed()) {
    String s("PSDESK");
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


