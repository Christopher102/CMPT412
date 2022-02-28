#define TRIG 3
#define ECHO 2

#include <Wire.h>
#include <Pololu3piPlus32U4.h>


#include <Pololu3piPlus32U4IMU.h>

#include <PololuMenu.h>

using namespace Pololu3piPlus32U4;

OLED display;
Buzzer buzzer;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
LineSensors lineSensors;
BumpSensors bumpSensors;
IMU imu;
Motors motors;
Encoders encoders;

void setup() {
  // put your setup code here, to run once:
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10); 
  digitalWrite(TRIG, LOW); 
  float distance = pulseIn(ECHO, HIGH, 38000); //ECHO Pin, Looking for a HIGH signal
  distance = distance / 58; 
  Serial.print(distance); 
  Serial.println(" cm"); 
  goDisplay(distance);
  delay(500);
}

void goDisplay(float distance){
    display.clear();
    display.setLayout21x8();
    display.print(F("Distance: "));
    display.print(distance);
}
