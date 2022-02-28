#define TRIG 4
#define ECHO 22


#include <Wire.h>
#include <Pololu3piPlus32U4.h>
#include <Servo.h>


#include <Pololu3piPlus32U4IMU.h>

#include <PololuMenu.h>

using namespace Pololu3piPlus32U4;


Buzzer buzzer;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
LineSensors lineSensors;
BumpSensors bumpSensors;
IMU imu;
Motors motors;
Encoders encoders;
Servo servo;

void setup() {
  // put your setup code here, to run once:
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(TRIG, HIGH);
  digitalWrite(TRIG, LOW);
  float currentDistance = (pulseIn(ECHO, HIGH) / 58);
  if(currentDistance > 30){
    double fowardspeed = currentDistance * 2;
    motors.setSpeeds(-fowardspeed, -fowardspeed);
  } else if(currentDistance < 30){
    double backspeed = currentDistance * 2;
    if(currentDistance > 10){
       motors.setSpeeds(backspeed, backspeed);
    } else {
      motors.setSpeeds(100, 100);
    }
  } else {
    motors.setSpeeds(0,0);
    delay(500);
  }
}
