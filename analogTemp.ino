#include <TheThingsNetwork.h>


const char *appEui = "70B3D57ED0016ADA";
const char *appKey = "9BA875FB5C9B81205BFE16AB99D8D22C";
    
#define loraSerial Serial1
#define debugSerial Serial
#define aref_voltage 3.3
#define freqPlan TTN_FP_EU868
    
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);


//global variables
const int tempPin = 0;        //the analog pin the TMP36's Vout (sense) pin is connected to
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures
int tempReading;        // the analog reading from the sensor



void setup() {
  loraSerial.begin(57600);
  debugSerial.begin(9600);
      
  // Initialize LED output pin
  pinMode(LED_BUILTIN, OUTPUT);
    
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
  ttn.sendBytes(tempReading, sizeof(tempReading));
      
  delay(5000); //delay 5000 ms

}
