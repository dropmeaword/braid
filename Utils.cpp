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

int clamp(int val, int min, int max)
{
    if (val < min)
        val = min;
    else if (val > max)
        val = max;
    return val;
}

int smaxabs2(int a, int b) {
  if((a < 0) && (b < 0)) {
		return min(a, b);
	} else if ((a < 0) && ( b > 0)) {
		return ((abs(a) > b) ? a : b);
	} else if ((a > 0) && ( b < 0)) {
		return ((abs(b) > a) ? b : a);
	} else {
		return max(a, b);
	}
}
 
int smaxabs3(int a, int b, int c) {
	int tmp = smaxabs2(a, b);
	return smaxabs2(c, tmp);
}
