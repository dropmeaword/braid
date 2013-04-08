#ifndef __EASER_H__
#define __EASER_H__

#include "EasingFunctions.h"
#include "GvsController.h"

#ifdef ARDUINO
#   if (ARDUINO >= 100)
#       include <Arduino.h>
#   else
#       include <WProgram.h>
#   endif
#endif

typedef struct _stimulus {
    int level;    // level of stimulus
    int duration; // duration in milliseconds to get to and stay at that position
} Stimulus;


// define "EasingFunc" callback function
// you can make your own of these to use
// t: current time, b: beginning value, c: change in value, d: duration
// t and d can be in frames or seconds/milliseconds
//
typedef float (*EasingFunc)(float t, float b, float c, float d);

// define "ArrivedFunc" to be called when servo arrives at position
// arguments provided are: currPos of servo & movesIndex of move list (if appl)
typedef void (*StimulusEndedFunc)(int level, int index);


class StimulusControl
{

  GvsController gvs;

private:
    unsigned long frame;

    float levelStart;
    float levelCurrent;
    float levelDelta;

    unsigned long duration;
    unsigned long tick;
    unsigned long tickCount;
    unsigned long lastMillis;

    Stimulus* stimuli;
    int stimuliIndex;
    int stimuliCount;
    int stimuliReps;
    bool stimuliLoop;

    EasingFunc easingFunc;
    StimulusEndedFunc stimulusEndedFunc;

    bool running;
    bool arrived;


public:

    // set up a servoeaser to use a particular servo
    //void begin(Servo s, int frameTime, int startPos);
    void begin(GvsController &gvs, int frameTime); //, int startPos);
    void begin(GvsController &gvs, int frameTime, Stimulus* list, int cnt, bool loop = false);

    void reset();

    // @param count is number of moves in the list
    // @param repetitions is the number of times to repeat
    void play( Stimulus* stimuli_, int cnt, bool loop = false);
    void play( Stimulus* stimuli_, int cnt, int repetitions, bool loop = false);
    void play( Stimulus* stimuli_, int cnt, int repetitions, int startFrom, bool loop = false);

    // for manual stimulation
    void easeTo( int level_, int duration_ );

    float getStimulationLevel() { return levelCurrent; }

    void getNextStimulus();

    void update(unsigned long millis);

    void setEasingFunc( EasingFunc func );
    void setStimulusEndedFunc( StimulusEndedFunc func );

    bool hasArrived();
    bool isRunning();

    void start();
    void stop();

}; // class Stimulation

#endif // __EASER_H__

