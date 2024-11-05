#include <EEPROM.h>
// Display setting. Address I2C, number of columns, number of rows.
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Add RigExpert Library
// Set which pins on the RigExpert TX/RX & Start RigExpert
#include "RigExpertZero.h"
#define RX_PIN 4
#define TX_PIN 7
RigExpertZero ZERO(RX_PIN, TX_PIN);

// Configure Keypad
#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {6, 5, 3, 2};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
unsigned long int defaultFreqs[] = {7200, 14250, 18130, 28400};


int32_t timer = millis();             // variable "timer" so as not to use the standard delay function
String input_freq;                    // the frequency we want to test

unsigned long int getEErom(int addr) {
  unsigned long int tmp = 0;
  byte val;
  for (int i = 0; i < sizeof(unsigned long int); i++) {
    val = EEPROM.read(addr + i);
    tmp = tmp << 8 + val;
  }
}
void putEErom(int addr, unsigned long int ulval) {
  for (int i = sizeof(unsigned long int) - 1; i >= 0; i--) {
    byte val = ulval & 0x000000ff;
    ulval = ulval >> 8;
    Serial.print("Putting ");
    Serial.print(val);
    Serial.print(" at ");
    Serial.println(addr + i);
    EEPROM.update(addr + i, val);
  }
}

void setup() {
  
  Serial.begin(9600);                 // Initialize Serial Monitor
  lcd.init();                         // Initialize display
  lcd.backlight();                    // Turn on display backlight

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Rig Expert");
  lcd.setCursor(3, 1);
  lcd.print("AA-30.Zero");
  delay(2000);                         // Display splash screen 2 seconds

  lcd.clear();
  lcd.print("Enter Frequency,");
  lcd.setCursor(0, 1);
  lcd.print("Then #, *=Reset");
  delay(2000);                        // Display instructions 2 seconds

  /* Check if the analyzer is found? */ 
  while (!ZERO.startZero()) {
    lcd.setCursor(4, 0);              // set the cursor
    lcd.print("Analyzer");            // and print text on the display
    lcd.setCursor(3, 1);
    lcd.print("Not Found");
    delay(50000);                     // Hardware problem, delay long enough for the user to notice
  }
  keypad.addEventListener(keypadEvent); // Add an event listener for this keypad
  start_display();
}
void keypadEvent(KeypadEvent key){
  int addr = (key - 'A');
  int eeAddr = addr * sizeof(unsigned long int);

  switch (keypad.getState()){
    case PRESSED:
        switch(key) {
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
          case '0':
            Serial.println(key);
            input_freq += key; // append new character to input freq string
            if (input_freq.length() == 6) {
              start_m();
              interact();
            } else {
              lcd.setCursor(5, 0);
              lcd.print(input_freq);
            }
            break;
          case '*':
            input_freq = "";
            start_display();
            break;
          case '#':
            start_m();
            interact();
            break;
        }
        break;

    case RELEASED:
        if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
          if (input_freq.length() == 0) {
            input_freq = defaultFreqs[addr];
            lcd.setCursor(5, 0);
            lcd.print(input_freq);
 
          }
       }
        break;

    case HOLD:
        if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
            unsigned long int ulVal = input_freq.toInt();
            defaultFreqs[addr] = ulVal;
            putEErom(eeAddr, ulVal);
        }
        break;
  }
}

void loop() {
  char key = keypad.getKey();

  // Update the frequency value on screen every 200 ms.
  if (millis() - timer > 200) {
      lcd.setCursor(5, 0);
      lcd.print(input_freq);
      timer = millis();
  } 
}

void start_display() {
  // Display Interface and start over
  lcd.clear();
  lcd.print("FQ =");
  lcd.setCursor(5, 0);
  lcd.print(input_freq);
  lcd.setCursor(12, 0);
  lcd.print("kHz");
  lcd.setCursor(0, 1);
  lcd.print("Input Frequency");      
}

void interact() {
    Serial.println("interacting");
    bool control_signal = true;
    while (control_signal) {
      char key = keypad.getKey();
      if (key) {
        Serial.println(key);
        switch (key) {
        case '*':
          control_signal = false;
          input_freq = "";
          start_display();
          break;
        case 'A':
          input_freq = 5 + input_freq.toInt();
          start_m();            
          break;
        case 'B':
          input_freq = 100 + input_freq.toInt();
          start_m();            
          break;
        case 'C':
          input_freq = input_freq.toInt() - 5;
          start_m();            
          break;
        case 'D':
          input_freq = input_freq.toInt() - 100;
          start_m();            
          break;
        default:
          // do nothing
          break;
      }
    }
  }
}

void start_m() {
  unsigned long int input_Int = input_freq.toInt();
  unsigned long int freq = input_Int * 1000;  // convert from KHz to Hz for testing

  ZERO.startMeasure(freq);              // start measurement
  float SWR = ZERO.getSWR();            // get SWR value
  int Z = ZERO.getZ();                  // get Z value

  lcd.clear();
  lcd.print("FQ:");
  lcd.setCursor(5, 0);
  lcd.print(input_freq);
  lcd.setCursor(0, 1);
  lcd.print("SWR");
  lcd.setCursor(4, 1);

  if (SWR < 10) {
    lcd.print(SWR, 2);
  } else {
    lcd.print(">10");
  }

  lcd.setCursor(9, 1);
  lcd.print('Z');
  lcd.setCursor(11, 1);
  if (Z <= 1000) {
    lcd.print(Z);
  } else {
    lcd.print(">1000");
  }
 }
