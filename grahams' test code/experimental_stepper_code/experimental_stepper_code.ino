/*  Use a stepper motor as a dial gauge to display measurements taken with a temperature sensor.
 *  Author: Graham Home
 *  Hardware:
 *  Arduino Nano
 *  DS18B20 Temperature Sensor
 *  Switech X27 168 Stepper Motor
 */

// Libraries needed for temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>

// Math library
#include <Math.h>

// I/O pin numbers
const int stepPin = 9; // Set high to start steppin'
const int dirPin = 8; // Sets direction
const int btnPin = 3; // Connected to pushbutton
const int tempPin = 6; // Connected to temperature sensor

// OneWire instance to communicate with the temperature sensor
OneWire oneWire(tempPin);

// Dallas Temperature instance created from the oneWire reference
DallasTemperature sensors(&oneWire);

// Max/min values for temperature readings, in degrees Fahrenheit
const int minTemp = 30;
const int maxTemp = 263;

// Max/min values for needle distance
const int minPos = 0;
const int maxPos = 600;

// Current needle position
int position = 0;

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

  // Start up the Dallas Temperatures library
  sensors.begin();

  // start serial port
  // Serial.begin(9600);
}

/** 
 *  Cyclic executive.
 */
void loop() {
  /*if ( on ) {
    updateTemp();
  }*/
  stepFwdAndBack();
}

/**
 * Updates the position of the needle gauge to indicate the measured temperature.
 */
void updateTemp() {
  sensors.requestTemperatures(); // Send the command to get temperature reading
  float temp = ( ( sensors.getTempCByIndex( 0 ) * 1.8 ) + 32 ); // Get temperature in degrees Fahrenheit
  int newPosition = ( temp / ( maxTemp-minTemp ) ) * ( maxPos-minPos );
  move( abs( position - newPosition ), newPosition > position );
}

/**
 * Resets the needle to the starting position.
 */
void resetNeedle() {
  digitalWrite( dirPin, LOW );
  for( int x = maxPos; x >= minPos; x-- ) {
    digitalWrite( stepPin, HIGH ); 
    delayMicroseconds( 500 ); 
    digitalWrite( stepPin, LOW ); 
    delayMicroseconds( 500 ); 
  }
  position = minPos;
  move( maxPos, true );
  move( minPos, false );
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
      digitalWrite( dirPin, HIGH );
    } else {
      digitalWrite( dirPin, LOW );
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
 * Steps the needle all the way forward, then back.
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
      delay(100);
    }
  }
}

/**
 * Toggles the program's on/off switch.
 */
void toggle() {
  on = !on;
}
