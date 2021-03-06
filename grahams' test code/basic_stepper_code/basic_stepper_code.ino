/*     Simple Stepper Motor Control Exaple Code
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */
// defines pins numbers
const int stepPin = 3; 
const int dirPin = 4; 
 
void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
}
void loop() {
  int stepTime = 1000;
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 400; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(stepTime); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(stepTime); 
  }
  delay(2000); // One second delay
  
  digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 800; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(stepTime);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(stepTime);
  }
  delay(2000);
}
