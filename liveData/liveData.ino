// https://github.com/clearwater/SwitecX25
#include <SwitecX25.h>

#define STEPS 945  //total steps for motor
#define RANGE 750  //total range for custom gauge
#define ZERO 10  //soft zero to avoid crashing motor at zero data

//switec speedometer instance
SwitecX25 speedometer(STEPS, 3, 4, 5, 6);  

//array for "live" data
const int spd[439] {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 3, 4, 5, 6, 7, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 4, 4, 3, 3, 3, 3, 2, 2, 1, 1, 1, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 10, 10, 11, 12, 13, 13, 14, 15, 15, 15, 15, 15, 16, 16, 17, 18, 19, 20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 27, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 29, 29, 29, 29, 29, 29, 29, 28, 28, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 36, 36, 36, 36, 36, 35, 34, 34, 34, 34, 33, 33, 33, 33, 33, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 29, 29, 28, 27, 27, 26, 26, 25, 24, 24, 22, 22, 21, 21, 20, 19, 18, 18, 17, 16, 15, 15, 12, 11, 10, 9, 9, 8, 7, 6, 6, 5, 4, 3, 3, 2, 1, 1, 1, 0
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
