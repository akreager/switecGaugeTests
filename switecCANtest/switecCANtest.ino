// https://github.com/clearwater/SwitecX25
#include <SwitecX25.h>
#include <Canbus.h>

#define STEPS 945  //total steps for motor
#define RANGE 750  //total range for custom gauge
#define ZERO 10  //soft zero to avoid crashing motor at zero data

//switec speedometer instance
SwitecX25 speedometer(STEPS, 3, 4, 5, 6);

int updateSPD = 0;     //variable for gauge update interval
//# of arduino cycles to wait for gauge update
int thresholdSPD = 2320;

//array to store data retrieved from ECU
char buffer[512];

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);  //debug

  //Initilize CANBUS at 500kbps
  if (!Canbus.init(CANSPEED_500)) {
    error01();
  }

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
    int MPH = getMPH();
    int speedStep = map(MPH, 0, 160, ZERO, RANGE);
    speedometer.setPosition(speedStep);
    updateSPD = 0;
  }
}

int getWaterTemp() {
  //Request water temperature, store it to buffer.
  //[degree C]
  Canbus.ecu_req(ENGINE_COOLANT_TEMP, buffer);
  //Convert information stored in buffer to integer
  int result01 = atoi(buffer);
  return result01;
}

int getRPM() {
  //request engine speed, store to buffer
  //engine speed in revolutions per minute [RPM]
  Canbus.ecu_req(ENGINE_RPM, buffer);
  //convert information stored in buffer to integer
  int result02 = atoi(buffer);
  return result02;
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

int getFuelLevel() {
  //Request fuel level, store it to buffer.
  //[%]
  Canbus.ecu_req(FUEL_LEVEL, buffer);
  //Convert information stored in buffer to integer
  int result07 = atoi(buffer);
  return result07;
}

int getMAP() {
  //Request manifold absolute Pressure, store it to buffer.
  //[kPa (gauge)]
  Canbus.ecu_req(MAP, buffer);
  //Convert information stored in buffer to integer
  int result08 = atoi(buffer);
  return result08;
}

int getAmbientAir() {
  //Request ambient air temperature, store it to buffer.
  //[degree C]
  Canbus.ecu_req(AMBIENT_AIR, buffer);
  //Convert information stored in buffer to integer
  int result11 = atoi(buffer);
  return result11;
}

int getFuelRate() {
  //Request engine fuel rate, store it to buffer.
  //[gal/h]
  Canbus.ecu_req(ENGINE_FUEL_RATE, buffer);
  //Convert information stored in buffer to integer
  int result12 = atoi(buffer);
  return result12;
}

//error conditions
void error01() {
  int errPos = 10;
  errPos = map(errPos, 0, 160, ZERO, RANGE);
  speedometer.setPosition(errPos);
  while (1) {
    speedometer.update();
  }
}

void error02() {
  int errPos = 20;
  errPos = map(errPos, 0, 160, ZERO, RANGE);
  speedometer.setPosition(errPos);
  while (1) {
    speedometer.update();
  }
}

void error03() {
  int errPos = 30;
  errPos = map(errPos, 0, 160, ZERO, RANGE);
  speedometer.setPosition(errPos);
  while (1) {
    speedometer.update();
  }
}
