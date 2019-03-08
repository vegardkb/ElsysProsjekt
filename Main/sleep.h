#ifndef SLEEP_H_
#define SLEEP_H_

//sleep.h
//Functions related to sleep


//Setup for sleeping
static void sleepInit();

//Go to sleep
static void sleep(int nCycles);

//Wake up
static void goodMorning();


ISR(WDT_vect){
  //DON'T FORGET THIS!  Needed for the watch dog timer.  This is called after a watch dog timer timeout - this is the interrupt function called after waking up
}// watchdog interrupt

#endif
