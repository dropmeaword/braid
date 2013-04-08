/**
 * (cc) nc-at 2013 Luis Rodil-Fernandez <dropmeaword@gmail.com>
 */
#include "Arduino.h"

#define TIMECTL_MAXTICKS 4294967295L
#define TIMECTL_INIT 0

extern "C" {
  int timeout(unsigned long tinterval, unsigned long *treference);
  unsigned long xmap(unsigned long x, unsigned long in_min, unsigned long in_max, unsigned long out_min, unsigned long out_max);
}

