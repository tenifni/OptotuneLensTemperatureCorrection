/*May 7 2021
  getLenTemp(): Reads in temperature from Optotune 10-30 tunable lens
  calculates correction current
  toDAC(): converts correction current to DAC input voltage 
*/

#include <Wire.h>
#include <math.h>
#include <SPI.h>

//Sets up to read temperature via I2C from STTS2004 slave 
byte TempSensorI2CAddress = 0x18; //stts004 temperature register address
byte TempReg = 0x05; // Temperature register address
byte ResReg = 0x08;  //Resolution register address

// Reads from STTS2004 temp sensor register
int getTemp() {
  unsigned char tempData[2];
  // Get 16-bit temperature from Temp register
  Wire.beginTransmission(TempSensorI2CAddress);
  Wire.write(TempReg);
  Wire.endTransmission();
  Wire.requestFrom(TempSensorI2CAddress, 2); //requests 2 bytes of data
  tempData[0] = Wire.read(); //MSB
  tempData[1] = Wire.read(); //LSB
  // Converts data to 16-bit. bitRead() starts from rightmost (LSB). //first 4 bits of temp data are flag bits so we don't use them.
  //float temperature = bitRead(tempData[0], 0) * 16 + bitRead(tempData[0], 1) * 32 + bitRead(tempData[0], 2) * 64 +  bitRead(tempData[0], 3) * 128 + bitRead(tempData[0], 4) * (-1);
  //temperature = temperature + bitRead(tempData[1], 0)*0.0625 + bitRead(tempData[1], 1) * 0.125 + bitRead(tempData[1], 2)*0.25  + bitRead(tempData[1], 3)*0.5 + bitRead(tempData[1], 4)*1 + bitRead(tempData[1], 5)*2 + bitRead(tempData[1], 6)*4 + bitRead(tempData[1], 7)*8;
  unsigned int temp=tempData[1];
  temp+=((unsigned int)(tempData[0]&0x1F))<<8;
  float temperature=temp*0.0625;
  return temperature;
//  //Show on serial monitor
//  Serial.print("16-bit  Temperature = ");
//  Serial.print(tempData[0]);
//  Serial.print(tempData[1]);
//  Serial.println();
//  Serial.print("Converted Temperature (degC)   ");
//  Serial.println(temperature);
//  delay(1000);
}

// Calibration data for performing linear interpolation
int current[16] = {-210,  -180,  -150,  -120,   -90,   -60 ,  -30, 0,    30,    60,    90,   120,   150,   180,   210,   240};
float Curve30[16]= {-11.0583,
   -9.8197,
   -8.4629,
   -6.9553,
   -5.2996,
   -3.4974,
   -1.5789,
    0.4126,
    2.3057,
    4.0869,
    5.7214,
    7.2111,
    8.5663,
    9.8051,
   10.9482,
   12.0202};
   
float Curve45[16] = {-10.3719,
   -9.1155,
   -7.7484,
   -6.2424,
   -4.5981,
   -2.8394,
   -0.9859,
    0.9529,
    2.7769,
    4.5248,
    6.1419,
    7.6319,
    8.9982,
   10.2451,
   11.4061,
   12.4819,
};


int n = sizeof(current)/sizeof(current[0]);// len of current array
int firstIdx = 0, lastIdx = n-1;
int maxCurrent = 250, minCurrent = -210;
int temp1 = 30, temp2 = 45;
float Curve[16] = {0};

// Binary search function
int rangeSearch(float a[], float key, int left,int right){
  
  while (left <= right){
    int middle =  left + (right - left)/2;
    if (key > a[middle]){
      left = middle + 1;
    }else if (key < a[middle]){
      right = middle -1;
    }else{
      return middle;
    }
  }
  return (left-1);
}

