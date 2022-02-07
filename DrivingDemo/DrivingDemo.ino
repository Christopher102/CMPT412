/*
This is a demo program for the 3pi+ 32U4 OLED Robot.

It uses the buttons, display, and buzzer to provide a user
interface.  It presents a menu to the user that lets the user
select from several different demos.

To use this demo program, you will need to have the OLED
display connected to the 3pi+.

If you have an older 3pi+ with a black and green LCD display, use the
other demo, DemoForLCDVersion.
*/

#include <Wire.h>
#include <Pololu3piPlus32U4.h>

/* The IMU is not fully enabled by default since it depends on the
Wire library, which uses about 1400 bytes of additional code space
and defines an interrupt service routine (ISR) that might be
incompatible with some applications (such as our TWISlave example).

Include Pololu3piPlus32U4IMU.h in one of your cpp/ino files to
enable IMU functionality.
*/
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

PololuMenu<typeof(display)> mainMenu;

// declarations for splash screen
#include "splash.h"

bool launchSelfTest = false;

// A couple of simple tunes, stored in program space.
const char beepBrownout[] PROGMEM = "<c8";
const char beepWelcome[] PROGMEM = ">g32>>c32";
const char beepThankYou[] PROGMEM = ">>c32>g32";
const char beepFail[] PROGMEM = "<g-8r8<g-8r8<g-8";
const char beepPass[] PROGMEM = ">l32c>e>g>>c8";
const char beepReadySetGo[] PROGMEM = ">c16r2>c16r2>>c4";

// Custom characters for the LCD:

// This character is a back arrow.
const char backArrow[] PROGMEM = {
  0b00000,
  0b00010,
  0b00001,
  0b00101,
  0b01001,
  0b11110,
  0b01000,
  0b00100,
};

// This character is two chevrons pointing up.
const char forwardArrows[] PROGMEM = {
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
};

// This character is two chevrons pointing down.
const char reverseArrows[] PROGMEM = {
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b10001,
  0b01010,
  0b00100,
  0b00000,
};

// This character is two solid arrows pointing up.
const char forwardArrowsSolid[] PROGMEM = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b01110,
  0b11111,
  0b00000,
};

// This character is two solid arrows pointing down.
const char reverseArrowsSolid[] PROGMEM = {
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
};

void loadCustomCharacters()
{
  // The LCD supports up to 8 custom characters.  Each character
  // has a number between 0 and 7.  We assign #7 to be the back
  // arrow; other characters are loaded by individual demos as
  // needed.

  display.loadCustomCharacter(backArrow, 7);
}

// Assigns #0-6 to be bar graph characters.
void loadCustomCharactersBarGraph()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  display.loadCustomCharacter(levels + 0, 0);  // 1 bar
  display.loadCustomCharacter(levels + 1, 1);  // 2 bars
  display.loadCustomCharacter(levels + 2, 2);  // 3 bars
  display.loadCustomCharacter(levels + 3, 3);  // 4 bars
  display.loadCustomCharacter(levels + 4, 4);  // 5 bars
  display.loadCustomCharacter(levels + 5, 5);  // 6 bars
  display.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

// Assigns #0-4 to be arrow symbols.
void loadCustomCharactersMotorDirs()
{
  display.loadCustomCharacter(forwardArrows, 0);
  display.loadCustomCharacter(reverseArrows, 1);
  display.loadCustomCharacter(forwardArrowsSolid, 2);
  display.loadCustomCharacter(reverseArrowsSolid, 3);
}

// Clears the LCD and puts [back_arrow]B on the second line
// to indicate to the user that the B button goes back.
void displayBackArrow()
{
  display.clear();
  display.gotoXY(0,1);
  display.print(F("\7B"));
  display.gotoXY(0,0);
}

void displaySplash(uint8_t *graphics, uint8_t offset = 0)
{
  memset(graphics, 0, sizeof(pololu3PiPlusSplash));
  for(uint16_t i = 0; i < sizeof(pololu3PiPlusSplash) - offset*128; i++)
  {
    graphics[i] = pgm_read_byte(pololu3PiPlusSplash + (i%128)*8 + i/128 + offset);
  }
  display.display();
}

