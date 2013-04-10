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

boolean sustain = false;
boolean sustain2 = false;

boolean otherIsOnline = false;

// running average init
int mra_samples = 33; //1000/INTERVAL_MEASUREMENT;
RunningAverage mrax(mra_samples);
RunningAverage mray(mra_samples);
RunningAverage mraz(mra_samples);

char sercommand[200]; // incoming serial command
int cidx;

GvsController gvs;
StimulusControl stim;

/*
 * stlimulus level should be clamped at 255, if you pass more than 8-bit resolution values the arduino starts sending really funky signals.
 */
#define ST_COUNT 3
Stimulus stimuli[ST_COUNT] = {
    {128, 80},
    {255, 80},
    {0, 2000},
};

/*
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
*/

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

// callback control command
void cb_acknowledge() {
  char *s = cmdMessenger.getAsString();
  if( 0 == strcmp(UNIT_ID_OTHER, s) ) {
    otherIsOnline = true;
  }
}

// callback control command
void cb_control() {
  // if we receive a control message and we are in manual mode
  // parse it and set stimulation level
  //sendLog("rcv_ctrl_msg");
  if(MODE_MANUAL == gvs._mode) {
    //sendLog("ctrl_msg_get_device");
    char *s = cmdMessenger.getAsString();
    // is the message meant for me?
    if( 0 == strcmp(UNIT_ID, s) ) {
      //sendLog("ctrl_msg_processing");
      int level = cmdMessenger.getAsLong();
      
      //if( stim.hasArrived() ) { // if last stimulus has been fulfilled
        stim.easeTo(level, 200);
      //}
      //long int duration = cmdMessenger.getAsLong();
      //Serial.print(level);
      //Serial.println(" << ");
      //Serial.print(duration);
    }
  } // if manual mode
}

/**
 * Convert accelerometer values from the other device
 * into stimulation levels for this device.
 */
int convertAccel2Stimulation(int xa, int ya, int za) {
  // convert from 10-bit to [-1.0, 1.0]
//  int calibration = 512;

  float fxa = xa / 512;
  //float fya = (ya - calibration) / 512;
  //float fza = (za - calibration) / 512;

  float sk = pow(fxa, 9); // calculate scaling exponential smoothing factor
  return ceil(GVS_CLAMP * sk);
}

// callback to process inclination of the other device
void cb_other_inclination() {
  //sendLog("received_acc_msg");
  if(MODE_AUTO == gvs._mode) {
    char *s = cmdMessenger.getAsString();

    //sendLog(s);
    /*
    Serial.print("I am ");
    Serial.print(UNIT_ID); 
    Serial.print(" and I have received a message from ");
    */

    //sendLog("parsing_acc_msg");
    // if msg comes from the other device
    if( 0 == strcmp(UNIT_ID_OTHER, s) ) {
      //sendLog("acc_msg_processing_other");
      // parse inclination data
      int ox = cmdMessenger.getAsLong();
      int oy = cmdMessenger.getAsLong();
      int oz = cmdMessenger.getAsLong();
      
      if( (ox < -200) || (ox > 200) ) {
        // conver it to a stimulation level
        int st = convertAccel2Stimulation(ox, oy, oz);
        stim.easeTo(st, 500);
      }
      //gvs.setLevel( st );
    }
  }
}

void msg_attach_callbacks()
{
  cmdMessenger.attach(kACK, cb_acknowledge);
  cmdMessenger.attach(kCTR, cb_control);
  cmdMessenger.attach(kACC, cb_other_inclination);
}

// ///////////////////////////////////////////////////////////////////////////
// MESSAGING
// ///////////////////////////////////////////////////////////////////////////

void setup()  {
  /*
   * set the analog reference voltage to extern
   * http://arduino.cc/en/Reference/AnalogReference?from=Reference.AREF
   *
   * Rene's design uses 3.3V as reference for all analog inputs
   */
  analogReference(EXTERNAL);
  
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
  cmdMessenger.print_LF_CR(); // make output more readable whilst debugging in Arduino Serial Monitor
  msg_attach_callbacks();
  
  // acknowledge this device is present and ready
  sendAck();

  stim.begin(gvs, 1); //, stimuli, ST_COUNT);
} 


void sendInclination() {
  char msg[128];
  sprintf(msg, "%s %d %d %d %d", UNIT_ID, gvs._accx, gvs._accy, gvs._accz, gvs.getLevel());
  cmdMessenger.sendCmd(kACC, msg);
}

void sendStatus() {
  char msg[128];
  sprintf(msg, "%s %d %d %d %d", UNIT_ID, gvs._mode, gvs._current, gvs._voltage, gvs._battery);
  cmdMessenger.sendCmd(kSTT, msg);
}

void sendLog(char *log_) {
  char msg[128];
  sprintf(msg, "%s %s", UNIT_ID, log_);
  cmdMessenger.sendCmd(kLOG, msg);
}

void sendAck() {
  digitalWrite(PIN_LED_POWER, HIGH);
  cmdMessenger.sendCmd(kACK, UNIT_ID);
}

void loop()  {
  // if not calibrated, take five seconds to calibrate
  if(gvs._calibrated == false) {
    gvs.calibrate(5000);
    char calib[128];
    sprintf(calib, "calib_(%d,%d,%d)", gvs._calibrationx, gvs._calibrationy, gvs._calibrationz);
    sendLog(calib);
  }
  // process incoming serial data
  cmdMessenger.feedinSerialData();

  // @todo send inclination data as often as possible
  if( timeout(interval1, &tmarkMeasurement) ) {
    gvs.readAccelValues();
    // update running average
    mrax.addValue(gvs._accx);
    mray.addValue(gvs._accz);
    mraz.addValue(gvs._accz);
    // send thru serial
    sendInclination();
  }
  
  // @todo  send the status only every now and then (maybe every 5 seconds)
  if( timeout(interval2, &tmarkStatus) ) {
    gvs.readDeviceStatus();
    sendStatus();
  }


  if(MODE_MANUAL == gvs._mode) {
    if( (gvs._accx < -200) || (gvs._accx > 200) ) {
      int stimulus = convertAccel2Stimulation(gvs._accx, gvs._accy, gvs._accz);
      stim.easeTo(stimulus, 500);
    }
  }

  unsigned long m = millis();
  stim.update( m );
  int lvl = ceil(stim.getStimulationLevel());
  gvs.setLevel( lvl );
  gvs.update();

} // loop()