// Calculates current setpoint given temp and diopter setpoint
int getIset(float temp, float FPset)
{
    int Iset;
  // Checks temperature range, then calculates current setpoint at this temperature, given FP setpiont using linear interpolation 
  if (temp == temp1) {
    //binary search for which range the DptSetpoint is in
    int l = rangeSearch(Curve30, FPset, 0, n-1);
    int r = l +1;
    // linear interploates for the current setpoint in that range
   Iset =abs((current[r]-current[l])*(FPset - Curve30[l]))/(Curve30[r]- Curve30[l]) + current[l];// x = (y-y1)/slope+x1
  } else if (temp == temp2){
    int l = rangeSearch(Curve45, FPset, 0, n-1);
    int r = l +1;
    Iset = abs((current[r]-current[l])*((FPset - Curve45[l])))/(Curve45[r]- Curve45[l]) + current[l];

  } else if (temp < temp1){
    //interpolates to get whole curve at this temp
    for (int i = 0 ; i <=n; i++){
      Curve[i] = Curve30[i] - ((temp1-temp)/abs(temp2-temp1))*(Curve45[i] - Curve30[i]);
    }
    //search for range of the FP setpoint
    int l = rangeSearch(Curve, FPset, 0, n-1);
    int r = l +1;
    //interpolate again to get current
    Iset = abs((current[r]-current[l])*((FPset - Curve[l])))/(Curve[r]- Curve[l]) + current[l];
  } else if (temp > temp2){
    //interpolates to get whole curve at this temp
    for (int i = 0 ; i <=n; i++){
      Curve[i] = Curve45[i] + ((temp-temp2)/abs(temp2-temp1))*(Curve45[i]-Curve30[i]);
    }
    int l = rangeSearch(Curve, FPset, 0, n-1);
    int r = l +1;
    //interpolate again to get current
    Iset = abs((current[r]-current[l])*((FPset - Curve[l])))/(Curve[r]- Curve[l]) + current[l];


  } else{ //in between two temperatures
    int l1 = rangeSearch(Curve30, FPset, 0, n-1); int r1 = l1+1;
    int l2 = rangeSearch(Curve45, FPset, 0, n-1);  int r2 = l2+1;
    int l = min(min(l1,r1), min(l2, r2));
    int r = max(max(l1,r1),max(l2,r2));
   
    for (int i = l; i<=r; i++){
      Curve[i] = Curve30[i] + (temp/abs(temp2-temp1))*(Curve45[i] - Curve30[i]);
    }
    int ll =  rangeSearch(Curve, FPset, l, r)+1;
    int rr = ll+1;
    //interpolate again to get current
    Iset = abs((current[rr]-current[ll])*(FPset - Curve[ll]))/((Curve[rr]- Curve[ll])) + current[ll];
  }
  
  if(Iset>maxCurrent){
      Iset =maxCurrent;
      return Iset;
  }else if(Iset < minCurrent){
      return Iset;
  }else{
      return Iset;
  }
}


// Setup for outputting DAC voltage via SPI protocol
#define DI 11//MOSI
#define SCK 13 //SPI clock
#define SS 10// Slave select
unsigned int Din;
int Vref = 5;

void SPI_setup(){
  pinMode(SS, OUTPUT);
  pinMode(DI, OUTPUT);
  pinMode(SCK, OUTPUT);
  SPI.begin();
  digitalWrite(SS,HIGH);
}

// Transmits voltage data to DAC regsiter 
byte writeToDAC(int data) {
    byte byte1 = (Din >> 8);
    byte byte2 = (Din & 0xFF);
    byte byte0 = 0;
    
    digitalWrite(SS,LOW);//DAC starts reading on falling edge
    SPI.transfer(SS, byte0);
    SPI.transfer(SS, byte1);
    SPI.transfer(SS, byte2);
    digitalWrite(SS,HIGH);//DAC stops reading on falling edge
}



float DptSet = 5.7;
int I = 100;
void setup() {
  Serial.begin(9600);
  Wire.begin();
  // Sets resolution of temperature value to 16-bit.
  Wire.beginTransmission(TempSensorI2CAddress);
  Wire.write(ResReg); // selects resolution register
  Wire.write(0x03); //resolution 0.0625C
  Wire.endTransmission();
  SPI_setup();

}

void loop() {
  int temp = getTemp();// Gets temperature from sensor
  int dI = abs(getIset(temp, DptSet)- I); // Calculates correction current
  Din = (dI * 262.14) * Vref; // converts to DAC voltage
  writeToDAC(Din); // Outputs to DAC
}
