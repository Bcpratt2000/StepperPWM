
/*
 Stepper Motor Control - speed control

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.
 A potentiometer is connected to analog input 0.

 The motor will rotate in a clockwise direction. The higher the potentiometer value,
 the faster the motor speed. Because setSpeed() sets the delay between steps,
 you may notice the motor is less responsive to changes in the sensor value at
 low speeds.

 Created 30 Nov. 2009
 Modified 28 Oct 2010
 by Tom Igoe

 */

#include <Stepper.h>

#define MAX_SPEED 300

#define X_STEPS_PER_REVOLUTION 3125
#define Y_STEPS_PER_REVOLUTION 3125
#define Z_STEPS_PER_REVOLUTION 3125

#define X_PULSE_PIN 2
#define X_DIRECTION_PIN 5
#define X_MIN 0
#define X_MAX 6250

#define Y_PULSE_PIN 3
#define Y_DIRECTION_PIN 6
#define Y_MIN 0
#define Y_MAX 6250

#define Z_PULSE_PIN 4
#define Z_DIRECTION_PIN 7
#define Z_MIN 0
#define Z_MAX 6250

#define PWM_PIN 12


// initialize the steppers
Stepper XStepper(X_STEPS_PER_REVOLUTION, X_PULSE_PIN, X_DIRECTION_PIN);
Stepper YStepper(Y_STEPS_PER_REVOLUTION, Y_PULSE_PIN, Y_DIRECTION_PIN);
Stepper ZStepper(Z_STEPS_PER_REVOLUTION, Z_PULSE_PIN, Z_DIRECTION_PIN);

long xPosition, yPosition, zPosition;

long target;

void setup() {
  xPosition = 0;
  yPosition = 0;
  zPosition = 0;
  Serial.begin(9600);

  pinMode(PWM_PIN, INPUT);
  
  XStepper.setSpeed(250);
  YStepper.setSpeed(250);
  ZStepper.setSpeed(250);
}



void loop() {
  //Serial debugging
  switch(Serial.peek()){
  case 'm':
    target = Serial.parseInt();
    Serial.println("moving to " + String(target));
    setXPosition(target);
    break;
  case 'r':
    Serial.println(String(xPosition));
    break;
  case 's':
    target = Serial.parseInt();
    XStepper.setSpeed(target);
    Serial.println("set speed to " + String(target));
  default:
    break;
  }
  Serial.read();

  //respond to pwm
  int pulseLength = pulseIn(PWM_PIN, HIGH, 50000)/10;
  pulseLength*=10;
  if(pulseLength>=1000 && pulseLength<=2000){
    setXPosition(map(pulseLength, 1000, 2000, X_MIN, X_MAX));
  }
  else if(pulseLength<1000){
    setXPosition(X_MIN);
  }
  else{
    setXPosition(X_MAX);
  }
  
  
  delay(10);
}


void setXPosition(long targetPosition){
  if(abs(targetPosition-xPosition) > 100){
    XStepper.step(xPosition-targetPosition);
    xPosition = targetPosition;
  }
  
}

void setYPosition(long targetPosition){

  YStepper.step(yPosition-targetPosition);
  yPosition = targetPosition;
  
}

void setZPosition(long targetPosition){

  ZStepper.step(zPosition-targetPosition);
  zPosition = targetPosition;
  
}

int readPWM(int inputPin){
  unsigned long highTime = pulseIn(inputPin, HIGH);
  unsigned long lowTime = pulseIn(inputPin, LOW);
  unsigned long cycleTime = highTime + lowTime;
  float dutyCycle = (float)highTime / float(cycleTime);
  return (int)(dutyCycle*(255/100));
}



