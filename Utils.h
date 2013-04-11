/**
 * (cc) nc-at 2013 Luis Rodil-Fernandez <dropmeaword@gmail.com>
 */
#include "Arduino.h"

#define TIMECTL_MAXTICKS 4294967295L
#define TIMECTL_INIT 0

extern "C" {
  /** 
   * Trigger a timeout within a given interval
   * @param tinterval duration of interval
   * @param treference pointer to variable containing the last time the trigger was activated
   * @returns 1 if timeout is triggered, 0 otherwise
   */
  int timeout(unsigned long tinterval, unsigned long *treference);
  /** 
   * Map a value from one range to another (e.g. map value 0.5 from range [-1.0, 1.0] to range [0, 20.0])
   * @param x value to map
   * @param in_min minimum of input range
   * @param in_max maximum of input range
   * @param out_min minimum of output range
   * @param out_max maximum of output range
   * @returns value mapped within output range
   */
  unsigned long xmap(unsigned long x, unsigned long in_min, unsigned long in_max, unsigned long out_min, unsigned long out_max);
  /** 
   * Clamp integer value between a minimum boundary and a maximum boundary
   * @param val_ value to clamp
   * @param min_ minimum possible value
   * @param max_ maxmum possible value
   * @returns clamped value
   */
  int clamp(int val_, int min_, int max_);
  
  /** 
   * Find maximum absolute value and return it signed
   * @param a input value
   * @param b input value
   * @returns greates value in absolute terms with its original sign
   */
  int smaxabs2(int a, int b);
  
  /**
   * Same as above but with three input values (this is used for example, to get the maximum value read from a calibrated three axis accelero)
   */
  int smaxabs3(int a, int b, int c);
}

