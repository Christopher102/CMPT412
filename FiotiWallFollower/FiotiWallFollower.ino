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
double kp = .001;
double ki = .01;
double kd = 0;
const double reqdistance = 10;

double kiTotal = 0.0;
double priorError = 0.0;
long prevTime = millis();

unsigned long currentTime, previousTime;
double elapsedTime;
double error;
double lastError;
double currentDistance, pidResult;
double totalError, rateError;
double totalDistance;
double avgDistance;
int cycle;
double leftspeed;
double rightspeed;


void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
  Serial.println("BEGIN!");
  servo.attach(21);
  servo.write(90);
  cycle = 1;
  leftspeed = -100;
  rightspeed = -100;
  motors.setSpeeds(leftspeed, rightspeed);
}

void loop() {
  digitalWrite(TRIG, HIGH);
  digitalWrite(TRIG, LOW);
  cycle1();
  digitalWrite(TRIG, HIGH);
  digitalWrite(TRIG, LOW);
  cycle2();
}



void cycle1(){
  cycle = 1;
  totalDistance = 0;
  int i = 0;
  servo.write(180);
  while(i < 1){
    float currentDistance = (pulseIn(ECHO, HIGH) / 58);
    totalDistance = totalDistance + currentDistance;
    i++;
  }
  avgDistance = totalDistance;
  changeSpeed(computePID(avgDistance));
  
}

void cycle2(){
  cycle = 2;
  totalDistance = 0;
  int i = 0;
  servo.write(180);
  while(i < 1){
    float currentDistance = (pulseIn(ECHO, HIGH) / 58);
    totalDistance = totalDistance + currentDistance;
    i++;
  }
  avgDistance = totalDistance;
  changeSpeed(computePID(avgDistance));
  
}




double computePID(double inp){     
        currentTime = millis();                //get current time
        elapsedTime = (double)(currentTime - previousTime);        //compute time elapsed from previous computation
        
        error = reqdistance - inp; // determine error
        double proportional = kp * error * elapsedTime;
        kiTotal += error;
        
        double integral = ki * kiTotal;                // compute integral
        if(integral > 200){
          integral = 200;
        } else if(integral < -200){
          integral = -200;
        }

        
        float derivative = kd * (error - priorError);  // compute derivative

        priorError = error;

        double out = proportional + integral + derivative;               //PID output               

        Serial.println(out);
        
        lastError = error;                                //remember current error
        previousTime = currentTime;                        //remember current time

        return out;                                        //have function return the PID output
}

void changeSpeed(double inp){
  inp = -inp;
  Serial.println(inp);
  float currentDistance = (pulseIn(ECHO, HIGH) / 58);
  if(currentDistance > reqdistance){

    motors.setSpeeds(leftspeed + inp, rightspeed - inp);
  } else if(currentDistance < reqdistance){
    motors.setSpeeds(leftspeed - inp, rightspeed + inp);
  }
}
