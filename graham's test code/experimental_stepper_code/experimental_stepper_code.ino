/*  Use a stepper motor as a dial gauge.
 *  Author: Graham Home
 */

// Define pin numbers
const int stepPin = 9; // Set high to start steppin'
const int dirPin = 8; // Sets direction
const int btnPin = 3; // Input pin for pushbutton

// Max/min values for needle distance
const int minPos = 0;
const int maxPos = 600;

// Variable to determine how far the needle will move
int increment = 20;

// Variable to determine the direction in which the needle will move
bool clockwise = true;

// On/off toggle
bool on = true;

/**
 * Resets the needle to the starting position.
 */
void resetNeedle() {
  digitalWrite( dirPin, LOW );
  for( int x = 0; x < maxPos; x++ ) {
    digitalWrite( stepPin, HIGH ); 
    delayMicroseconds( 500 ); 
    digitalWrite( stepPin, LOW ); 
    delayMicroseconds( 500 ); 
  }
}

/**
 * Moves the needle the specified distance.
 * @param dist : The distance to move.
 */
void move(int dist) {
  for( int x = 0; x < dist; x++ ) {
    digitalWrite( stepPin, HIGH ); 
    delayMicroseconds( 500 ); 
    digitalWrite( stepPin, LOW ); 
    delayMicroseconds( 500 ); 
  }
}

/**
 * Steps the needle all the way forward, then back.
 */
void stepFwdAndBack() {
  if ( clockwise ) {
    digitalWrite( dirPin, HIGH );
  } else {
    digitalWrite( dirPin, LOW );
  }
  int pos = minPos;
  while ( pos < maxPos ) {
    if ( on ) {
      move(increment);
      pos += increment;
    }
    delay(250);
  }
  clockwise = !clockwise;
  delay(1000);
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
 * Toggles the program's 'on' switch.
 */
void toggle() {
  on = !on;
}
 
void setup() {
  // Set up I/O pins
  pinMode( stepPin, OUTPUT ); 
  pinMode( dirPin, OUTPUT );
  pinMode( btnPin, INPUT );
  attachInterrupt( digitalPinToInterrupt(btnPin), toggle, FALLING );
  resetNeedle();
  
  
}
void loop() {
  //checkButton();
  stepFwdAndBack();
}
