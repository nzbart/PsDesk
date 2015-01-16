#include <Wire.h>
#include <EEPROM.h>
#include <SPI.h>
#include "MotorDriver.h"
#include "Interval.h"

#define LEFT_MOTOR_REVERSED true
#define RIGHT_MOTOR_REVERSED true

#define LEFT_MOTOR_ENABLE_PIN_A 8
#define LEFT_MOTOR_ENABLE_PIN_B 7
#define LEFT_MOTOR_ISENSE_PIN_A 2  //analog
#define LEFT_MOTOR_ISENSE_PIN_B 3  //analog
#define LEFT_MOTOR_PWM_PIN_A 6     //PWM capable digital
#define LEFT_MOTOR_PWM_PIN_B 9     //PWM capable digital

#define RIGHT_MOTOR_ENABLE_PIN_A 4
#define RIGHT_MOTOR_ENABLE_PIN_B 2
#define RIGHT_MOTOR_ISENSE_PIN_A 0  //analog
#define RIGHT_MOTOR_ISENSE_PIN_B 1  //analog
#define RIGHT_MOTOR_PWN_PIN_A 5     //PWM capable digital
#define RIGHT_MOTOR_PWN_PIN_B 3     //PWM capable digital

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

//ACCELEROMETER BEGIN
#define MAX_INT 32767
int accelerometerLevel = MAX_INT;

//Assign the Chip Select signal to pin 10.
int CS=10;

//This is a list of some of the registers available on the ADXL345.
//To learn more about these and the rest of the registers on the ADXL345, read the datasheet!
char POWER_CTL = 0x2D;	//Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;	//X-Axis Data 0
char DATAX1 = 0x33;	//X-Axis Data 1
char DATAY0 = 0x34;	//Y-Axis Data 0
char DATAY1 = 0x35;	//Y-Axis Data 1
char DATAZ0 = 0x36;	//Z-Axis Data 0
char DATAZ1 = 0x37;	//Z-Axis Data 1

//This function will write a value to a register on the ADXL345.
//Parameters:
//  char registerAddress - The register to write a value to
//  char value - The value to be written to the specified register.
void writeRegister(char registerAddress, char value){
  //Set Chip Select pin low to signal the beginning of an SPI packet.
  digitalWrite(CS, LOW);
  //Transfer the register address over SPI.
  SPI.transfer(registerAddress);
  //Transfer the desired register value over SPI.
  SPI.transfer(value);
  //Set the Chip Select pin high to signal the end of an SPI packet.
  digitalWrite(CS, HIGH);
}

//This function will read a certain number of registers starting from a specified address and store their values in a buffer.
//Parameters:
//  char registerAddress - The register addresse to start the read sequence from.
//  int numBytes - The number of registers that should be read.
//  char * values - A pointer to a buffer where the results of the operation should be stored.
void readRegister(char registerAddress, int numBytes, char * values){
  //Since we're performing a read operation, the most significant bit of the register address should be set.
  char address = 0x80 | registerAddress;
  //If we're doing a multi-byte read, bit 6 needs to be set as well.
  if(numBytes > 1)address = address | 0x40;
  
  //Set the Chip select pin low to start an SPI packet.
  digitalWrite(CS, LOW);
  //Transfer the starting register address that needs to be read.
  SPI.transfer(address);
  //Continue to read registers until we've read the number specified, storing the results to the input buffer.
  for(int i=0; i<numBytes; i++){
    values[i] = SPI.transfer(0x00);
  }
  //Set the Chips Select pin high to end the SPI packet.
  digitalWrite(CS, HIGH);
}
//ACCELEROMETER END

double upSpeedFactor = 1, downSpeedFactor = 0.75;

enum { MOVE_DIRECTION_UP, MOVE_DIRECTION_DOWN, MOVE_DIRECTION_NONE } moveDirection = MOVE_DIRECTION_NONE;

void setup()
{   
  // Set PWM frequency to 62.5kHz
  TCCR0B = TCCR0B & B11111000 | B00000001;
  TCCR1B = TCCR1B & B11111000 | B00000001;
  TCCR2B = TCCR2B & B11111000 | B00000001;

  stopMove();

  Serial.begin(19200);
  
  //ACCEL
    //Initiate an SPI communication instance.
  SPI.begin();
  //Configure the SPI connection for the ADXL345.
  SPI.setDataMode(SPI_MODE3);
  //Set up the Chip Select pin to be an output from the Arduino.
  pinMode(CS, OUTPUT);
  //Before communication starts, the Chip Select pin needs to be set high.
  digitalWrite(CS, HIGH);
  //Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register.
  writeRegister(DATA_FORMAT, 0x01);
  //Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeRegister(POWER_CTL, 0x08);  //Measurement mode  
  //ACCEL
}

void stopMove() {
  leftMotor.StopMove();
  rightMotor.StopMove();
  moveDirection = MOVE_DIRECTION_NONE;
}

bool writeLevelInfo = false;
bool writeHeightInfo = false;
Interval outOfLevelCheckInterval(OUT_OF_LEVEL_TRIPTIME, 64);

void loop()
{
  leftMotor.Sense();
  rightMotor.Sense();

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
      Serial.println("WTF?");
      break;
    }
  }

//ACCEL
  char values[10];
  //Reading 6 bytes of data starting at register DATAX0 will retrieve the x,y and z acceleration values from the ADXL345.
  //The results of the read operation will get stored to the values[] buffer.
  readRegister(DATAX0, 6, values);
  //The ADXL345 gives 10-bit acceleration values, but they are stored as bytes (8-bits). To get the full value, two bytes must be combined for each axis.
  //The X value is stored in values[0] and values[1].
  const int x = ((int)values[1]<<8)|(int)values[0];
//ACCEL

  if(accelerometerLevel == MAX_INT)
    accelerometerLevel = x;  //calibrate

  if(abs(accelerometerLevel - x) > 20)
  {
   stopMove(); 
   Serial.println("EMERGENCY STOP INITIATED BY ACCELEROMETER!");
  }
}
