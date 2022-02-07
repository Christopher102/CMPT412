
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
const unsigned long PERIOD = 50;

long countsLeft = 0;
long countsRight = 0;
long prevLeft = 0;
long prevRight = 0;

const int CPR = 12;
const float RATIO = 75.81F;
const float DIAMETER = 3.2;
const int CIRCUMFERENCE = 10.0531;

float Sl = 0.0F;
float Sr = 0.0F;

void item1()
{
  display.setLayout21x8();
  display.clear();
  display.print(F("Press A to continue.")); 
  display.gotoXY(0,7);
  display.print(F("\7 Press B to cancel."));

  bool waiting = true;
  while(waiting)
  {
    char button = menu.buttonMonitor();
    switch(button)
    {
    case 'B':
      display.setLayout8x2();
      return;
    case 'A':
      waiting = false;
    }
  }

  display.setLayout8x2();
  display.print(F("Going"));
  display.gotoXY(5,1);
  display.print(F("..."));
  delay(200);
  while(buzzer.isPlaying())
  {
    if(menu.buttonMonitor() == 'B')
      return;
  }
  goDemoInternal();
}

void goDemoInternal(){
  bool testing = true;
  while(testing){
  char button = menu.buttonMonitor();
  if(button = 'B'){
    testing = false;
    return;
  }
  currentMillis = millis();
  if(currentMillis > previousMillis + PERIOD){
    countsLeft += encoders.getCountsAndResetLeft();
    countsRight += encoders.getCountsAndResetRight();
    Sl += ((countsLeft - prevLeft) / (CPR * RATIO) * CIRCUMFERENCE);
    previousMillis = currentMillis;

  }
  }
}

void item2()
{
  display.clear();
  display.print("Two!");
  delay(1000);
}

void setup()
{
  // The list of menu items.
  //
  // Strings are stored in flash using the F() macro.  This only works
  // within a function, so we define the menu items in setup().  The
  // static const specifiers ensure that they will remain available
  // after setup() exits.
  static const PololuMenuItem items[] = {
    { F("Drive!"), item1 },
    { F("Beep!"), item2 },
  };

  // Set up the menu.
  //
  // Note that you must pass in the correct number of items to
  // setItems().  Instead of typing a literal number, you might find
  // it more convenient to have the Arduino compiler compute it
  // automatically with the following formula:
  //
  // sizeof(items)/sizeof(items[0]))
  menu.setItems(items, 2);

  // Attach the LCD and buttons.  All are optional, but there's
  // probably not much reason to use this library without an LCD and
  // at least two buttons.
  menu.setLcd(display);
  menu.setSelectButton(buttonB, 'B');
  menu.setNextButton(buttonC, 'C');
}

void loop()
{
  menu.select();
}
