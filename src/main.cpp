/*********************************************************************************************************************
 ** 2017-06-15, RDU: Built for stepper motor
 **                  https://www.arduino.cc/en/Tutorial/StepperOneStepAtATime
 **                  https://github.com/esp8266/Arduino/blob/master/variants/d1_mini/pins_arduino.h#L40-L50
 **                  https://github.com/arduino/Arduino/tree/master/libraries/Stepper/src
 ** 2017-09-03, RDU: Added NTP for initial time setup
 *********************************************************************************************************************/
#include <Homie.h>
#include <Stepper.h>

#include <time.h>
//#include <unistd.h>
//#include <TimeLib.h>
//#include <Time.h>
//#include <Timezone.h>



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
  // Software specifications
    #define FW_NAME    "Clock24h"
    #define FW_VERSION "0.17.9.3"

  // Hardware specs
    const int stepsPerRevolution = 64;
    const int motorRpm = 300;

  // Sensor consts/vars
    HomieNode stepperNode("switch", "switch");
    Stepper myStepper(stepsPerRevolution, 14, 13, 12, 15);
    unsigned long stepCount = 0UL;

  // Loop control
    unsigned int loopInterval = 1000UL; // ms
    unsigned long lastLoopExec = 0UL;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Business logic
void testTime()
{
  int timeZone = 2;
  int dst = 1;
  configTime(timeZone, dst * 3600, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  delay(1000);
  delay(0);
  delay(1000);
  delay(0);
  delay(1000);
  delay(0);

  while (time(nullptr)) {
    Serial.print(".");
    delay(500);
    delay(0);
  }
  time_t now = time(nullptr);
  struct tm *tmNow = localtime(&now);
  Serial << ctime(&now) << "\tHour=" << tmNow->tm_hour << "\tMinute=" << tmNow->tm_min << "\tSecond=" << tmNow->tm_sec << endl;

  // TODO: Returns UTC!
  // https://github.com/JChristensen/Timezone/blob/master/examples/WorldClock/WorldClock.ino
  /*TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
  TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
  Timezone CE(CEST, CET);
  TimeChangeRule *tcr;
  printTime(CE.toLocal(tmNow, &tcr), tcr -> abbrev, "Paris");
  */
  while(1) {delay(0);}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Homie Setup Handler
void setupHandler() {
  testTime();

  return;

  myStepper.setSpeed(motorRpm);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Homie loop Handler
void loopHandler() {
  return;
  if (millis() - lastLoopExec >= loopInterval /** 1000UL*/ || lastLoopExec == 0) {
    int degrees = 180;

    // http://www.instructables.com/id/BYJ48-Stepper-Motor/
    //int steps = (float)(32 * (float)stepsPerRevolution) * (float)((float)degrees / 360);

    //steps = stepsPerRevolution; // To force a full revolution at once
    int steps = 32 * 64;
    myStepper.step(steps);
    stepCount += steps;
    Serial << "\tRPM: " << motorRpm
           << "\t\tsteps: " << steps
           << "\t\tstepCount: " << stepCount
           << "\t\tlastLoopExec: " << lastLoopExec
           << endl;

    lastLoopExec = millis();
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino Setup Handler
void setup() {
  // Serial setup
  Serial.begin(115200); // Required to enable serial output
  Serial << endl << endl;

  // Firmware setup
  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(setupHandler);

  // Device setup
  Homie.setLoopFunction(loopHandler); // Move after setupHandler definition?
  //Homie.disableLedFeedback();
  Homie.setup();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino Loop Handler
void loop() {
  Homie.loop();
}
