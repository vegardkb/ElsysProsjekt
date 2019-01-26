#include <TheThingsNetwork.h>

/*Basert på
http://www.kevindarrah.com/download/arduino_code/LowPowerVideo.ino
samt the things uno quick start guide.
Her er ikke digitalinterrupt implementert, wakeup kun gjennom watchdog
*/

//IKKE TESTET ENDA



const char *appEui = "70B3D57ED0016ADA";
const char *appKey = "9BA875FB5C9B81205BFE16AB99D8D22C";
    
#define loraSerial Serial1
#define debugSerial Serial
#define aref_voltage 3.3
#define freqPlan TTN_FP_EU868
    
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);


//global variables
const int tempPin = 1;        //the analog pin the TMP36's Vout (sense) pin is connected to
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures
int tempReading;        // the analog reading from the sensor



void setup() {
  //Save Power by writing all Digital IO LOW - note that pins just need to be tied one way or another, do not damage devices!
  for (int i = 0; i < 20; i++) {
    pinMode(i, OUTPUT);
  }

  //SETUP WATCHDOG TIMER
  WDTCSR = (24);//change enable and WDE - also resets
  WDTCSR = (33);//prescalers only - get rid of the WDE and WDCE bit
  WDTCSR |= (1<<6);//enable interrupt mode

  //Disable ADC - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
  ADCSRA &= ~(1 << 7);

  //ENABLE SLEEP - this enables the sleep mode
  SMCR |= (1 << 2); //power down mode
  SMCR |= 1;//enable sleep
  
  loraSerial.begin(57600);
  debugSerial.begin(9600);
      
    
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);
    
  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);


  // If you want to set the aref to something other than 5v
  analogReference(EXTERNAL);
}

void loop() {
  //TempSensorAnalog
  tempReading = analogRead(tempPin);
  
  byte payload[2];
  payload[0] = highByte(tempReading);
  payload[1] = lowByte(tempReading);

    
  // Send it off
  ttn.sendBytes(payload, sizeof(payload));
      


  //BOD DISABLE - this must be called right before the __asm__ sleep instruction
  MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
  MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
  __asm__  __volatile__("sleep");//in line assembler to go to sleep

}


ISR(WDT_vect){
  //DON'T FORGET THIS!  Needed for the watch dog timer.  This is called after a watch dog timer timeout - this is the interrupt function called after waking up
}// watchdog interrupt