void showSplash()
{
  // We only need the graphics array within showSplash(); it's not
  // used elsewhere in the demo program, so we can make it a local
  // variable.
  uint8_t graphics[1024];

  display.setLayout21x8WithGraphics(graphics);
  displaySplash(graphics, 0);

  uint16_t blinkStart = millis();
  while((uint16_t)(millis() - blinkStart) < 900)
  {
    // keep setting the LEDs on for 1s
    // the Green/Red LEDs might turn off during USB communication
    ledYellow(1);
    ledGreen(1);
    ledRed(1);
  }

  // scroll quickly up
  for(uint8_t offset = 1; offset < 5; offset ++)
  {
    delay(100);
    displaySplash(graphics, offset);
  }

  display.clear();
  display.gotoXY(0, 5);
  display.print(F("Push B to start demo!"));
  display.gotoXY(0, 6);
  display.print(F("For more info, visit"));
  display.gotoXY(0, 7);
  display.print(F(" www.pololu.com/3pi+"));

  while((uint16_t)(millis() - blinkStart) < 2000)
  {
    // keep the LEDs off for 1s
    ledYellow(0);
    ledGreen(0);
    ledRed(0);
  }

  // Keep blinking the green LED while waiting for the
  // user to press button B.
  blinkStart = millis();
  while (mainMenu.buttonMonitor() != 'B')
  {
    uint16_t blinkPhase = millis() - blinkStart;
    ledGreen(blinkPhase < 1000);
    if (blinkPhase >= 2000) { blinkStart += 2000; }
  }
  ledGreen(0);

  display.setLayout11x4WithGraphics(graphics);
  display.clear();
  display.gotoXY(0,3);
  display.noAutoDisplay();
  display.print(F("Thank you!!"));
  display.display();

  buzzer.playFromProgramSpace(beepThankYou);
  delay(1000);
  display.clear();
  display.setLayout8x2();
}

// Blinks all three LEDs in sequence.
void ledDemo()
{
  displayBackArrow();

  uint8_t state = 3;
  static uint16_t lastUpdateTime = millis() - 2000;
  while (mainMenu.buttonMonitor() != 'B')
  {
    if ((uint16_t)(millis() - lastUpdateTime) >= 500)
    {
      lastUpdateTime = millis();
      state = state + 1;
      if (state >= 4) { state = 0; }

      switch (state)
      {
      case 0:
        buzzer.play("c32");
        display.gotoXY(0, 0);
        display.print(F("Red   "));
        ledRed(1);
        ledGreen(0);
        ledYellow(0);
        break;

      case 1:
        buzzer.play("e32");
        display.gotoXY(0, 0);
        display.print(F("Green"));
        ledRed(0);
        ledGreen(1);
        ledYellow(0);
        break;

      case 2:
        buzzer.play("g32");
        display.gotoXY(0, 0);
        display.print(F("Yellow"));
        ledRed(0);
        ledGreen(0);
        ledYellow(1);
        break;
      }
    }
  }

  ledRed(0);
  ledYellow(0);
  ledGreen(0);
}

void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  static const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, (char)255};
  display.print(barChars[height]);
}

void selfTestWaitShowingVBat()
{
  ledYellow(0);
  ledGreen(0);
  ledRed(0);
  while(!mainMenu.buttonMonitor())
  {
    display.gotoXY(0,0);
    display.print(' ');
    display.print(readBatteryMillivolts());
    display.print(F(" mV"));
    delay(100);
  }
}

void selfTestFail()
{
  display.gotoXY(0, 1);
  display.print(F("FAIL"));
  buzzer.playFromProgramSpace(beepFail);
  while(!mainMenu.buttonMonitor());
}

