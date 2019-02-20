#include "sleep.h"

//sleep.ino

//Setup for sleeping
void sleepInit(){
  
  //Save Power by writing all Digital IO LOW - note that pins just need to be tied one way or another, do not damage devices!
  //Use exceptions for pins which need to be used for measurments or other stuff
  for (int i = 0; i < 14; i++) {
    if(i != 2)
      pinMode(i, OUTPUT);
  }

  attachInterrupt(0, digitalInterrupt, FALLING); //interrupt for waking up, attach button and stuff

  //SETUP WATCHDOG TIMER
  WDTCSR = (24);//change enable and WDE - also resets
  WDTCSR = (33);//prescalers only - get rid of the WDE and WDCE bit
  WDTCSR |= (1<<6);//enable interrupt mode


  //ENABLE SLEEP - this enables the sleep mode
  SMCR |= (1 << 2); //power down mode
  SMCR |= 1;//enable sleep
}

void sleep(int nCycles){
	
  //disable ADC
  ADCSRA &= ~(1 << 7);
  // sleep for nCycles*8 sec
  for(int i = 0; i < nCycles; i++){
    //delay();
    
    //BOD DISABLE - this must be called right before the __asm__ sleep instruction
    MCUCR |= (3 << 5);                      //set both BODS and BODSE at the same time
    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
    __asm__  __volatile__("sleep");         //in line assembler to go to sleep
  }
}

void goodMorning(){
  //enable ADC
  ADCSRA |= (1 << 7);
}
