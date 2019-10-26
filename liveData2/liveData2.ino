// https://github.com/clearwater/SwitecX25
#include <SwitecX25.h>

#define STEPS 945  //total steps for motor
#define RANGE 750  //total range for custom gauge
#define ZERO 10  //soft zero to avoid crashing motor at zero data

//switec speedometer instance
SwitecX25 speedometer(STEPS, 3, 4, 5, 6);  

//array for "live" data
const int spd[537] {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 4, 6, 8, 10, 12, 13, 15, 16, 16, 16, 18, 19, 21, 22, 23, 23, 23, 24, 24, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 29, 28, 28, 28, 28, 29, 29, 30, 31, 32, 33, 34, 34, 36, 36, 36, 37, 37, 37, 37, 38, 37, 36, 36, 37, 38, 39, 39, 41, 41, 41, 42, 42, 42, 43, 44, 45, 45, 46, 47, 48, 49, 50, 52, 53, 54, 53, 53, 53, 53, 54, 54, 54, 55, 55, 55, 55, 56, 56, 57, 57, 57, 57, 58, 58, 59, 59, 59, 60, 60, 60, 60, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 66, 66, 66, 66, 66, 66, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 68, 67, 67, 67, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 67, 67, 67, 67, 66, 66, 65, 65, 65, 65, 65, 65, 65, 65, 65, 66, 66, 66, 67, 67, 67, 67, 67, 67, 68, 68, 68, 68, 68, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 68, 68, 68, 68, 68, 67, 67, 67, 67, 66, 66, 66, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 64, 64, 64, 64, 64, 64, 64, 64, 64, 63, 62, 62, 62, 61, 60, 60, 60, 59, 59, 59, 59, 59, 59, 60, 60, 60, 59, 59, 59, 58, 57, 57, 57, 57, 58, 58, 58, 58, 58, 59, 59, 59, 59, 59, 59, 59, 59, 59, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 59, 59, 57, 56, 55, 55, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 54, 54, 54, 54, 53, 52, 52, 52, 52, 52, 52, 52, 53, 53, 53, 54, 54, 53, 53, 52, 52, 52, 52, 51, 50, 50, 50, 50, 49, 49, 48, 48, 47, 47, 46, 45, 45, 44, 44, 43, 42, 42, 41, 41, 40, 40, 39, 39, 39, 38, 38, 38, 37, 37, 37, 37, 37, 37, 36, 36, 36, 36, 35, 35, 34, 34, 34, 33, 32, 30, 29, 29, 29, 28, 27, 27, 26, 25, 24, 22, 21, 19, 18, 18, 16, 15, 14, 13, 12, 10, 9, 7, 5, 3, 2, 1, 1
  };
int updateSPD = 0;     //variable for gauge update interval
//# of arduino cycles to wait for gauge update
int thresholdSPD=2320; 
int i=0;               //index to "live" data array

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  //debug
  speedometer.zero();  //hard zero gauge
  delay(100);
  //set desired position to max gauge range
  speedometer.setPosition(RANGE);
  speedometer.updateBlocking();  //sweep gauge to max range
  delay(150);
  //set desired position to "soft zero"
  speedometer.setPosition(ZERO);  
  speedometer.updateBlocking();  //run gauge to "soft zero"
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  //check if update interval is below threshold
  if (updateSPD < thresholdSPD) {
  updateSPD++;  //increment update interval
  speedometer.update();  //update speedometer
  }
  //if interval 
  else {
    if (i < 439) {
    int speedSteps = map(spd[i], 0, 160, ZERO, RANGE);
    speedometer.setPosition(speedSteps);
    updateSPD = 0;
    i++;
    }
    else {
    i=0;
    }
  }
}
