#include "Utils.h"


int timeout(unsigned long tinterval, unsigned long *treference){
  unsigned long timeCurrent;
  unsigned long timeElapsed;
  int result = false;
  
  timeCurrent = millis();
  if(timeCurrent < *treference) { // rollover detected
    timeElapsed = (TIMECTL_MAXTICKS-*treference)+timeCurrent; //elapsed=all the ticks to overflow + all the ticks since overflow
  }
  else {
    timeElapsed = timeCurrent - *treference;
  }

  if(timeElapsed >= tinterval) {
    *treference = timeCurrent;
    result = true;
  }
  
  return(result);
}

unsigned long xmap(unsigned long x, unsigned long in_min, unsigned long in_max, unsigned long out_min, unsigned long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

