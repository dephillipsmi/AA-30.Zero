
#include <Keypad.h>
/* Added a library to work with the display. */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// SUBSCRIBE TO HAM RADIO DUDE PLEASE.  My Mom says her favorite youtuber is KK6USY.
// https://www.youtube.com/c/hamradiodude
// Get your Day Started Right with COFFEE AND HAM RADIOS - Weekend Mornings at 0900 Central on YouTube!
//
//
// OTHER RIG EXPERT FUNCTIONS CAN BE CALLED:
//
//     Serial.print("Fq: ");
//        Serial.print(temp);
//        Serial.print(", R: ");
//        Serial.print(ZERO.getR());
//        Serial.print(", Rp: ");
//        Serial.print(ZERO.getRp());
//        Serial.print(", X: ");
//        Serial.print(ZERO.getX());
//        Serial.print(", Xp: ");
//        Serial.print(ZERO.getXp());
//        Serial.print(", SWR: ");
//        Serial.print(ZERO.getSWR());
//        Serial.print(", RL: ");
//        Serial.print(ZERO.getRL());
//        Serial.print(", Z: ");
//        Serial.print(ZERO.getZ());
//        Serial.print(", Phase: ");
//        Serial.print(ZERO.getPhase());
//        Serial.print(", Rho: ");
//        Serial.print(ZERO.getRho());
//        Serial.print("\r\n");

// The Possibilities are out there, lets see what you got!

// There is a lot of re used code here from Rig Expert, The Arduino 4x4 Keypad Library Examples, and whatever else.
// I like Comments.

/* Display setting. Address I2C, number of columns, number of rows. */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* Added a library to work with AA-30 ZERO analyzer */
#include "RigExpertZero.h"


const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

/* setting the center frequency of the measurement, Hz . */
unsigned long int freq = 0;

/* variable "timer" so as not to use the standard delay function */
int32_t timer = millis();

/* ZERO pins  The standard through rig expert would not work with the keypad hooked up.  Or at least it was cleaner this way. */
#define RX_PIN 12
#define TX_PIN 13

RigExpertZero ZERO(RX_PIN, TX_PIN);



Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
String input_freq;

void setup() {
  Serial.begin(9600);
  ZERO.startZero();                   // Initialize analyzer
  delay(50);                          // required delay
  lcd.init();                         // Initialize display
  delay(50);                          // required delay
  lcd.backlight();                    // Turn on display backlight

  /* Check if the analyzer is found? */
  while (!ZERO.startZero()) {
    lcd.setCursor(4, 0);              // set the cursor
    lcd.print("Analyzer");            // and print text on the display
    lcd.setCursor(3, 1);
    lcd.print("Not Found");
    delay(1000);                      // recheck after 1 second
  }

  /* If found, then display the greeting on the screen. */
  lcd.setCursor(1, 0);
  lcd.print("The Dude Expert");
  lcd.setCursor(4, 1);
  lcd.print("Analyzer");
  delay(4000);                         // Display splash screen 4 seconds
  lcd.clear();                         // Clear display

  lcd.setCursor(0, 1);
  lcd.print("Input Frequency");

  // input_freq.reserve(7); // maximum input characters is 6
}



void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.println(key);

    if (key == '*') {
      clear_lcd();
      Serial.println("Frequeny cleared");
    } else if (key == 'A') {
      input_freq = "7200";
      start_m();
    } else if (key == 'B') {
      input_freq = "14250";
      start_m();
    } else if (key == 'C') {
      input_freq = "18130";
      start_m();
    } else if (key == 'D') {
      input_freq = "28400";
      start_m();
    } else if (key == '*') {
      clear_lcd();
      loop();
    } else if (key == '#') {
      start_m();
    } else {
      input_freq += key; // append new character to input freq string
      // I need to convert a String to a integer 07/25/2021 SK
      //int input_int = input_freq.toInt();
      //      Serial.println(input_int);
    }
  }
  /* Update the frequency value on screen every 200 ms. */
  if (millis() - timer > 200) {
    if ((freq / 1000) > 10000) {
      lcd.setCursor(0, 0);
      lcd.print("FQ = ");
      lcd.setCursor(5, 0);
      lcd.print(input_freq);
      lcd.setCursor(12, 0);
      lcd.print("kHz");
      timer = millis();
    }
    if ((freq / 1000) < 9999) {
      lcd.setCursor(0, 0);
      lcd.print("FQ = ");
      lcd.setCursor(9, 0);
      lcd.print(' ');
      lcd.setCursor(5, 0);
      lcd.print(input_freq);
      lcd.setCursor(12, 0);
      lcd.print("kHz");
      timer = millis();
    }
  }
}

