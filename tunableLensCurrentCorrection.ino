/*
  DAC output voltage according to compensation current
  May 6 2021
  ======================= AD5060 DAC =================
  The DAC is 16-bit, so the oputput voltage has range 0-65535V (2^16 posisble values). The DAC is supplied with a 5V reference voltage  with from REF5050.
  Vout =  Vin* Vref/65535
  Mapping (-250,250)mA to 65536 is 262.14 scale factor
  Temperature data format: 16-bit (2 bytes) with BIT 12-2 = Tmperature (default 10bit). But solution can go to 0.0625.
  |  BIT 15  | ... |BIT 0 |
  |   MSB    | ... | LSB  |  D
  Eg.  mapped voltage 36811 (= 2^15+2^11+2^10+2^9+2^8 +2^7+2^6+2^3+2+1) = 10001111 11001011
*/

#include <SPI.h>

#define DI 11//MOSI
#define SCK 13 //SPI clock
#define SS 8// Slave select

int dI = 150;
float myVolt = dI/250;
int Vref = 5;

void SPI_setup(){
  pinMode(SS, OUTPUT);
//  pinMode(DI, OUTPUT);
//  pinMode(SCK, OUTPUT);
  SPI.begin();
  SPI.setDataMode(SPI_MODE1);
  digitalWrite(SS,HIGH);
}
void writeToDAC() {
    unsigned int Din = (myVolt+Vref)*65535./10;
    byte byte1 = (Din >> 8);
    byte byte2 = (Din & 0xFF);
    byte byte0 = 0;
    

    digitalWrite(SS,LOW);//DAC starts reading on falling edge
    SPI.transfer(byte0);
    SPI.transfer(byte1);
    SPI.transfer(byte2);
    digitalWrite(SS,HIGH);//DAC stops reading on falling edge

    Serial.print("Binary: ");
    Serial.println(byte1, BIN);
    Serial.println(byte2, BIN);

    Serial.print("Correction Current: ");
    Serial.println(dI, DEC);
    Serial.print("Output voltage: ");
    Serial.println(Din);
//    Serial.print("Mapped Voltage (V): ");
//    Serial.println(Din*10/65535 -Vref, DEC);
}
void setup() {
  Serial.begin(9600);
  SPI_setup();
   
}
void loop() {
  
  writeToDAC();

  delay(3000);
}
