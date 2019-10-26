#include <SwitecX12.h>
#include <SwitecX25.h>

#define STEPS 945
#define RANGE 750
#define ZERO 10

SwitecX25 speedometer(STEPS,4,5,6,7);

void setup() {
  // put your setup code here, to run once:
  speedometer.zero();
  delay(2000);
  speedometer.setPosition(RANGE);
  speedometer.updateBlocking();
  delay(150);
  speedometer.setPosition(ZERO);
  speedometer.updateBlocking();
}

void loop() {
  // put your main code here, to run repeatedly:

}
