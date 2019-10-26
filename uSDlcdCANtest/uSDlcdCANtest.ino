/*

*/
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
#include <SoftwareSerial.h>
#include <SD.h>

Sd2Card card;
SdVolume volume;
SdFile root;
SdFile file;
int SD_CS = 9;

//Define Joystick connection pins
#define UP     A1
#define DOWN   A3
#define LEFT   A2
#define RIGHT  A5
#define CLICK  A4

//Create software serial instance for LCD
SoftwareSerial sLCD = SoftwareSerial(3, 6);
#define COMMAND 0xFE
#define CLEAR   0x01
#define LINE0   0x80
#define LINE1   0xC0

//Declare variables to be used:
//variable to store data retrieved from ECU/PCM
char buffer[512];

//local ECU PID variables
int waterTemp;
int RPM;
int MPH;
int MAF;
int O2Volts;
int throttle;
int fuelLevel;
int manifAbsPres;
int iat;
int barometric;
int ambientAirTemp;
int engineFuelRate;

//update interval/threshold
//update variable
int updateCycle = 0;
//cycles to wait to update
int updateThreshold = 500;

//sketch state variables
//assume all functions are deactivated.
bool CANinitialized = false;
bool uSDinitialized = false;

//Menu Variables variables
int menuLevel = 0;  //current menu level
int maxMenuLevel = 3;  //highest menu level
int btnStateUP;  //current up button state
int btnStateDN;  //current down button state
int lastUPstate;  //last up button state
int lastDNstate;  //last down button state

//Debounce counters
unsigned long lastUPdebounce = 0;
unsigned long lastDNdebounce = 0;
unsigned long debounceDelay = 50;

void setup() {
  // put your setup code here, to run once:
  //Initialize analog pins as inputs
  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(CLICK, INPUT);

  //Pull analog pins high to enable reading of joystick movements
  digitalWrite(UP, HIGH);
  digitalWrite(DOWN, HIGH);
  digitalWrite(LEFT, HIGH);
  digitalWrite(RIGHT, HIGH);
  digitalWrite(CLICK, HIGH);

  //Initialize software serial LCD
  sLCD.begin(9600);
  //clear LCD
  clearLCD();
  delay(500);
  //Display welcome message and wait
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.print(F("  Hello World!"));
  delay(1000);

  //Initialize CAN Controller
  CANinit();

  //Initialize uSD card
  //uSDinit();

  //Clear the LCD and wait
  clearLCD();
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (updateCycle < updateThreshold) {
    updateCycle++;
  }
  else {
    if (uSDinitialized) {
      logData();
    }
    if (CANinitialized) {
      switch (menuLevel) {
        case 0:
          updateLCD_0();
          break;
        case 1:
          updateLCD_1();
          break;
        case 2:
          updateLCD_2();
          break;
        case 3:
          updateLCD_3();
          break;
      }
      updateCycle = 0;
    }
  }
}

//Sketch Functions
void clearLCD(void) {
  sLCD.write(COMMAND);
  sLCD.write(CLEAR);
}

void CANinit(void) {
  clearLCD();
  if (Canbus.init(CANSPEED_500)) {
    CANinitialized = true;
    sLCD.write(COMMAND);
    sLCD.write(LINE0);
    sLCD.print(F("  CAN init OK!"));
    delay(500);
  }
  else {
    sLCD.write(COMMAND);
    sLCD.write(LINE0);
    sLCD.print(F("CAN init failed"));
    return;
  }
}

void uSDinit(void) {
  pinMode(SD_CS, OUTPUT);
  clearLCD();
  if (SD.begin(SD_CS)) {
    uSDinitialized = true;
    sLCD.write(COMMAND);
    sLCD.write(LINE0);
    sLCD.print(F("  uSD init OK!"));
    delay(500);
  }
  else {
    sLCD.write(COMMAND);
    sLCD.write(LINE0);
    sLCD.print(F("uSD init failed"));
    return;
  }
}

void dataError(void) {
  clearLCD();
  while (CLICK == HIGH) {
    sLCD.write(COMMAND);
    sLCD.write(LINE0);
    sLCD.print(F("  No data from  "));
    sLCD.write(COMMAND);
    sLCD.write(LINE1);
    sLCD.print(F("     CANbus     "));
  }
  menuLevel = 0;
}

void logData(void) {
  //update variables from ecu
  waterTemp = getWaterTemp();
  RPM = getRPM();
  MPH = getMPH();
  MAF = getMAF();
  O2Volts = getO2Volts();
  throttle = getThrottle();
  fuelLevel = getFuelLevel();
  manifAbsPres = getMAP();
  iat = getIAT();
  barometric = getBaro();
  ambientAirTemp = getAmbientAir();
  engineFuelRate = getFuelRate();

  //write values to uSD
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) {
    //dataFile.print(millis());
    //dataFile.print(",");
    dataFile.print(waterTemp);
    dataFile.print(",");
    dataFile.print(RPM);
    dataFile.print(",");
    dataFile.print(MPH);
    dataFile.print(",");
    //dataFile.print(ODO);
    //dataFile.print(",");
    //dataFile.print(secondsPerMile * miles);
    //dataFile.print(",");
    dataFile.print(MAF);
    dataFile.print(",");
    dataFile.print(O2Volts);
    dataFile.print(",");
    dataFile.print(throttle);
    dataFile.print(",");
    dataFile.print(fuelLevel);
    dataFile.print(",");
    dataFile.print(manifAbsPres);
    dataFile.print(",");
    dataFile.print(iat);
    dataFile.print(",");
    dataFile.print(barometric);
    dataFile.print(",");
    dataFile.print(ambientAirTemp);
    dataFile.print(",");
    dataFile.print(engineFuelRate);
    //dataFile.print(",");
    //dataFile.println(millis());
    dataFile.close();
  }
}

