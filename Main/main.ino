#include <TheThingsNetwork.h>
#include "measurments.h"
#include "sleep.h"
#include <EEPROM.h>
/*Basert på
http://www.kevindarrah.com/download/arduino_code/LowPowerVideo.ino
samt the things uno quick start guide
*/

const char *appEui = "70B3D57ED0016ADA";
const char *appKey = "9BA875FB5C9B81205BFE16AB99D8D22C";
    
#define loraSerial Serial1
#define debugSerial Serial
#define aref_voltage 3.3
#define freqPlan TTN_FP_EU868
    
static TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

//Global variables
//analog pins
const int tempPin = 1;
const int phPin = 2;
const int turbPin = 3;
const int condPin = 4;

//digital pins
const int condDig_1 = 3;
const int condDig_2 = 4;

//measurment constants
const int phInterval = 20;
const int phOffset = 0.09; //Used for calibrating pH sensor
const int nPh = 4;
const int nCondCycles = 4;

const int cyclesAddr = 0;
const int flagAddr = 1;

const int measAddr = 2;
const int flag1Addr = 3;

//number of sleep cycles, sleep time '=' nCycles*8 seconds
//remove const if implementing system to update sleep length from ttn
//number of measurment sets in each packet and number of variables (temp, pH, turb, cond, count)
byte nCycles = 1;
byte nMeasurments = 1;
byte nVariables = 6;




void setup() {
  ttn.wake();
  
  ttn.onMessage(message);
  
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  byte hasWrittenToCycles =  EEPROM.read(flagAddr); //check if the interval has been written to
  if(hasWrittenToCycles == 1){
    nCycles = EEPROM.read(cyclesAddr); //update the interval on setup
  }

  byte hasWrittenToMeas =  EEPROM.read(flag1Addr); //check if the interval has been written to
  if(hasWrittenToMeas == 1){
    nMeasurments = EEPROM.read(measAddr); //update the interval on setup
  }
   
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);
    
  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

  sleepInit();

}

void loop() {  
  byte* payload = new byte[nMeasurments*nVariables];
  if(payload == NULL){
    //Error allocating memory
  }
  else{
    Measurment m;
    for(byte i = 0; i < nMeasurments; ++i){
      
      //ttn.sleep(nCycles*8000);
      //sleep(nCycles);
      //...zzzz
      //goodMorning();
      //ttn.wake();
      
      m = takeMeasurment(i);
      updatePayload(i, payload, m, nCycles);
    }
    
    // Send payload
    ttn.sendBytes(payload, nMeasurments*nVariables);
    delay(1000); //Wait for message
    
  }
  
  delete[] payload;
}


//Called when receiving message
void message(const byte* payload, int length, int port){
  Serial.println("--- MESSAGE");
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
