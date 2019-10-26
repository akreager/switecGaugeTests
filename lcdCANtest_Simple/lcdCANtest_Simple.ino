/*

*/
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
#include <SoftwareSerial.h>

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
int MPH = 0;

//update interval/threshold
//update variable
int updateCycle = 0;
//cycles to wait to update
int updateThreshold = 100;

void setup() {
  // put your setup code here, to run once:
  //Initialize software serial LCD
  sLCD.begin(9600);

  //clear LCD
  clearLCD();
  delay(100);

  //Display welcome message and wait
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.print(F("CANBUS LCD TEST"));
  sLCD.write(COMMAND);
  sLCD.write(LINE1);
  sLCD.print(F("     Simple"));
  delay(1000);

  //Initialize CAN Controller
  CANinit();

  //Clear the LCD and wait
  clearLCD();
  delay(100);

  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.write("Vehicle Speed:");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (updateCycle < updateThreshold) {
    updateCycle++;
  }
  else {
    Canbus.ecu_req(VEHICLE_SPEED, buffer);
    //convert information stored in buffer to integer
    //convert km/h to miles per hour [MPH]
    int result02 = atoi(buffer) / 1.60934;
    float result03 = atoi(buffer) / 1.60934;

    //write vehicle speed to LCD
    sLCD.write(COMMAND);
    sLCD.write(LINE1);
    sLCD.print(result02);
    sLCD.print(" MPH  ");
    sLCD.write(COMMAND);
    sLCD.write(LINE1 + 9);
    sLCD.print(result03);
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
