/*
    July 16, 2020
    This will be use to explore the use of 4x4 keypad for
      1. T9 text entry
      2. A, B, C, and D will be use for the menu operations

    Author: George Bantique ( TechToTinker )
*/

#include "LiquidCrystal.h"
#include "Keypad.h"


#define DEFAULT_DELAY 300

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

int x = 0;                // Holds the LCD x position
int y = 0;                // Holds the LCD y position
int minValue = 0;         // Lower character location for T9 text entry
int maxValue = 0;         // Max character location for T9 text entry
int keyPressTime = 100;   // Number of loops check of the key
String msg = "";          // Holds the created message
String num = "";          // Holds the mobile number
String alpha = "!@_$%?1 ABC2 DEF3 GHI4 JKL5 MNO6 PQRS7 TUV8 WXYZ9 * 0# "; // Characters for T9 text entry
String ident = "";

char hexaKeys[ROWS][COLS] = { // Character matrix for the keypad
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {7, 6, 5, 4};        // pin assignments for keypad rows
byte colPins[COLS] = {3, 2, 1, 0};        // pin assignments for keypad columns

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);  // pin assignments for LCD

byte charUp[8] = {  // arrow up character for LCD
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte charDown[8] = {  // arrow down character for LCD
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B01110,
  B00100
};
byte charUpDown[8] = {  // arrow up and down character for LCD
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B11111,
  B01110,
  B00100
};

byte menuLevel = 0;     // Level 0: no menu display, display anything you like
                        // Level 1: display main menu
                        // Level 2: display sub menu
                        // Level 3: display sub menu of sub menu
byte menu = 1;          // holds the menu level
byte sub = 1;           // holds the sub menu level


void setup()
{
  lcd.begin(16, 2);               // initialized the LCD as 16 characters with 2 lines
  lcd.createChar(0, charUp);      // arrow up character
  lcd.createChar(1, charDown);    // arrow down character
  lcd.createChar(2, charUpDown);  // arrow up and down character
  updateLevel_0();                // display the HOME screen
}

void loop() {
  // Process the keys
  processkey();

  // Do other stuffs here
  
}

void processkey() {
  char key = customKeypad.getKey();
  if (isAlpha(key)) {   // check if key press is a letter
    processKeys(key);   // process it according to keys
  } 
}

void parseKey(int minValue, int maxValue, char keyPress) {
  int ch = minValue;
  char key = keyPress;
  if (keyPress == '*') {              // if *, means backspace
    if ( (x > 0) || (y > 0) ) {       // prevent backspace when no character yet
      x = x - 1;                      // go back to previous character position
      lcd.setCursor(x,y);             // set the new lcd position
      lcd.print("*");                 // write *, which means for editing
      msg.remove(msg.length() - 1);   // remove the last character from the string
    }
  } else {
    for (int i = 0; i < keyPressTime; i++) {
      if (key == keyPress) {          // make sure that same key is press
        lcd.setCursor(x, y);          // set the lcd position
        lcd.print(alpha[ch]);         // print the character according to the character position
        ch++;                         // increment character position
        if (ch > maxValue) {          // if the character counter reached the max value
          ch = minValue;              // reset to min value
          i = 0;                      // reset the loop counter
        }
      }
      key = customKeypad.getKey();  // get the keypress
      delay(10);                    // delay for some time
    }
    x++;                    // increment the x position
    msg += alpha[ch - 1];   // add the character to the variable msg
    if (x > 15) {           // if the lcd reached the rightmost position
      y = 1;                // then wrap to the next line
      x = 0;                // in first character in the left              
    }
  }
}

void enterNUM() {
  char key;
  lcd.clear();
  x = 0;
  y = 0;
  num = "";
  do {
    key = customKeypad.getKey();
    if (isDigit(key)) {                 // verify if the key press is a number
      num = num + key;
      lcd.setCursor(x,y);
      lcd.print(key);                   // print it to the lcd
      x++;                              // increment the x position
      if (x > 15)                       // if the lcd reached the rightmost position
      {                                 // then wrap to the next line
        x = 0;
        y = 1;
      }
    } else {                              // key press is not a number
      if (key == '*') {                   // if *, means backspace
        if ( (x > 0) || (y > 0) ) {       // prevent backspace when no character yet
          x = x - 1;                      // go back to previous character position
          lcd.setCursor(x,y);             // set the new lcd position
          lcd.print("*");                 // write *, which means for editing
          num.remove(num.length() - 1);   // remove the last character from the string
        }
      }
    }
  } while (key != '#');                   // exit the loop when # is pressed
                                          // means entering number is complete
  lcd.setCursor(0, 0);
  lcd.print("created num");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print(ident+num);
  delay(1000);
  lcd.clear();
  lcd.print("Sending SMS");
  lcd.clear();
  lcd.print("Done Sending");
  delay(2000);
}

void processKeys(char keyPressed) {
  switch (menuLevel) {
    case 0:                     // Level 0, home screen
      switch ( keyPressed ) {
        case 'D':               // Enter
          menu = 1;
          menuLevel = 1;        // go to main menu
          updateLevel_1();      // show main menu
          delay(DEFAULT_DELAY);
          break;
        case 'A': // Up
          break;
        case 'B': // Down
          break;
        case 'C': // Back
          menuLevel = 0;        // go to home screen
          updateLevel_0();      // show home screen
          delay(DEFAULT_DELAY);
          break;
        default:
          break;
      }
      break;
    case 1:                     // Level 1, main menu
      switch ( keyPressed ) {
        case 'D':               // Enter
          sub = 1;
          menuLevel = 2;        // go to sub menu
          updateLevel_2();      // show sub menu
          delay(DEFAULT_DELAY);
          break;
        case 'A':               // Up
          menu--;
          updateLevel_1();      // show main menu
          delay(DEFAULT_DELAY);
          break;
        case 'B':               // Down
          menu++;
          updateLevel_1();      // show main menu
          delay(DEFAULT_DELAY);
          break;
        case 'C': // Back
          menuLevel = 0;        // hide menu, go back to level 0
          updateLevel_0();      // show home screen
          delay(DEFAULT_DELAY);
          break;
        default:
          break;
      }
      break;
    case 2:                     // Level 2, sub menu
      switch ( keyPressed ) {
        case 'D':               // Enter
          if        (sub == 1) {  // Create SMS
            lcd.clear();
            lcd.print("Enter number");
            delay(500);
            enterNUM();
            menuLevel = 2;        // go to sub menu
            updateLevel_2();      // show sub menu
          }
          delay(DEFAULT_DELAY);
          break;
        case 'C': // Back
          menuLevel = 1;        // hide menu, go back to level 0
          updateLevel_1();      // show home screen
          delay(DEFAULT_DELAY);
          break;
        default:
          break;
      }
      
      break;
    default:
      break;
  }
}

void updateLevel_0() {
  lcd.clear();
  lcd.println("Keypad TextEntry");
  lcd.setCursor(0, 1);
  lcd.println("  - D for menu  ");
}

void updateLevel_1 () {
  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">State       ");
      lcd.setCursor(0, 1);
      lcd.print(" ON       ");
      lcd.setCursor(15, 1);
      lcd.write((byte)1);     // down arrow
      break;
    case 2:
      lcd.clear();
      lcd.print(" State       ");
      lcd.setCursor(0, 1);
      lcd.print(">ON       ");
      lcd.setCursor(15, 1);
      lcd.write((byte)2);     // up and down arrow
      break;
    case 3:
      lcd.clear();
      lcd.print(" ON       ");
      lcd.setCursor(0, 1);
      lcd.print(">OFF        ");
      lcd.setCursor(15, 1);
      lcd.write((byte)0);     // up arrow
      break;
    case 4:
      menu = 3;
      break;
  }
}

void updateLevel_2 () {
  switch (menu) {
    case 0:
      break;
    case 1:                                 // Messages
      switch (sub) {
        case 0:
          break;
        case 1:
          ident = "S";
          lcd.clear();
          lcd.print(" State:      ");
          lcd.setCursor(0, 1);
          lcd.print("  Socket number    ");
          lcd.setCursor(15, 1);
          break;

        default:
          break;
      }
      break;
    case 2:                                 // Relay 2
      switch (sub) {
        case 0:
          break;
        case 1:
          ident = "ON";
          lcd.clear();
          lcd.print(" ON      ");
          lcd.setCursor(0, 1);
          lcd.print("  Socket number ");
          lcd.setCursor(15, 1);
          
          break;
        default:
          break;
      }
      break;
    case 3:                               // Relay 3
      switch (sub) {
        case 0:
          break;
        case 1:
          ident = "OFF";
          lcd.clear();
          lcd.print(" OFF:        ");
          lcd.setCursor(0, 1);
          lcd.print("  Socket number       ");
          lcd.setCursor(15, 1);
     
          break;
        default:
          break;
      }
      break;
    case 4:
      sub = 3;
      break;
  }
}