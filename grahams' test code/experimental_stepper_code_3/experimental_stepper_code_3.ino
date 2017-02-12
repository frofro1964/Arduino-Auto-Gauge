/*  Use a stepper motor as a dial gauge to display measurements taken with a temperature sensor.
 *  Also display temperature on an LCD display.
 *  Author: Graham Home
 *  Hardware:
 *  Arduino Nano
 *  Honeywell HIH9121 Temperature Sensor
 *  Switech X27 168 Stepper Motor
 */

// Math library
#include <Math.h>

// I2C libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <stdlib.h>

// LCD display device
LiquidCrystal_I2C lcd(0x3F,16,2); //Addr: 0x3F, 20 chars & 4 lines

// I/O pin numbers
const int stepPin = 9; // Set high to start steppin'
const int dirPin = 8; // Sets direction
const int btnPin = 3; // Connected to pushbutton

// Max/min values for temperature readings, in degrees Fahrenheit
const int minTemp = 30;
const int maxTemp = 263;

// Max/min values for needle distance
const int minPos = 0;
const int maxPos = 600;

// Current needle position (default to max position value)
int position = maxPos;

// On/off toggle
bool on = true;

void setup() {
  // Set up I/O pins
  pinMode( stepPin, OUTPUT ); 
  pinMode( dirPin, OUTPUT );
  pinMode( btnPin, INPUT );

  // Set up interrupt for pushbutton
  attachInterrupt( digitalPinToInterrupt(btnPin), toggle, FALLING ); // Debounced

  // Set dial gauge to starting position
  resetNeedle();

  // start serial port
  //Serial.begin(9600);

  // Start I2C
  Wire.begin();

  // Start LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.clear();
}

/** 
 *  Cyclic executive.
 */
void loop() {
  if ( on ) {
    updateTemp();
  }
}

/**
 * Prints the specified float value on the LCD display.
 */
void print_float(float f, int num_digits){
  lcd.clear();
  int f_int;
  int pows_of_ten[4] = {1, 10, 100, 1000};
  int multiplier, whole, fract, d, n;

  multiplier = pows_of_ten[num_digits];
  if (f < 0.0)
  {
      f = -f;
      lcd.print("-");
  }
  whole = (int) f;
  fract = (int) (multiplier * (f - (float)whole));

  lcd.print(whole);
  if ( num_digits > 0 ) {
    lcd.print(".");
  }

  for (n=num_digits-1; n>=0; n--) // print each digit with no leading zero suppression
  {
       d = fract / pows_of_ten[n];
       lcd.print(d);
       fract = fract % pows_of_ten[n];
  }
}

/**
 * Updates the position of the needle gauge to indicate the measured temperature.
 */
void updateTemp() {
  float temp = checkTemp();
  int newPosition = ( temp / ( maxTemp-minTemp ) ) * ( maxPos-minPos );
  int numSquares = ( temp / ( maxTemp-minTemp ) ) * 16;
  if ( newPosition != position ) {
    print_float( temp, 0 );
    for ( int i = 0; i < 16; i++ ) {
      lcd.setCursor(i, 1);
      if (i <= numSquares) {
        lcd.print((char) 0xff);
      } else {
        lcd.print(" ");
      }
    }
    move( abs( position - newPosition ), newPosition > position );
  }
}

/**
 * Gets a temperature measurement from the sensor in degrees Fahrenheit
 */
float checkTemp() {
  byte address, Temp_H, Temp_L;
  unsigned int H_dat, T_dat;
  address = 0x27;
  Wire.beginTransmission(address); 
  Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom((int)address, (int) 4);
  Wire.read();
  Wire.read();
  Temp_H = Wire.read();
  Temp_L = Wire.read();
  Wire.endTransmission();
  
  T_dat = (((unsigned int)Temp_H) << 8) | Temp_L;
  T_dat = T_dat / 4;
  float temp = (float) T_dat * 1.007e-2 - 40.0;
  temp = ( ( temp * 1.8 ) + 32 ); // Get temperature in degrees Fahrenheit
  return temp;
}

/**
 * Resets the needle to the starting position.
 */
void resetNeedle() {
  move( maxPos - minPos, false );
}

/**
 * Moves the needle the specified distance in the specified direction.
 * @param distance : The distance to move.
 * @param clockwise : True to move clockwise, false for counter-clockwise.
 */
void move(int distance, bool clockwise) {
  bool validMove = ( ( clockwise ) ? ( ( position + distance ) <= maxPos ) : ( ( position - distance ) >= minPos ) );
  if ( validMove ) {
    if ( clockwise ) {
      digitalWrite( dirPin, LOW );
    } else {
      digitalWrite( dirPin, HIGH );
    }
    for( int x = 0; x < distance; x++ ) {
      digitalWrite( stepPin, HIGH ); 
      delayMicroseconds( 5000 ); 
      digitalWrite( stepPin, LOW ); 
      delayMicroseconds( 5000 ); 
    }
    position = ( ( clockwise ) ? position + distance : position - distance );
  }
}

/**
 * Incrementally steps the needle all the way forward, then back.
 * Test function.
 */
void stepFwdAndBack() {
  int increment = 10;
  bool clockwise = true;
  while ( position < maxPos ) {
    if ( on ) {
      move( increment, clockwise );
    }
    delay(100);
  }
  clockwise = false;
  while ( position > minPos ) {
    if ( on ) {
      move( increment, clockwise );
    }
    delay(100);
  }
}

/**
 * Toggles the program's on/off switch.
 */
void toggle() {
  on = !on;
}
