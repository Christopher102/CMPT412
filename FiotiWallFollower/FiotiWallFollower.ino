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

int pos = servo.read();
int rightlimit = servo.read();
int leftlimit = rightlimit + 60;

//PID constants
double kp = .0001;
double ki = .001;
double kd = .01;
const double reqdistance = 10;

unsigned long currentTime, previousTime;
double elapsedTime;
double error;
double lastError;
double currentDistance, pidResult;
double totalError, rateError;


void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(TRIG, HIGH);
  digitalWrite(TRIG, LOW);
  float currentDistance = (pulseIn(ECHO, HIGH) / 58);
  double motorspeed = computePID(currentDistance);
  delay(100);
  motors.setSpeeds(-motorspeed, -motorspeed);
}


double computePID(double inp){     
        currentTime = millis();                //get current time
        elapsedTime = (double)(currentTime - previousTime);        //compute time elapsed from previous computation
        
        error = reqdistance - inp;                                // determine error
        totalError += error * elapsedTime;                // compute integral
        rateError = (error - lastError)/elapsedTime;   // compute derivative

        double out = kp*error + ki*totalError + kd*rateError;                //PID output               

        Serial.println(out);
        
        lastError = error;                                //remember current error
        previousTime = currentTime;                        //remember current time

        return out;                                        //have function return the PID output
}
