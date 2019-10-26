/*

*/
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
#include <SoftwareSerial.h>

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
int waterTemp = 0;
int RPM = 0;
int MPH = 0;
int MAF = 0;
int O2Volts = 0;
int throttle = 0;
int fuelLevel = 0;
int manifAbsPres = 0;
int iat = 0;
int barometric = 0;
int ambientAirTemp = 0;
int engineFuelRate = 0;

//update interval/threshold
//update variable
int updateCycle = 0;
//cycles to wait to update
int updateThreshold = 100;

//Menu Variables variables
int menuLevel = 0;  //current menu level
int lastMenuLevel;  //last menu level
int maxMenuLevel = 5;  //highest menu level
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
  delay(100);
  //Display welcome message and wait
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.print(F("CANBUS LCD TEST"));
  delay(1000);

  //Initialize CAN Controller
  CANinit();

  //Clear the LCD and wait
  clearLCD();
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Read the state of the buttons
  int readingUP = digitalRead(UP);
  int readingDN = digitalRead(DOWN);

  //check for state change
  if (readingUP != lastUPstate) {
    lastUPdebounce = millis();
  }
  if (readingDN != lastDNstate) {
    lastDNdebounce = millis();
  }

  //check if UP button state change has
  //stayed changed long enough
  if ((millis() - lastUPdebounce) > debounceDelay) {
    //increase menu level
    if (readingUP != btnStateUP) {
      btnStateUP = readingUP;
      if (btnStateUP == LOW) {
        menuLevel++;
      }
    }
  }

  //check if DOWN button state change has
  //stayed changed long enough
  if ((millis() - lastDNdebounce) > debounceDelay) {
    //increase menu level
    if (readingDN != btnStateDN) {
      btnStateDN = readingDN;
      if (btnStateDN == LOW) {
        menuLevel--;
      }
    }
  }

  //check if highest menu level has been reached
  if (menuLevel > maxMenuLevel) {
    //reset menu level to zero if highest level has been reached
    menuLevel = 0;
  }
  if (menuLevel < 0) {
    menuLevel = maxMenuLevel;
  }

  if (menuLevel != lastMenuLevel) {
    clearLCD();
  }

  if (updateCycle < updateThreshold) {
    updateCycle++;
  }
  else {
    switch (menuLevel) {
      case 0:
        updateMenu0();
        break;
      case 1:
        updateMenu1();
        break;
      case 2:
        updateMenu2();
        break;
      case 3:
        updateMenu3();
        break;
      case 4:
        updateMenu4();
        break;
      case 5:
        updateMenu5();
        break;
    }
    updateCycle = 0;
  }

  //set last state variables to current state
  lastUPstate = readingUP;
  lastDNstate = readingDN;
  lastMenuLevel = menuLevel;
}

//Sketch Functions
void clearLCD(void) {
  sLCD.write(COMMAND);
  sLCD.write(CLEAR);
}

void CANinit(void) {
  clearLCD();
  if (Canbus.init(CANSPEED_500)) {
    sLCD.write(COMMAND);
    sLCD.write(LINE0);
    sLCD.print("  CAN init OK!");
    delay(500);
  }
  else {
    sLCD.write(COMMAND);
    sLCD.write(LINE0);
    sLCD.print("CAN init failed");
    return;
  }
}

void updateMenu0(void) {
  //update variables from ECU for this screen
  MPH = getMPH();
  RPM = getRPM();

  //write variables to LCD
  //write vehicle speed to LCD
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.write("Veh Spd: ");
  sLCD.print(MPH);
  sLCD.print(" MPH ");

  //write RPM to LCD
  sLCD.write(COMMAND);
  sLCD.write(LINE1);
  sLCD.write("En Spd: ");
  sLCD.print(RPM);
  sLCD.print(" RPM ");
  delay(50);
}

