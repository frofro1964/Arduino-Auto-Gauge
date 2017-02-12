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

// Pass the oneWire reference to a new Dallas Temperature instance
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
  Serial.begin(9600);
}

/** 
 *  Cyclic executive.
 */
void loop() {
   sensors.requestTemperatures(); // Send the command to get temperature reading
   float temp = ( ( sensors.getTempCByIndex( 0 ) * 1.8 ) + 32 ); // Get temperature in degrees Fahrenheit
   Serial.print( "Current temp: (F): " );
   Serial.println( temp );
   int newPosition = ( temp / ( maxTemp-minTemp ) ) * ( maxPos-minPos );
   Serial.print( "Old position: " );
   Serial.println( position );
   Serial.print( "New position: " );
   Serial.println( newPosition );
   move( abs( position - newPosition ), newPosition > position );
}

/**
 * Resets the needle to the starting position.
 */
void resetNeedle() {
  digitalWrite( dirPin, LOW );
  for( int x = maxPos; x > minPos; x-- ) {
    digitalWrite( stepPin, HIGH ); 
    delayMicroseconds( 500 ); 
    digitalWrite( stepPin, LOW ); 
    delayMicroseconds( 500 ); 
  }
  position = minPos;
}

/**
 * Moves the needle the specified distance in the specified direction.
 * @param dist : The distance to move.
 * @param clockwise : True to move clockwise, false for counter-clockwise.
 */
void move(int dist, bool clockwise) {
   if ( clockwise ) {
    digitalWrite( dirPin, HIGH );
  } else {
    digitalWrite( dirPin, LOW );
  }
  for( int x = 0; x < dist; x++ ) {
    digitalWrite( stepPin, HIGH ); 
    delayMicroseconds( 500 ); 
    digitalWrite( stepPin, LOW ); 
    delayMicroseconds( 500 ); 
  }
  position = ( ( clockwise ) ? position + dist : position - dist );
}

/**
 * Steps the needle all the way forward, then back.
 * Test function.
 */
void stepFwdAndBack() {
  int increment = 10;
  bool clockwise = true;
  int pos = minPos;
  while ( pos < maxPos ) {
    if ( on ) {
      move(increment, clockwise);
      pos += increment;
    }
    delay(100);
  }
  clockwise = !clockwise;
}

/**
 * Checks the pushbutton switch.
 */
void checkButton() {
  if ( digitalRead( btnPin ) ) {
    delay ( 500 );
    if ( digitalRead ( btnPin ) ) {
      on = !on;
      delay( 1000 );
    }
  }
}

/**
 * Toggles the program's on/off switch.
 */
void toggle() {
  on = !on;
}
