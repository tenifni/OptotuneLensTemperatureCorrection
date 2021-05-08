/*
  Reading in temperature from Optotune 10-30 tunable lens
  May 3 2021
  ====================== Wire.h library ======================================
  Wire.beginTransmission(): begin the transmission to a particualr slave
  Wire.write(); asks for particular data from the registers of the slave
  Wire.endTransmission(): terminate the transmisison and transmits the data from the registers
  Wire.requestFrom(); requests the transmitted data from the regusters
  Wire.available(); returns the number of bytes available for retrieval
  Wire.read(); if the number of bytes available match with out requested btyes, this function will rea the bytes from the resgiters

  ======================= STTS2004 Temperature sensor registers=================
  The temperature sensor has many programmable registers, and each needs to write its address to the pointer regsiter. 
  The list of address of all the registers in the sensors are listed on Table 4 in STTS2004 datasheet.
  We're interested in getting data from the read-only *Temperature register* (Hex address 05).

  Temperature data format: 16-bit (2 bytes) with BIT 12-2 = Tmperature (default 10bit). But solution can go to 0.0625.
  |  BIT 15  |  BIT 14  |  BIT 13  |BIT 12|BIT 11|BIT 10|BIT 9|BIT 8|BIT 7|BIT 6|BIT 5|BIT 4|BIT 3 |BIT 2  |BIT 1 |BIT 0 |
  | Flag bit | Flag bit | Flag bit | Sign | 128  |  64  | 32  | 16  |  8  |  4  |  2  |  1  | 0.5 | 0.25 | 0.125  |0.0625|  DegC/LSB
  Eg. 10bit data (Bit 12-2): 00111110000 = 0(positvie), 0*128+ 1*64 + 1*32 + 1*16 + 0*8+ 0*4+ 0*2+ 0*1+ 0*0.5+ 0*0.25 =124 degC

  If we want to go change the defaul 10-bit resolution, we can change the status of the *Resolution register* (Hex address 08)
  The resolution register bits are given on Table 23 on STTS2004 datasheet.
  Bits 9  (00) = 0.5 degC/LSB
  Bits 10 (01) = 0.25 degC/LSB
  Bits 11 (10) = 0.125 degC/LSB
  Bits 12 (11) = 0.0625 degC/LSB
  The default is 01 (10-bit). Setting it to '11' (12-bit) will read the temperatures to nearest 0.0625 degC.
*/

#include <Wire.h>
int TempSensorI2CAddress = 0x18; //stts2994 temperature register address
byte TempResigter = 0x05; // Temperature register address
byte ResolutionResiger = 0x08;  //Resolution register address
byte *tempVal;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  int n = Wire.available();
  Serial.println(n); // prints number of bytes available for retrieval

  // Sets resultion of temperature data to 16-bit.
  Wire.beginTransmission(TempSensorI2CAddress);
  Wire.write(ResolutionResiger); // selects resolutino register
  Wire.write(0x11); //resolution 0.0625C
  Wire.endTransmission();
}

void getTemp() {
  unsigned int tempData[2];
  // Get 16-bit temperature from Temp register
  Wire.beginTransmission(TempSensorI2CAddress);
  Wire.write(TempResigter);// select temp register
  Wire.endTransmission();
  Wire.requestFrom(TempSensorI2CAddress, 2); //requests 2 bytes of data
  tempData[0] = Wire.read(); //MSB
  tempData[1] = Wire.read(); //LSB
  // Converts data to 16-bit. bitRead() starts from rightmost (LSB). //first 4 bits of temp data are flag bits so we don't use them.
  int temperature = bitRead(tempData[0], 0) * 16 + bitRead(tempData[0], 1) * 32 + bitRead(tempData[0], 2) * 64 +  bitRead(tempData[0], 3) * 128 + bitRead(tempData[0], 4) * (-1);
  temperature = temperature + bitRead(tempData[1], 0)*0.0625 + bitRead(tempData[1], 1) * 0.125 + bitRead(tempData[1], 2)*0.25  + bitRead(tempData[1], 3)*0.5 + bitRead(tempData[1], 4)*1 + bitRead(tempData[1], 5)*2 + bitRead(tempData[1], 6)*4 + bitRead(tempData[1], 7)*8;
  
  //Show on serial monitor
  Serial.println("Raw Temperature = ");
  Serial.print(tempData[0]);
  Serial.print(tempData[1]);
  Serial.println(" Converted Temperature (degC)   ");
  Serial.print(temperature);
  delay(500);
}

void loop() {
  getTemp();
}