void updateLCD_0(void) {
  //update variables from ECU for this screen
  RPM = getRPM();
  MPH = getMPH();

  //write RPM data to LCD
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.print(RPM);
  sLCD.print(F(" RPM    "));

  //write vehicle speed data to LCD
  sLCD.write(COMMAND);
  sLCD.write(LINE1);
  sLCD.print(MPH);
  sLCD.print(F(" MPH    "));
}

void updateLCD_1(void) {
  //update variables from ECU for this screen
  waterTemp = getWaterTemp();
  RPM = getRPM();
  MPH = getMPH();
  fuelLevel = getFuelLevel();

  //write variables to LCD
  //write RPM data to LCD
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.print(RPM);
  sLCD.print(F(" RPM    "));

  //write temperature data to LCD
  sLCD.write(COMMAND);
  sLCD.write(LINE0 + 9);
  sLCD.print(waterTemp);
  sLCD.print((char)223);
  sLCD.print(F("C    "));

  //write vehicle speed data to LCD
  sLCD.write(COMMAND);
  sLCD.write(LINE1);
  sLCD.print(MPH);
  sLCD.print(F(" MPH    "));

  //write fuel level data to LCD
  sLCD.write(COMMAND);
  sLCD.write(LINE1 + 9);
  sLCD.print(fuelLevel);
  sLCD.print(F(" %    "));
}

void updateLCD_2(void) {

}

void updateLCD_3(void) {

}

int getWaterTemp() {
  //Request water temperature, store it to buffer.
  //[degree C]
  if (Canbus.ecu_req(ENGINE_COOLANT_TEMP, buffer) == true) {
    //Convert information stored in buffer to integer
    int result = atoi(buffer);
    return result;
  }
  else {
    dataError();
  }
}

int getRPM() {
  //request engine speed, store to buffer
  //engine speed in revolutions per minute [RPM]
  Canbus.ecu_req(ENGINE_RPM, buffer);
  //convert information stored in buffer to integer
  int result = atoi(buffer);
  return result;
}

int getMPH() {
  //request vehicle speed, store to buffer
  //vehicle speed in kilometers per hour [km/h]
  Canbus.ecu_req(VEHICLE_SPEED, buffer);
  //convert information stored in buffer to integer
  //convert km/h to miles per hour [MPH]
  int result = atoi(buffer) / 1.60934;
  return result;
}

int getMAF() {
  //Request mass air flow reading, store it to buffer.
  //[gram/s]
  if (Canbus.ecu_req(MAF_SENSOR, buffer) == 1) {
    //Convert information stored in buffer to integer
    int result = atoi(buffer);
    return result;
  }
  else {
    dataError();
  }
}

int getO2Volts() {
  //Request O2 voltage, store it to buffer.
  //[%]
  if (Canbus.ecu_req(O2_VOLTAGE, buffer) == 1) {
    //Convert information stored in buffer to integer
    int result = atoi(buffer);
    return result;
  }
  else {
    dataError();
  }
}

int getThrottle() {
  //Request throttle position, store it to buffer.
  //[%]
  if (Canbus.ecu_req(THROTTLE, buffer) == 1) {
    //Convert information stored in buffer to integer
    int result = atoi(buffer);
    return result;
  }
  else {
    dataError();
  }
}

int getFuelLevel() {
  //Request fuel level, store it to buffer.
  //[%]
  if (Canbus.ecu_req(FUEL_LEVEL, buffer) == true) {
    //Convert information stored in buffer to integer
    int result = atoi(buffer);
    return result;
  }
  else {
    dataError();
  }
}

int getMAP() {
  //Request manifold absolute Pressure, store it to buffer.
  //[kPa (gauge)]
  if (Canbus.ecu_req(MAP, buffer) == 1) {
    //Convert information stored in buffer to integer
    int result = atoi(buffer);
    return result;
  }
  else {
    dataError();
  }
}

int getIAT() {
  //Request intake air temperature, store it to buffer.
  //[degree C]
  if (Canbus.ecu_req(IAT, buffer) == 1) {
    //Convert information stored in buffer to integer
    int result = atoi(buffer);
    return result;
  }
  else {
    dataError();
  }
}

int getBaro() {
  //Request barometric pressure, store it to buffer.
  //[kPa (absolute)]
  if (Canbus.ecu_req(BAROMETRIC, buffer) == 1) {
    //Convert information stored in buffer to integer
    int result = atoi(buffer);
    return result;
  }
  else {
    dataError();
  }
}

int getAmbientAir() {
  //Request ambient air temperature, store it to buffer.
  //[degree C]
  if (Canbus.ecu_req(AMBIENT_AIR, buffer) == 1) {
    //Convert information stored in buffer to integer
    int result = atoi(buffer);
    return result;
  }
  else {
    dataError();
  }
}

int getFuelRate() {
  //Request engine fuel rate, store it to buffer.
  //[gal/h]
  if (Canbus.ecu_req(ENGINE_FUEL_RATE, buffer) == 1) {
    //Convert information stored in buffer to integer
    int result = atoi(buffer);
    return result;
  }
  else {
    dataError();
  }
}
