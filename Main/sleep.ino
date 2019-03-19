#include "sleep.h"

#if defined (__AVR__)
#include <avr/power.h>
#endif

//sleep.ino

//Setup for sleeping
void sleepInit(){
  
  //Save Power by writing all Digital IO LOW - note that pins just need to be tied one way or another, do not damage devices!
  //Use exceptions for pins which need to be used for measurments or other stuff
  for (int i = 0; i < 14; i++) {
    pinMode(i, OUTPUT);
  }

  attachInterrupt(12, digitalInterrupt, FALLING); //interrupt for waking up, attach button and stuff

  //SETUP WATCHDOG TIMER
  WDTCSR = (24);//change enable and WDE - also resets
  WDTCSR = (33);//prescalers only - get rid of the WDE and WDCE bit
  WDTCSR |= (1<<6);//enable interrupt mode

  //ENABLE SLEEP - this enables the sleep mode
  SMCR |= (1 << 2); //power down mode
  SMCR |= 1;//enable sleep
  
}

void gotoSleep(int nCycles){  
  //disable ADC
  ADCSRA &= ~(1 << 7);
  
  ttn.sleep(10000);
  pinMode(radioResetPin, OUTPUT);
  digitalWrite(radioResetPin, LOW);
  
  pinMode(radioSwitch, OUTPUT);
  digitalWrite(radioSwitch, LOW);

  
  loraSerial.end();
  Serial1.end();
  
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);


  power_usart1_disable();
  power_timer3_disable();
  power_timer1_disable();
  power_timer0_disable();
  power_spi_disable();
  power_twi_disable();
  power_usb_disable();

  
  digitalWrite(radioSwitch, LOW);
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

  power_usart1_enable();
  power_timer3_enable();
  power_timer1_enable();
  power_timer0_enable();
  power_spi_enable();
  power_twi_enable();
  power_usb_enable();

  //Serial1.begin(57600);
  loraSerial.begin(57600);

  //enable ADC
  ADCSRA |= (1 << 7);
  
}