void clear_lcd() {
  input_freq = "";
  lcd.clear();
  setup();

}
void start_m() {
  unsigned long int input_Int = input_freq.toInt();
  unsigned long int convert_hz;
  convert_hz = input_Int * 1000;
  freq = convert_hz;
  unsigned long int fiveup;
  unsigned long int fivedown;
  int x = 0;
  int display_freq;
  Serial.println(freq);
  ZERO.startMeasure(freq);              // start measurement
  lcd.clear();
  delay(10);
  lcd.setCursor(1, 0);
  lcd.print("FQ:");
  lcd.setCursor(4, 0);
  lcd.print(input_Int);
  Serial.println("LCD Output Freq in start_m() Function");
  delay(10);

  // lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("SWR");
  float SWR = ZERO.getSWR();            // get SWR value
  lcd.setCursor(4, 1);
  char key = keypad.getKey();


  if (SWR < 10) {
    lcd.print(SWR, 2);
  }
  if ((SWR >= 10) && (SWR < 100)) {
    lcd.print(SWR, 1);
  }
  if ((SWR >= 100) && (SWR <= 200)) {
    lcd.print(">100");
  }


  int Z = ZERO.getZ();                  // get Z value
  lcd.setCursor(9, 1);
  lcd.print('Z');
  lcd.setCursor(11, 1);
  if (Z <= 1000) {
    lcd.print(Z);
  }
  if (Z > 1000 ) {
    lcd.print(">1000");


  }

  while (x == 0) {
    key = keypad.getKey();
    if (key == 'A') {
      freq = freq + 5000;
      input_Int = freq;
      display_freq = input_Int / 1000;
      Serial.println(freq);
      ZERO.startMeasure(freq);              // start measurement
      lcd.clear();
      delay(10);
      lcd.setCursor(1, 0);
      lcd.print("FQ:");
      lcd.setCursor(4, 0);
      lcd.print(display_freq);
      delay(10);

      // lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("SWR");
      float SWR = ZERO.getSWR();            // get SWR value
      lcd.setCursor(4, 1);
      char key = keypad.getKey();


      if (SWR < 10) {
        lcd.print(SWR, 2);
      }
      if ((SWR >= 10) && (SWR < 100)) {
        lcd.print(SWR, 1);
      }
      if ((SWR >= 100) && (SWR <= 200)) {
        lcd.print(">100");
      }


      int Z = ZERO.getZ();                  // get Z value
      lcd.setCursor(9, 1);
      lcd.print('Z');
      lcd.setCursor(11, 1);
      if (Z <= 1000) {
        lcd.print(Z);
      }
      if (Z > 1000 ) {
        lcd.print(">1000");


        x = 1;
      }

      Serial.println("fiveup");
    } if (key == 'B') {
      freq = freq - 5000;
      input_Int = freq;
      display_freq = input_Int / 1000;
      Serial.println(freq);
      ZERO.startMeasure(freq);              // start measurement
      lcd.clear();
      delay(10);
      lcd.setCursor(1, 0);
      lcd.print("FQ:");
      lcd.setCursor(4, 0);
      lcd.print(display_freq);
      delay(10);

      // lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("SWR");
      float SWR = ZERO.getSWR();            // get SWR value
      lcd.setCursor(4, 1);
      char key = keypad.getKey();


      if (SWR < 10) {
        lcd.print(SWR, 2);
      }
      if ((SWR >= 10) && (SWR < 100)) {
        lcd.print(SWR, 1);
      }
      if ((SWR >= 100) && (SWR <= 200)) {
        lcd.print(">100");
      }


      int Z = ZERO.getZ();                  // get Z value
      lcd.setCursor(9, 1);
      lcd.print('Z');
      lcd.setCursor(11, 1);
      if (Z <= 1000) {
        lcd.print(Z);
      }
      if (Z > 1000 ) {
        lcd.print(">1000");


        x = 1;
      }

      Serial.println("fiveDown");
    } if (key == 'C') {
      freq = freq + 50000;
      input_Int = freq;
      display_freq = input_Int / 1000;
      Serial.println(freq);
      ZERO.startMeasure(freq);              // start measurement
      lcd.clear();
      delay(10);
      lcd.setCursor(1, 0);
      lcd.print("FQ:");
      lcd.setCursor(4, 0);
      lcd.print(display_freq);
      delay(10);

      // lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("SWR");
      float SWR = ZERO.getSWR();            // get SWR value
      lcd.setCursor(4, 1);
      char key = keypad.getKey();


      if (SWR < 10) {
        lcd.print(SWR, 2);
      }
      if ((SWR >= 10) && (SWR < 100)) {
        lcd.print(SWR, 1);
      }
      if ((SWR >= 100) && (SWR <= 200)) {
        lcd.print(">100");
      }


      int Z = ZERO.getZ();                  // get Z value
      lcd.setCursor(9, 1);
      lcd.print('Z');
      lcd.setCursor(11, 1);
      if (Z <= 1000) {
        lcd.print(Z);
      }
      if (Z > 1000 ) {
        lcd.print(">1000");


        x = 1;
      }

      Serial.println("fiveup");

    } if (key == 'D') {
      freq = freq - 50000;
      input_Int = freq;
      display_freq = input_Int / 1000;
      Serial.println(freq);
      ZERO.startMeasure(freq);              // start measurement
      lcd.clear();
      delay(10);
      lcd.setCursor(1, 0);
      lcd.print("FQ:");
      lcd.setCursor(4, 0);
      lcd.print(display_freq);
      delay(10);

      // lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("SWR");
      float SWR = ZERO.getSWR();            // get SWR value
      lcd.setCursor(4, 1);
      char key = keypad.getKey();


      if (SWR < 10) {
        lcd.print(SWR, 2);
      }
      if ((SWR >= 10) && (SWR < 100)) {
        lcd.print(SWR, 1);
      }
      if ((SWR >= 100) && (SWR <= 200)) {
        lcd.print(">100");
      }


      int Z = ZERO.getZ();                  // get Z value
      lcd.setCursor(9, 1);
      lcd.print('Z');
      lcd.setCursor(11, 1);
      if (Z <= 1000) {
        lcd.print(Z);
      }
      if (Z > 1000 ) {
        lcd.print(">1000");


        x = 1;
      }
    } if (key == '*') {
      x = 0;
      break;
    }
  }
  clear_lcd();
}
