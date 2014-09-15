#ifndef GVSCONTROLLER_H
#define GVSCONTROLLER_H

#include "Arduino.h"
#include "RunningAverage.h"

#define DIR_LEFT  2
#define DIR_RIGHT 4

#define MODE_MANUAL 1
#define MODE_AUTO   1<<1

#define GVS_CLAMP 255

class GvsController
{
  int _cinterval;  // interval in between steps
  int _csteps;      // steps to get from current level to desired level
  unsigned long _tmarkStep;
  
public:
  int _dir;  /* direction of stimulation */
  int _strength; /* strength of stimulation */
  boolean _sustained;

  int _lastMode;
  int _mode;
  
  int _level;
  int _lastLevel;
  
  int _accx;
  int _accy;
  int _accz;

  double _angx;
  double _angy;
  double _angz;
  
  int _current;
  int _voltage;
  int _battery;

  int _calibrationx;  
  int _calibrationy;  
  int _calibrationz;  
  boolean _calibrated;

  int _outputTest;
  int _stepTest;

public:
  GvsController();
  
  void update();

  void setup();

  void calibrate(unsigned long ms);

  int stableAnalogRead(int pin);

  int readSwitchMode();
  void readAccelValues();
  void readDeviceStatus();
  
  void switchToLeft();
  void switchToRight();
  
  void setLevel(int lvl);
  int getLevel();
};

#endif // GVSCONTROLLER_H

