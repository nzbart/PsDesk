#include "Adxl345Accelerometer.h"  
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

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

void writeRegister(int chipSelectPin, char registerAddress, char value)
{
    digitalWrite(chipSelectPin, LOW);
    SPI.transfer(registerAddress);
    SPI.transfer(value);
    digitalWrite(chipSelectPin, HIGH);
}

void readRegister(int chipSelectPin, char registerAddress, int numBytes, char* values)
{
    char address = 0x80 | registerAddress;
    if(numBytes > 1)address = address | 0x40;
    digitalWrite(chipSelectPin, LOW);
    SPI.transfer(address);
    for(int i=0; i<numBytes; i++){
        values[i] = SPI.transfer(0x00);
    }
    digitalWrite(chipSelectPin, HIGH);
}

Adxl345Accelerometer::Adxl345Accelerometer(int chipSelectPin)
{
    _chipSelectPin = chipSelectPin;

    SPI.begin();
    SPI.setDataMode(SPI_MODE3);
    pinMode(_chipSelectPin, OUTPUT);
    digitalWrite(_chipSelectPin, HIGH);
    writeRegister(_chipSelectPin, DATA_FORMAT, 0x00);
    //Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
    writeRegister(_chipSelectPin, POWER_CTL, 0x08);  //Measurement mode  
}

int Adxl345Accelerometer::GetX()
{
    char values[10];
    //Reading 6 bytes of data starting at register DATAX0 will retrieve the x,y and z acceleration values from the ADXL345.
    //The results of the read operation will get stored to the values[] buffer.
    readRegister(_chipSelectPin, DATAX0, 6, values);
    //The ADXL345 gives 10-bit acceleration values, but they are stored as bytes (8-bits). To get the full value, two bytes must be combined for each axis.
    //The X value is stored in values[0] and values[1].
    const int x = ((int)values[1]<<8)|(int)values[0];
    
    return x;
}
