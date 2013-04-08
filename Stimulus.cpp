//#include "ofMain.h"
#include "Stimulus.h"
#include "EasingFunctions.h"


void StimulusControl::begin(GvsController &gvs_, int frameTime,
								Stimulus* list, int count, bool loop)
{
    begin( gvs, frameTime );
    play( list, count, loop );
}

void StimulusControl::begin(GvsController &gvs_, int frameTime)
{
	gvs = gvs_;
	frame = frameTime;

	arrived = true;

	easingFunc = Easer_easeInOutCubic;
	stimulusEndedFunc = 0;

	stimuliIndex = 0;

	reset();
}

// reset easer to initial conditions, does not nuke easingFunc or arrivedFunc
void StimulusControl::reset()
{
//    currPos = servo.read();
    levelCurrent = gvs.getLevel(); //_level;
    levelStart = levelCurrent;  // get everyone in sync
    levelDelta = 0;       // might be overwritten below

    if( stimuliCount > 0 ) {
        levelDelta = stimuli[ stimuliIndex ].level - levelStart;
        duration = stimuli[ stimuliIndex ].duration;
    }

    tickCount = duration / frame;
    tick = 0;
}

void StimulusControl::play( Stimulus* list, int count, bool loop)
{
    play( list, count, 0, 0, loop);
}

void StimulusControl::play( Stimulus* list, int count, int repetitions, bool loop )
{
    play( list, count, repetitions, 0, loop );
}

void StimulusControl::play( Stimulus* list, int count, int repetitions, int startFrom, bool loop)
{
    stimuli = list;
    stimuliCount = (count>0) ? count : 0;
    stimuliReps  = (repetitions>0)  ? repetitions  : 0;
    stimuliIndex = (startFrom>0) ? startFrom : 0;
    stimuliLoop = loop;

    running = true;
    arrived = false;

    reset();
}

void StimulusControl::easeTo( int level_, int duration_ )
{
    stimuliCount = 0;
    levelStart = levelCurrent;
    levelDelta = level_ - levelStart;
    duration = duration_;
    tickCount = duration / frame;
    tick = 0;
    arrived = false;
    running = true;
}

void StimulusControl::getNextStimulus()
{
    //ofLog() << ">> getNextStimulus " << ofGetElapsedTimeMillis();
    arrived = true;
    if( stimulusEndedFunc != 0 ) { stimulusEndedFunc( levelCurrent, stimuliIndex ); }

    // if not working through a list of stimuli
    if( (stimuliCount==0) || (0 == stimuli) ) {
        return;
    }

    stimuliIndex++;
    if( stimuliIndex == stimuliCount ) {
        stimuliIndex = 0;  // loop
        stimuliReps--;
        if( stimuliReps == 0 ) { // we are done
            //ofLog() << "stopping reps";
            stop();
        } else if( stimuliReps == -1 ) { // we are infinite
            stimuliReps = 0;
        }
    }
    levelStart = levelCurrent;

    levelDelta = stimuli[ stimuliIndex ].level - levelStart;
    duration = stimuli[ stimuliIndex ].duration;

    //ofLog() << ".nextStimulus " << duration << " (" << stimuli[ stimuliIndex ].level << ")";

    tickCount = duration / frame;
    tick = 0;
    arrived = false;
    //ofLog() << ".tickCount = " << tickCount << " / frame = " << frame;
}

void StimulusControl::update(unsigned long millis)
{
    if( ((millis - lastMillis) < frame) || !running ) {  // time yet?
        return;
    }
    lastMillis = millis;

    levelCurrent = easingFunc( tick, levelStart, levelDelta, tickCount );

    //Serial.println(levelCurrent);

    if( !arrived ) tick++;
    if( tick == tickCount ) {
        getNextStimulus();
    }
}


void StimulusControl::start()
{
    running = true;
}

void StimulusControl::stop()
{
    running = false;
}

bool StimulusControl::isRunning()
{
    return running;
}

bool StimulusControl::hasArrived()
{
    return arrived;
}

void StimulusControl::setEasingFunc( EasingFunc func )
{
    easingFunc = func;
}

void StimulusControl::setStimulusEndedFunc( StimulusEndedFunc func )
{
    stimulusEndedFunc = func;
}






