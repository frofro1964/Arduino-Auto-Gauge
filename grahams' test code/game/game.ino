/*  Use an LCD display and a pushbutton to play a simple game.
 *  Author: Graham Home
 */

// Math library
#include <Math.h>

// I2C libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Standard library
#include <stdlib.h>

// LCD display device
LiquidCrystal_I2C lcd(0x3F,16,2); //Addr: 0x3F, 20 chars & 4 lines

// I/O pin numbers
const int btnPin = 3; // Connected to pushbutton

// Gameplay toggle
bool playing = false;

void setup() {
  // Set up I/O pins
  pinMode( btnPin, INPUT );

  // Set up interrupt for pushbutton
  attachInterrupt( digitalPinToInterrupt(btnPin), toggle, FALLING ); // Debounced

  // Start I2C
  Wire.begin();

  // Start LCD
  lcd.begin();
  lcd.backlight();
}

/** 
 *  Cyclic executive.
 */
void loop() {
  play();
}

/**
 * Gameplay function.
 */
void play() {
  lcd.clear();
  lcd.setCursor( 0, 0 );
  lcd.print( "Moving Bar Game" );
  lcd.setCursor( 0, 1 );
  lcd.print( "Hold Btn 2 Start" );
  while( !digitalRead(btnPin) ) {delay(1000);};
  int number = rand() % 17; //0-16
  int position = 0;
  while( digitalRead(btnPin) ) {
    lcd.clear();
    lcd.setCursor( 0, 0 );
    lcd.print( "Stop Bar At " );
    lcd.print( number );
    for ( position; position<16; position+=1 ) {
      if ( digitalRead(btnPin) ) {
        lcd.setCursor( position, 1 );
        lcd.print( (char) 0xff );
        delay(100);
      } else {
        break;
      }
    }
    for ( position; position>0; position-- ) {
      if ( digitalRead(btnPin) ) {
        lcd.setCursor(position, 1);
        lcd.print( (char) 0xfe );
        delay(100);
      } else {
        break;
      }
    }
  }
  lcd.clear();
  lcd.setCursor( 0, 0 );
  if (position == number) {
    lcd.print( "You Win!" );
  } else {
    lcd.print( "You Lose, Sucka!" );
    lcd.setCursor( 0, 1 );
    lcd.print( "Goal: " );
    lcd.print( number );
    lcd.print( " You: " );
    lcd.print( position );
  }
  delay(2000);
}

/**
 * Toggles the gameplay on/off.
 */
void toggle() {
  playing = !playing;
}