void updateMenu1(void) {
  //update variables from ECU for this screen
  waterTemp = getWaterTemp();
  fuelLevel = getFuelLevel();

  //write variables to LCD
  //write temperature
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.write("Wtr tmp:  ");
  sLCD.print(waterTemp);
  sLCD.print((char)223);
  sLCD.print("C ");

  //write fuel level to LCD
  sLCD.write(COMMAND);
  sLCD.write(LINE1);
  sLCD.write("Fuel lvl: ");
  sLCD.print(fuelLevel);
  sLCD.print(" % ");
  delay(50);
}

void updateMenu2(void) {
  //update variables from ECU for this screen
  ambientAirTemp = getAmbientAir();
  iat = getIAT();

  //write variables to LCD
  //write ambient air temperature
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.write("Ambient:   ");
  sLCD.print(ambientAirTemp);
  sLCD.print((char)223);
  sLCD.print("C ");

  //write intake air temp
  sLCD.write(COMMAND);
  sLCD.write(LINE1);
  sLCD.write("Intake:    ");
  sLCD.print(iat);
  sLCD.print((char)223);
  sLCD.print("C ");
  delay(50);
}

void updateMenu3(void) {
  //update variables from ECU for this screen
  barometric = getBaro();
  manifAbsPres = getMAP();

  //write variables to LCD
  //write barometric pressure
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.write("Baro:    ");
  sLCD.print(barometric);
  sLCD.print(" kPa ");

  //write manifold abs. pressure
  sLCD.write(COMMAND);
  sLCD.write(LINE1);
  sLCD.write("MAP:     ");
  sLCD.print(manifAbsPres);
  sLCD.print(" kPa ");
  delay(50);
}

void updateMenu4(void) {
  //update variables from ECU for this screen
  throttle = getThrottle();
  MAF = getMAF();

  //write variables to LCD
  //write throttle position
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.write("Acc pos:   ");
  sLCD.print(throttle);
  sLCD.print(" % ");

  //write mass air flow
  sLCD.write(COMMAND);
  sLCD.write(LINE1);
  sLCD.write("MAF:    ");
  sLCD.print(MAF);
  sLCD.print(" g/s ");
  delay(50);
}

void updateMenu5(void) {
  //update variables from ECU for this screen
  engineFuelRate = getFuelRate();
  O2Volts = getO2Volts();

  //write variables to LCD
  //write engine fuel rate
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.write("Fuel: ");
  sLCD.print(engineFuelRate);
  sLCD.print(" gal/hr     ");

  //write O2 voltage
  sLCD.write(COMMAND);
  sLCD.write(LINE1);
  sLCD.write("O2:       ");
  sLCD.print(MAF);
  sLCD.print(" V ");
  delay(50);
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

int getMAF() {
  //Request mass air flow reading, store it to buffer.
  //[gram/s]
  Canbus.ecu_req(MAF_SENSOR, buffer);
  //Convert information stored in buffer to integer
  int result04 = atoi(buffer);
  return result04;
}

int getO2Volts() {
  //Request O2 voltage, store it to buffer.
  //[%]
  Canbus.ecu_req(O2_VOLTAGE, buffer);
  //Convert information stored in buffer to integer
  int result05 = atoi(buffer);
  return result05;
}

int getThrottle() {
  //Request throttle position, store it to buffer.
  //[%]
  Canbus.ecu_req(THROTTLE, buffer);
  //Convert information stored in buffer to integer
  int result06 = atoi(buffer);
  return result06;
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

int getIAT() {
  //Request intake air temperature, store it to buffer.
  //[degree C]
  Canbus.ecu_req(IAT, buffer);
  //Convert information stored in buffer to integer
  int result09 = atoi(buffer);
  return result09;
}

int getBaro() {
  //Request barometric pressure, store it to buffer.
  //[kPa (absolute)]
  Canbus.ecu_req(BAROMETRIC, buffer);
  //Convert information stored in buffer to integer
  int result10 = atoi(buffer);
  return result10;
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
