#include <TheThingsNetwork.h>
#include "measurments.h"
#include "sleep.h"
#include <EEPROM.h>
#if defined (__AVR__)
#include <avr/power.h>
#endif

const char *appEui = "70B3D57ED0016ADA";
const char *appKey = "5CCFA43140CDD066E05EF81E4821631D";
    
#define loraSerial Serial1
#define debugSerial Serial
#define aref_voltage 3.3
#define freqPlan TTN_FP_EU868
    
static TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
//static TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan, sf); sf = spreading factor, defaults to 7

//Global variables
//analog pins
const int tempPin = 1;
const int phPin = 2;
const int turbPin = 3;
const int condPin = 4;

//digital pins
const int condDig_1 = 5;
const int condDig_2 = 6;
const int radioSwitch = 7;
const int sensorSwitch = 8;
const int phTurbSwitch = 9;
const int radioResetPin = 4;

//measurment constants
const int phInterval = 20;
const int phOffset = 0.09; //Used for calibrating pH sensor
const int nPh = 10;
const int nCond = 10;
const int nTemp = 10;
const int nTurb = 10;

const int cyclesAddr = 0;
const int flagAddr = 1;

const int measAddr = 2;
const int flag1Addr = 3;

//number of sleep cycles, sleep time '=' nCycles*8 seconds
//remove const if implementing system to update sleep length from ttn
//number of measurment sets in each packet and number of variables (temp, pH, turb, cond, count)
byte nCycles = 1;
byte nMeasurments = 1;
byte nVariables = 4;

void initRadio();


void setup() {
  // Initialize radio
  initRadio();

  byte hasWrittenToCycles =  EEPROM.read(flagAddr); //check if the interval has been written to
  if(hasWrittenToCycles == 1){
    nCycles = EEPROM.read(cyclesAddr); //update the interval on setup
  }

  byte hasWrittenToMeas =  EEPROM.read(flag1Addr); //check if the interval has been written to
  if(hasWrittenToMeas == 1){
    nMeasurments = EEPROM.read(measAddr); //update the interval on setup
  }

  //Check for new message
  ttn.poll();
  // Wait a maximum of 5s for Serial Monitor
  while (!debugSerial && millis() < 5000);

 
  // Setup sleep
  sleepInit();

}

void initRadio(){
  pinMode(radioSwitch, OUTPUT);
  digitalWrite(radioSwitch, HIGH);
  pinMode(radioResetPin, OUTPUT);
  digitalWrite(radioResetPin, HIGH);
  ttn.wake();
  
  ttn.onMessage(message);
  
  loraSerial.begin(57600);
  debugSerial.begin(9600);
  
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 5000);
    
  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey); // Default: retries = infinite, retryDelay = 10 000 ms
}


void connectAndSend(byte* payload){
  //Send nCycles
  payload[0] = nCycles;
  initRadio();
  // Send payload, if it fails, wait and try again
  if(ttn.sendBytes(payload, nMeasurments*nVariables) == TTN_ERROR_SEND_COMMAND_FAILED){
    delay(1000);
    ttn.sendBytes(payload, nMeasurments*nVariables);
  }
  delay(1000); //Wait for message
}


void loop() {  
  byte* payload = new byte[1+nMeasurments*nVariables];
  if(payload == NULL)
  {
    //Error allocating memory
  }
  else
  {
    Measurment m;
    for(byte i = 0; i < nMeasurments; ++i)
    {
      gotoSleep(nCycles);
      //...zzzz
      goodMorning();
      
      //Measure and update payload
      m = takeMeasurment();
      updatePayload(i, payload, m);
    }
    connectAndSend(payload);
  }
  
  delete[] payload;
}


//Called when receiving message
void message(const byte* payload, int length, int port){
  debugSerial.println("--- MESSAGE");
  //
  if(length == 1){
    nCycles = payload[0];
    EEPROM.write(cyclesAddr, nCycles); //save the interval into flash memory
    EEPROM.write(flagAddr, 1); //write a 1 to indicate that the interval has changed
    return;
  }
  //Making sure nMeasurments does not become too big to send
  else if(length == 2 && payload[1] < 10){
    nCycles = payload[0];
    EEPROM.write(cyclesAddr, nCycles);//save the interval into flash memory
    EEPROM.write(flagAddr, 1); //write a 1 to indicate that the interval has changed
    
    nMeasurments = payload[1];

    EEPROM.write(measAddr, nMeasurments);//save the interval into flash memory
    EEPROM.write(flag1Addr, 1); //write a 1 to indicate that the interval has changed
  }
  
}
