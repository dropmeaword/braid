/**
 * Software for BRAID, a performantive environment for networked proprioception.
 *
 * (cc) nc-at 2013 Luis Rodil-Fernandez <dropmeaword@gmail.com>
 */
#include <Arduino.h>
#include <stdio.h>
#include <stdarg.h>

#include <Base64/Base64.h>
#include <Streaming/Streaming.h>
#include "CmdMessenger.h"

#include "GvsConfig.h"
#include "Utils.h"
#include "Stimulus.h"

// Serial messaging setup
char field_separator = ' ';
char command_separator = ';';
CmdMessenger cmdMessenger = CmdMessenger(Serial, field_separator, command_separator);


// reference timestamp
unsigned long tmarkStatus = 0;
unsigned long tmarkMeasurement = 0;
unsigned long int interval1 = INTERVAL_MEASUREMENT;
unsigned long int interval2 = INTERVAL_STATUS;

unsigned long tmarkTest = 0;

boolean sustain = false;
boolean sustain2 = false;

boolean otherIsOnline = false;

// running average init
int mra_samples = 3*8;
RunningAverage mra(mra_samples);

char sercommand[200]; // incoming serial command
int cidx;

// serial number of data packet sent out
unsigned long framenum = 0;

GvsController gvs;
StimulusControl stim;

/*
 * stlimulus level should be clamped at 255, if you pass more than 8-bit resolution values the arduino starts sending really funky signals.
 */
 /*
#define ST_COUNT 3
Stimulus stimuli[ST_COUNT] = {
    {128, 80},
    {255, 80},
    {0, 2000},
};
*/


#define ST_COUNT 11
Stimulus stimuli[ST_COUNT] = {
    {50, 500},
    {300, 20},
    {380, 60},
    {460, 40},
    {100, 50},
    {200, 50},
    {900, 500},
    {500, 1000},
    {30,  50},
    {10,  75},
    {0, 10},
    // 2.235s
};


// ///////////////////////////////////////////////////////////////////////////
// MESSAGING
// ///////////////////////////////////////////////////////////////////////////
enum
{
  kACK           = 1, // acknowledge
  kACC           = 2, // accelerometer
  kCTR           = 3, //3, // control command
  kSTT           = 4, // status command
  kLOG           = 5, // device specific log message

  kSEND_CMDS_END, // marker
};

/**
 * Convert reference value from accelero into stimulation levels for this device.
 * The reference of the accelero is the maximum value of the three axis readings.
 */
int convertAccel2Stimulation(int ref) {
  float fref = float(ref) / 256;
  float sk = pow(fref, 3); // calculate scaling exponential smoothing factor
  int retval = ceil(float(GVS_CLAMP) * sk * 4.0);
  return clamp(retval, -255, 255);
}

// ///////////////////////////////////////////////////////////////////////////
// MESSAGING
// ///////////////////////////////////////////////////////////////////////////

void setup()  {
  analogReference(DEFAULT);
  
  /*
   * Using the following line in your setup routine will 
   * set the PWM frequency to 31250 Hz.
   * http://playground.arduino.cc/Code/PwmFrequency
   */
  TCCR2B = TCCR2B & 0b11111000 | 1;
  
  cidx = 0;
  
  Serial.begin(COMM_BAUD_RATE);

  gvs.setup();
  
  // Serial messaging
  cmdMessenger.print_LF_CR(); // make output more readable

  // run self-test on startup
  gvs.bootTest();
  
  // acknowledge this device is present and ready
  sendAck();
  gvs._mode = MODE_MANUAL;
  stim.begin(gvs, 1, stimuli, ST_COUNT);
} 

void sendInclination() {
  char msg[128];
  sprintf(msg, "%s %lu %d %d %d %d", UNIT_ID, framenum++, gvs._accx, gvs._accy, gvs._accz, gvs.getLevel());
  cmdMessenger.sendCmd(kACC, msg);
}

void sendStatus() {
  char msg[128];
  sprintf(msg, "%s %lu %d %d %d %d", UNIT_ID, framenum++, gvs._mode, gvs._current, gvs._voltage, gvs._battery);
  cmdMessenger.sendCmd(kSTT, msg);
}

void sendLog(char *log_) {
  char msg[128];
  sprintf(msg, "%s %lu %s", UNIT_ID, framenum++, log_);
  cmdMessenger.sendCmd(kLOG, msg);
}

void sendAck() {
  digitalWrite(PIN_LED_POWER, HIGH);
  char msg[128];
  sprintf(msg, "%s %lu", UNIT_ID, framenum++);
  cmdMessenger.sendCmd(kACK, UNIT_ID);
}

void loop()  {
  // send inclination data as often as possible
  if( timeout(interval1, &tmarkMeasurement) ) {
    gvs.readAccelValues();
    // send thru serial
    sendInclination();
  }

  
  // send the status only every now and then
  if( timeout(interval2, &tmarkStatus) ) {
    gvs.readDeviceStatus();
    sendStatus();
  }

  // run a self-test step every second
  if( timeout(1*1000, &tmarkTest) ) {
    gvs.selfTest();
  }

  unsigned long m = millis();
  stim.update( m );
  int lvl = ceil(stim.getStimulationLevel());
  gvs.setLevel( lvl );
  gvs.update();
} // loop()