void selfTest()
{
  display.clear();
  display.print(F("3\xf7+ 32U4"));
  display.gotoXY(0, 1);
  display.print(F("SelfTest"));
  delay(1000);

  bumpSensors.calibrate();

  display.clear();
  display.print(F("Press"));
  display.gotoXY(0, 1);
  display.print(F("bumpers"));
  do
  {
     bumpSensors.read();
  }
  while(!bumpSensors.leftIsPressed() || !bumpSensors.rightIsPressed());

  buzzer.play("!c32");
  display.gotoXY(0, 1);
  display.print(F("        "));

  // test some voltages and IMU presence
  display.gotoXY(0, 0);
  display.print(F("USB "));
  if(usbPowerPresent())
  {
    display.print(F("on"));
    selfTestFail();
    return;
  }
  else
  {
    display.print(F("off"));
  }
  ledYellow(1);
  delay(500);

  display.gotoXY(0, 0);
  display.print(F("VBAT     "));
  int v = readBatteryMillivolts();
  display.gotoXY(4, 0);
  display.print(v);
  if(v < 4000 || v > 7000)
  {
    selfTestFail();
    return;
  }
  ledGreen(1);
  delay(500);

  display.gotoXY(0, 0);
  display.print(F("IMU     "));
  display.gotoXY(4, 0);
  if(!imu.init())
  {
    selfTestFail();
    return;
  }
  display.print(F("OK"));
  ledRed(1);
  delay(500);

  // test motor speed, direction, and encoders
  display.gotoXY(0, 0);
  display.print(F("Motors  "));
  ledYellow(1);
  ledGreen(1);
  ledRed(1);
  imu.configureForTurnSensing();

  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  motors.setSpeeds(90, -90);
  delay(250);

  // check rotation speed
  imu.read();
  int16_t gyroReading = imu.g.z;

  motors.setSpeeds(0, 0);
  delay(100);
  int left = encoders.getCountsAndResetLeft();
  int right = encoders.getCountsAndResetRight();
  display.clear();
  if(gyroReading > -7000 && gyroReading < -5000 &&
    left > 212 && left < 288 && right > -288 && right < -212)
  {
    display.print(F("Standrd?"));
  }
  else if(gyroReading > -1800 && gyroReading < -1200 &&
    left > 140 && left < 200 && right > -200 && right < -140)
  {
    display.print(F("Turtle?"));
  }
  else if(gyroReading > 9500 && gyroReading < 17000 &&
    left > 130 && left < 370 && right > -370 && right < -130)
  {
    display.print(F("Hyper?"));
  }
  else
  {
    display.clear();
    display.print(left);
    display.gotoXY(4, 0);
    display.print(right);

    display.gotoXY(4, 1);
    display.print(gyroReading/100);
    selfTestFail();
    return;
  }

  display.gotoXY(0,1);
  display.print(F("A=?  B=Y"));
  while(true)
  {
    char button = mainMenu.buttonMonitor();
    if(button == 'A')
    {
      display.clear();
      display.print(left);
      display.gotoXY(4, 0);
      display.print(right);

      display.gotoXY(0, 1);
      display.print(gyroReading);
    }
    if(button == 'B')
    {
      break;
    }
    if(button == 'C')
    {
      selfTestFail();
      return;
    }
  }

  // Passed all tests!
  display.gotoXY(0, 1);
  display.print(F("PASS    "));
  delay(250); // finish the button beep
  buzzer.playFromProgramSpace(beepPass);
  selfTestWaitShowingVBat();
}

