#include <math.h>
int I =200;
float temp = 35.6;
int FPset = 5;

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

int rangeSearch(float a[], float key, int right, int left){
  int middle = (left+right)/2;
  while (left < = right){
    if (key > a[middle]){
      left = middle + 1;
    }else if (key < a[middle]){
      rigth = middle -1;
    }else{
      return middle;
    }
  }
  return (left-1);
}
int main()
{

  // Checks temperature range, then calculates current setpoint at this temperature, given FP setpiont using linear interpolation 
  if (temp == temp1) {
    //binary search for which range the DptSetpoint is in
    int l = rangeSearch(Curve30, FPset, 0, n-1);
    int r = l +1;
    // linear interploates for the current setpoint in that range
   int Iset = ((Curve30[r]-Curve30[l])/(current[r]-current[l]))*(FPset - Curve30[l]) + current[l];// x = slope*(y-y1)+x1
  } else if (temp == temp2){
    int l = rangeSearch(Curve45, FPset, 0, n-1);
    int r = l +1;
    int Iset = ((Curve45[r]-Curve45[l])/(current[r]-current[l]))*(FPset - Curve45[l]) + current[l];// x = slope*(y-y1)+x1
  } else if (temp < temp1){
    //interpolates to get whole curve at this temp
    for (int i = 0 ; i <n; i++){
      Curve[i] = Curve30[i] - ((temp1-temp)/abs(temp2-temp1))*(Curve45[i] - Curve30[i]);
    }
    //search for range of the FP setpoint
    int l = rangeSearch(Curve, FPset, 0, n-1);
    int r = l +1;
    //interpolate again to get current
    Iset = ((Curve[r]- Curve[l])/(current[r]-current[l]))*(Curve[r]-Curve[l]) + Curve[l];

  } else if (temp > temp 2){
    //interpolates to get whole curve at this temp
    for (int i = 0 ; i <n; i++){
      Curve[i] = Curve45[i] + ((temp-temp2)/abs(temp2-temp1))*(Curve45[i]-Curve30[i]);
    }
    int l = rangeSearch(Curve, FPset, 0, n-1);
    int r = l +1;
    //interpolate again to get current
    Iset = (current[r]-current[l])/((Curve[r]- Curve[l]))*(Curve[r]-FPset) + current[l];

  } else{int
    int l1 = rangeSearch(Curve30, FPset, 0, n-1); int r1 = l1+1;
    int l2 = rangeSearch(Curve45, FPset, 0, n-1);  r2 = l2+1;
    int l = min(min(l1,r1), min(l2, r2);
    r = max(max(l1,r1),max(l2,r2);
    for (int i = l; i<r; i++){
      Curve[i] = Curve30[i] + (temp/abs(temp2-temp1))*(Curve45[i] - Curve30[i]);
    }
    int ll =  rangeSearch(Curve, FPset, 0, n-1);
    int rr = ll+1;
    //interpolate again to get current
    Iset = (current[r]-current[l])/((Curve[r]- Curve[l]))*(Curve[r]-FPset) + current[l];
  }
  // Calculates correction current
  dI = abs(Iset-I);

}

