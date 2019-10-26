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

// Button variables
int menuLevel = 0;  //current menu level
int maxMenuLevel = 4;  //highest menu level
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

  //Initialize Serial LCD
  sLCD.begin(9600);
  clearLCD();
  delay(100);
  //Display welcome message and wait
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.print(F("   Menu Demo!   "));
  delay(1000);

  clearLCD();
  sLCD.write(COMMAND);
  sLCD.write(LINE0);
  sLCD.print(F("  Menu Level:   "));
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

  //check if UP button state change has stayed changed long enough
  if ((millis() - lastUPdebounce) > debounceDelay) {
    //increase menu level
    if (readingUP != btnStateUP) {
      btnStateUP = readingUP;
      if (btnStateUP == LOW) {
        menuLevel++;
      }
    }
  }

  //check if DOWN button state change has stayed changed long enough
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

  //print menu level on LCD
  sLCD.write(COMMAND);
  sLCD.write(LINE1);
  sLCD.print(F("                "));
  sLCD.write(COMMAND);
  sLCD.write(LINE1 + 8);
  sLCD.print(menuLevel);

  lastUPstate = readingUP;
  lastDNstate = readingDN;
}

void clearLCD(void) {
  sLCD.write(COMMAND);
  sLCD.write(CLEAR);
}
