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
float angle;
float currentDistance;


void setup() {
  // put your setup code here, to run once:
  servo.attach(21);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
  angle = 90;
}

void loop() {
  while(angle > 45){
    angle--;
    delay(10);
    servo.write(angle);
    
  }
  digitalWrite(TRIG, HIGH);
  digitalWrite(TRIG, LOW);
  currentDistance = (pulseIn(ECHO, HIGH) / 58);
  Serial.print("Distance at 45: ");
  Serial.println(currentDistance);
  while(angle < 135){
    angle++;
    delay(10);
    servo.write(angle);
  }
  currentDistance = (pulseIn(ECHO, HIGH) / 58);
  Serial.print("Distance at 135: ");
  Serial.println(currentDistance);
}
