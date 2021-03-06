#include <Stepper.h>

#define MAX_SPEED 300

#define X_STEPS_PER_REVOLUTION 4000
#define Y_STEPS_PER_REVOLUTION 3125
#define Z_STEPS_PER_REVOLUTION 3125

#define X_PULSE_PIN 2
#define X_DIRECTION_PIN 5
#define X_MIN 0
#define X_MAX 31250

#define Y_PULSE_PIN 3
#define Y_DIRECTION_PIN 6
#define Y_MIN 0
#define Y_MAX 6250

#define Z_PULSE_PIN 4
#define Z_DIRECTION_PIN 7
#define Z_MIN 0
#define Z_MAX 6250

#define PWM_PIN 12

#define POWER_LOSS_PIN 9


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
  pinMode(POWER_LOSS_PIN, INPUT);

  XStepper.setSpeed(50);
}



void loop() {
  //check if power is going to be lost
  if (digitalRead(POWER_LOSS_PIN) == HIGH) {

    //Serial debugging
    switch (Serial.peek()) {
      case -1:
        break;
      case 'm':
        target = Serial.parseInt();
        Serial.println("moving to " + String(target));
        //        setXPosition(target);
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
    int pulseLength = pulseIn(PWM_PIN, HIGH, 50000);
    if (pulseLength >= 1000 && pulseLength <= 2000) {
      moveTowardXPosition(map(pulseLength, 1000, 2000, X_MIN, X_MAX));
    }
    else if (pulseLength < 1000) {
      moveTowardXPosition(X_MIN);
    }
    else {
      moveTowardXPosition(X_MAX);
    }

  } else {
    Serial.println("Power Lost");
    setXPosition(X_MIN);
  }


  delayMicroseconds(10);
}


void setXPosition(long targetPosition) {
  XStepper.step(targetPosition - xPosition);
  xPosition = targetPosition;
}

void moveTowardXPosition(long targetPosition) {
  long difference = targetPosition - xPosition;
  long stepAmount = 0;
  if (abs(difference) > ((X_MAX - X_MIN)/100)) {
    if (difference > 0) {
      stepAmount = abs(difference) > 200 ? 200 : difference;
    } else {
      stepAmount = abs(difference) > 200 ? -200 : difference;
    }
    XStepper.step(stepAmount);
    xPosition += stepAmount;
  }

}

//void setYPosition(long targetPosition) {
//
//  YStepper.step(yPosition - targetPosition);
//  yPosition = targetPosition;
//
//}
//
//void setZPosition(long targetPosition) {
//
//  ZStepper.step(zPosition - targetPosition);
//  zPosition = targetPosition;
//
//}

int readPWM(int inputPin) {
  unsigned long highTime = pulseIn(inputPin, HIGH);
  unsigned long lowTime = pulseIn(inputPin, LOW);
  unsigned long cycleTime = highTime + lowTime;
  float dutyCycle = (float)highTime / float(cycleTime);
  return (int)(dutyCycle * (255 / 100));
}



