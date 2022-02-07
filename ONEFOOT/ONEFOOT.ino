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

#ifdef __ARDUINO_CI
// For continuous integration testing using arduino-ci:
//  https://github.com/pololu/arduino-ci
#include <AStar32U4.h>
AStar32U4LCD display;
AStar32U4ButtonB buttonB;
AStar32U4ButtonC buttonC;
#endif

PololuMenu<typeof(display)> menu;


/*
 * Variables
 */

unsigned long currentMillis;
unsigned long previousMillis;
const unsigned long PERIOD = 5;

long countsLeft = 0;
long countsRight = 0;
long prevLeft = 0;
long prevRight = 0;
int Speed = 100;
const int CPR = 12;
const float RATIO = 75.81F;
const float DIAMETER = 3.2;
const int CIRCUMFERENCE = 10.0531;

float Sl = 0.0F;
float Sr = 0.0F;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
}

void loop() {
  // put your main code here, to run repeatedly:
  goRun();

}


void goRun(){
  goDemoInternal();
}
void goDisplay(){
    display.clear();
    display.setLayout21x8();
    display.print(F("Left: "));
    display.print(Sl);
    display.gotoXY(0,1);
    display.print(F("Right: "));
    display.print(Sr);
    display.gotoXY(0,2);
    display.print(F("Speed: "));
    display.print(Speed);
}

void goReverse(){
  delay(200);
  currentMillis = millis();
  if(currentMillis > previousMillis + PERIOD){
    countsLeft += encoders.getCountsAndResetLeft();
    countsRight += encoders.getCountsAndResetRight();
    Sl = ((countsLeft - prevLeft) / (CPR * RATIO) * CIRCUMFERENCE);
    Sr = ((countsRight - prevRight) / (CPR * RATIO) * CIRCUMFERENCE);


    motors.setSpeeds(-100,-100);
    } else {
      motors.setSpeeds(0,0);
    }
    goDisplay();
    previousMillis = currentMillis;
  }
void goDemoInternal(){
  currentMillis = millis();
  if(currentMillis > previousMillis + PERIOD){
    countsLeft += encoders.getCountsAndResetLeft();
    countsRight += encoders.getCountsAndResetRight();
    Sl = ((countsLeft - prevLeft) / (CPR * RATIO) * CIRCUMFERENCE);
    Sr = ((countsRight - prevRight) / (CPR * RATIO) * CIRCUMFERENCE);


    if(Sl < 30.48 && Sr < 30.48) {
      if(Sl > 20 && Sr > 20){
        Speed = 100 * ((30-Sl) / 10);
        if(Speed < 20) Speed = 20;
      }
      if(Sl < 10 && Sr < 10){
        Speed = 10 * ((60+Sl) / 10);
        if(Speed < 20) Speed = 20;
      }
      motors.setSpeeds(Speed, Speed);
    } else {
      motors.setSpeeds(-10,-10);
    }
    goDisplay();
    previousMillis = currentMillis;

  }
  else if(currentMillis < previousMillis + PERIOD){
    motors.setSpeeds(-100,-100);
  }
}