// Spin in place
void goDemo()
{
  display.setLayout21x8();
  display.clear();
  display.print(F("Press A to continue.")); 
  display.gotoXY(0,7);
  display.print(F("\7 Press B to cancel."));

  bool waiting = true;
  while(waiting)
  {
    char button = mainMenu.buttonMonitor();
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
  displayBackArrow();
  display.print(F("Going"));
  display.gotoXY(5,1);
  display.print(F("..."));
  delay(200);
  buzzer.playFromProgramSpace(beepReadySetGo);
  while(buzzer.isPlaying())
  {
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
  goDemoInternal();
  motors.setSpeeds(0, 0);
}

void goDemoInternal()
{
  // Spin right
  display.print(F("Forward!"));
  bumpSensors.calibrate();
  delay(50);
  for(int i = 0; i < 200; i++)
  {
    motors.setSpeeds(i * 10, i * 10);
    delay(50);
    bumpSensors.read();
    if(mainMenu.buttonMonitor() == 'B'){
      return;
    }
    else if (bumpSensors.leftIsPressed() && ! bumpSensors.rightIsPressed())
    {

      display.clear();
      display.print(F("Right!")); 
      motors.setSpeeds(50,-50);
      delay(1500);
      display.clear();
    }
    else if (bumpSensors.rightIsPressed() && ! bumpSensors.leftIsPressed())
    {

      display.clear();
      display.print(F("Left!")); 
      motors.setSpeeds(-50,50);
      delay(1500);
      display.clear();
    }
     else if (bumpSensors.rightIsPressed()&& bumpSensors.leftIsPressed())
    {

      display.clear();
      display.print(F("Reversing!")); 
      motors.setSpeeds(-50,-50);
      delay(1500);
      motors.setSpeeds(-50, 50);
      delay(3000);
      display.clear();
    }
  }
}

// Spin in place
void spinDemo()
{
  display.setLayout21x8();
  display.clear();
  display.print(F("Warning: this will")); display.gotoXY(0,1);
  display.print(F("run both motors up to")); display.gotoXY(0,2);
  display.print(F("full speed for a few")); display.gotoXY(0,3);
  display.print(F("seconds.")); display.gotoXY(0,5);

  display.print(F("Press A to continue.")); display.gotoXY(0,7);
  display.print(F("\7 Press B to cancel."));

  bool waiting = true;
  while(waiting)
  {
    char button = mainMenu.buttonMonitor();
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
  displayBackArrow();
  display.print(F("Spinning"));
  display.gotoXY(5,1);
  display.print(F("..."));
  delay(200);
  buzzer.playFromProgramSpace(beepReadySetGo);
  while(buzzer.isPlaying())
  {
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
  spinDemoInternal();
  motors.setSpeeds(0, 0);
}

void spinDemoInternal()
{
  // Spin right
  for(int i = 0; i < 400; i++)
  {
    motors.setSpeeds(i * 10, -i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
  for(int i = 40; i >= 0; i--)
  {
    motors.setSpeeds(i * 10, -i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }

  // Spin left
  for(int i = 0; i < 400; i++)
  {
    motors.setSpeeds(-i * 10, i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
  for(int i = 40; i >= 0; i--)
  {
    motors.setSpeeds(-i * 10, i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
}

void beepDemo()
  {
  display.setLayout8x2();
  display.clear();
  display.print(F("Press A to beep!")); display.gotoXY(0,1);
  
  bool waiting = true;
  while(waiting)
  {
    char button = mainMenu.buttonMonitor();
    switch(button)
    {
    case 'B':
      display.setLayout8x2();
      return;
    case 'A':
      buzzer.playFromProgramSpace(beepPass);
    }
  }
}

void setup()
{
  static const PololuMenuItem mainMenuItems[] = {
    { F("Beep"), beepDemo},
    { F("Go"), goDemo },
    { F("Spin"), spinDemo },
  };
  mainMenu.setItems(mainMenuItems, sizeof(mainMenuItems)/sizeof(mainMenuItems[0]));
  mainMenu.setDisplay(display);
  mainMenu.setBuzzer(buzzer);
  mainMenu.setButtons(buttonA, buttonB, buttonC);
  mainMenu.setSecondLine(F("\x7f" "A \xa5" "B C\x7e"));

  loadCustomCharacters();

  // The brownout threshold on the ATmega32U4 is set to 4.3
  // V.  If VCC drops below this, a brownout reset will
  // occur, preventing the AVR from operating out of spec.
  //
  // Note: Brownout resets usually do not happen on the 3pi+
  // 32U4 because the voltage regulator goes straight from 5
  // V to 0 V when VIN drops too low.
  //
  // The bootloader is designed so that you can detect
  // brownout resets from your sketch using the following
  // code:
  bool brownout = MCUSR >> BORF & 1;
  MCUSR = 0;

  if (brownout)
  {
    // The board was reset by a brownout reset
    // (VCC dropped below 4.3 V).
    // Play a special sound and display a note to the user.

    buzzer.playFromProgramSpace(beepBrownout);
    display.clear();
    display.print(F("Brownout"));
    display.gotoXY(0, 1);
    display.print(F(" reset! "));
    delay(1000);
  }
  else
  {
    buzzer.playFromProgramSpace(beepWelcome);
  }

  // allow skipping quickly to the menu by holding button C
  if (buttonC.isPressed())
  {
    selfTest();
    return;
  }

  showSplash();

  mainMenuWelcome();
}

// Clear LEDs and show a message about the main menu.
void mainMenuWelcome()
{
  ledYellow(false);
  ledGreen(false);
  ledRed(false);
  display.clear();
  display.print(F("  Main"));
  display.gotoXY(0, 1);
  display.print(F("  Menu"));
  delay(1000);
}

void loop()
{
  if(mainMenu.select())
  {
    // a menu item ran; show "Main Menu" again and repeat
    mainMenuWelcome();
  }
}
