#include <SwitecX12.h>
#include <SwitecX25.h>

#define STEPS 945
#define RANGE 750
#define ZERO 10
#define DELAY1 150
#define DELAY2 150
#define DELAY3 500
int cycleReset = 16;
int cycle = 0;

SwitecX25 speedometer(STEPS,4,5,6,7);

void setup() {
  // put your setup code here, to run once:
  speedometer.zero();
  delay(100);
  speedometer.setPosition(RANGE);
  speedometer.updateBlocking();
  delay(150);
  speedometer.setPosition(ZERO);
  speedometer.updateBlocking();
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  speedometer.setPosition(103);
  speedometer.updateBlocking();
  delay(DELAY1);
  speedometer.setPosition(ZERO);
  speedometer.updateBlocking();
  delay(DELAY2);
  speedometer.setPosition(195);
  speedometer.updateBlocking();
  delay(DELAY1);
  speedometer.setPosition(ZERO);
  speedometer.updateBlocking();
  delay(DELAY1);
  speedometer.setPosition(288);
  speedometer.updateBlocking();
  delay(DELAY1);
  speedometer.setPosition(ZERO);
  speedometer.updateBlocking();
  delay(DELAY2);
  speedometer.setPosition(380);
  speedometer.updateBlocking();
  delay(DELAY1);
  speedometer.setPosition(ZERO);
  speedometer.updateBlocking();
  delay(DELAY2);
  speedometer.setPosition(473);
  speedometer.updateBlocking();
  delay(DELAY1);
  speedometer.setPosition(ZERO);
  speedometer.updateBlocking();
  delay(DELAY2);
  speedometer.setPosition(565);
  speedometer.updateBlocking();
  delay(DELAY1);
  speedometer.setPosition(ZERO);
  speedometer.updateBlocking();
  delay(DELAY2);
  speedometer.setPosition(658);
  speedometer.updateBlocking();
  delay(DELAY1);
  speedometer.setPosition(ZERO);
  speedometer.updateBlocking();
  delay(DELAY2);
  speedometer.setPosition(RANGE);
  speedometer.updateBlocking();
  delay(DELAY1);
  int dispCycle = cycle * 10 + ZERO;
  int thisCycle = map(dispCycle, 0, 160, ZERO, RANGE);
  speedometer.setPosition(thisCycle);
  speedometer.updateBlocking();
  delay(5000);  
  if (cycle < cycleReset) {
    speedometer.setPosition(ZERO);
    speedometer.updateBlocking();
    cycle++;
    delay(DELAY3);
  } else {
    speedometer.zero();
    speedometer.setPosition(ZERO);
    speedometer.updateBlocking();
    cycle = 0;
    delay(DELAY3);
  }
  
  }
