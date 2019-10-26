// https://github.com/clearwater/SwitecX25
#include <SwitecX25.h>
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

//Declare variables to be used:
//variable to store data retrieved from ECU/PCM
char buffer[512];

//local ECU PID variables
int MPH = 0;

#define STEPS 945  //total steps for motor
#define RANGE 750  //total range for custom gauge
#define ZERO 10  //soft zero to avoid crashing motor at zero data

//switec speedometer instance
SwitecX25 speedometer(STEPS, 3, 4, 5, 6);

void setup() {
  // put your setup code here, to run once:
  Canbus.init(CANSPEED_500);

  //hard zero gauge
  speedometer.zero();
  delay(100);

  //set desired position to max gauge range
  speedometer.setPosition(RANGE);
  speedometer.updateBlocking();  //run gauge to max range
  delay(100);

  //set desired position to "soft zero"
  speedometer.setPosition(ZERO);
  speedometer.updateBlocking();  //run gauge to "soft zero"
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  MPH = getMPH();
  int speedStep = map(MPH, 0, 160, ZERO, RANGE);
  speedometer.setPosition(speedStep);
  speedometer.update();
}

int getMPH() {
  //request vehicle speed, store to buffer
  //vehicle speed in kilometers per hour [km/h]
  Canbus.ecu_req(VEHICLE_SPEED, buffer);
  //convert information stored in buffer to integer
  //convert km/h to miles per hour [MPH]
  int result03 = atoi(buffer) / 1.60934;
  return result03;
}
