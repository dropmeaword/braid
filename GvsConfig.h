#ifndef __GVSCONFIG_H__
#define __GVSCONFIG_H__

#include "Arduino.h"


#define UNIT_ID "B"
#define UNIT_ID_OTHER "A"

#define MAX_VOLTAGE 24   // in V
#define MAX_CURRENT 4.5  // in mA

#define INTERVAL_STATUS 1*500 // ms
#define INTERVAL_MEASUREMENT 1*35 // ms

#define COMM_BAUD_RATE 57600

/**
 * Inputs:
 * 1) current limit (used to read the current limit value)
 * 2) output voltage (used to see what the voltage on the output is)
 * 3) accelero X
 * 4) accelero Y
 * 5) accelero Z
 * 6) battery voltage (used to read the battery voltage, to avoid playing with a half dead battery)
 */
// IN
// (analog)
#define PIN_CURRENT_IN      A1
#define PIN_OUTPUTVOLT_IN   A0
#define PIN_ACCELX_IN       A2
#define PIN_ACCELY_IN       A3
#define PIN_ACCELZ_IN       A4
#define PIN_BATTERY_IN      A5
// (digital)
#define PIN_MANUALSWITCH_IN  10

#define PIN_DIRECTION_IN 9

// OUT
#define PIN_LEVEL_OUT      3 // stimulation level (pwm)
#define PIN_DIRECTION_OUT  2 // stimulation direction (bin)

// control LEDs
#define PIN_LED_LEFT 4
#define PIN_LED_RIGHT 5
#define PIN_LED_MANUAL 6
#define PIN_LED_AUTO 7
#define PIN_LED_POWER 8

#endif  // __GVSCONFIG_H__
