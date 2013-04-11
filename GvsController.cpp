#include "GvsController.h"
#include "GvsConfig.h"
#include "Utils.h"

GvsController::GvsController() {
}

void GvsController::selfTest() {
}

void GvsController::setup() {
  // INPUTS
  pinMode(PIN_CURRENT_IN, INPUT); 
  pinMode(PIN_OUTPUTVOLT_IN, INPUT);
  pinMode(PIN_BATTERY_IN, INPUT);
  pinMode(PIN_ACCELX_IN, INPUT);
  pinMode(PIN_ACCELY_IN, INPUT);
  pinMode(PIN_ACCELZ_IN, INPUT);
  pinMode(PIN_MANUALSWITCH_IN, INPUT);
  // OUTPUTS
  pinMode(PIN_LEVEL_OUT, OUTPUT); 
  pinMode(PIN_DIRECTION_OUT, OUTPUT);

  // LEDs ouptputs for status indication
  pinMode(PIN_LED_LEFT, OUTPUT);
  pinMode(PIN_LED_RIGHT, OUTPUT);
  pinMode(PIN_LED_MANUAL, OUTPUT);
  pinMode(PIN_LED_AUTO, OUTPUT);
  pinMode(PIN_LED_POWER, OUTPUT);
  
  _mode = MODE_AUTO;
  _lastMode = _mode;

  _dir  = DIR_LEFT;
  _level = 0;

  _calibrated = false;
  
  _current = 0;
  _voltage = 0;
  _battery = 0;

  _calibrationx = 0;
  _calibrationy = 0;
  _calibrationz = 0;
}

void GvsController::update() {
  //_mode = readSwitchMode();
  readDeviceStatus();
  readAccelValues();
  
  // update level of stimulation
  if(_level != _lastLevel) {
    int lvl = abs(_level);
    analogWrite(PIN_LEVEL_OUT, lvl);
  }

  // set direction LEDs properly
  if(_level < -1) {
    digitalWrite(PIN_LED_LEFT, HIGH);
    digitalWrite(PIN_LED_RIGHT, LOW);
  } else if (_level > 1) {
    digitalWrite(PIN_LED_LEFT, LOW);
    digitalWrite(PIN_LED_RIGHT, HIGH);
  } else {
    digitalWrite(PIN_LED_LEFT, LOW);
    digitalWrite(PIN_LED_RIGHT, LOW);
  }

  if(LOW == digitalRead(PIN_MANUALSWITCH_IN) ) {
    digitalWrite(PIN_LED_MANUAL, HIGH);
    digitalWrite(PIN_LED_AUTO, LOW);
  } else {
    digitalWrite(PIN_LED_MANUAL, LOW);
    digitalWrite(PIN_LED_AUTO, HIGH);
  }
}

void GvsController::calibrate(unsigned long ms) {
  if(_calibrated == false) {
    int i = 0;
    long ax = 0, ay = 0 , az = 0;
    // take one sample every srate milliseconds
    int srate = 200;
    // how many smaples to take from the time requested for calibration
    int samples = ms / srate;
    // loop untill all samples are collected    
    for(i = 0; i < samples; i++) {
      ax += analogRead(PIN_ACCELX_IN);
      ay += analogRead(PIN_ACCELY_IN);
      az += analogRead(PIN_ACCELZ_IN);
      delay(srate);
    }
    // find average and consider those our calibration values
    _calibrationx = ax / samples;
    _calibrationy = ay / samples;
    _calibrationz = az / samples;
    // flag this device as calibrated
    _calibrated = true;
  }
}

int GvsController::stableAnalogRead(int pin) {
  int accum;
  for(int i = 0; i < 4; i++) {
    accum += analogRead(pin);
  }
  return ceil(accum/4.0);
}

void GvsController::readAccelValues() {
  int xread, yread, zread;

  _accx = stableAnalogRead(PIN_ACCELX_IN);
  _accy = stableAnalogRead(PIN_ACCELY_IN);
  _accz = stableAnalogRead(PIN_ACCELZ_IN);

  _accx = _accx - _calibrationx;
  _accy = _accy - _calibrationy;
  _accz = _accz - _calibrationz;
}

void GvsController::readDeviceStatus() {
  _mode = readSwitchMode();
  
  // set stimulation level to 0 when device changes status
  if(_mode != _lastMode) { 
    _level = 0; 
    _lastMode = _mode; 
  }

  _current = analogRead(PIN_CURRENT_IN);
  _voltage = analogRead(PIN_OUTPUTVOLT_IN);
  _battery = analogRead(PIN_BATTERY_IN);
}

int GvsController::readSwitchMode() {
  return ((HIGH == digitalRead(PIN_MANUALSWITCH_IN)) ? MODE_MANUAL : MODE_AUTO);
}


void GvsController::switchToLeft() {
  // set current vector pin
  digitalWrite(PIN_DIRECTION_OUT, HIGH);
  // show state on LED
  digitalWrite(PIN_LED_LEFT, HIGH);
  digitalWrite(PIN_LED_RIGHT, LOW);
  _dir = DIR_LEFT;
}

void GvsController::switchToRight() {
  // set current vector pin
  digitalWrite(PIN_DIRECTION_OUT, LOW);
  // show state on LED
  digitalWrite(PIN_LED_LEFT, LOW);
  digitalWrite(PIN_LED_RIGHT, HIGH);
  _dir = DIR_RIGHT;
}

  void GvsController::setLevel(int lvl) {
    // negative values are left-sided stimulations
    if(lvl < 0) {
      switchToLeft();
    } else {
      switchToRight();
    }

    // clamp to 8-bit (necessary for pwm)
    _level = clamp(lvl, -255, 255);
  }

  int GvsController::getLevel() { 
    return _level;
  }
