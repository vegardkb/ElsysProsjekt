#include <TheThingsNetwork.h>

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
    
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);


//Global variables
//analog pins
const int tempPin = 1;
const int phPin = 2;
const int turbPin = 3;
const int condPin = 4;

//digital pins
const int condDig_1 = 3;
const int condDig_2 = 4;

//
const int phInterval = 20;
const int phOffset = 0; //Used for calibrating pH sensor
const int nPh = 4;
const int nCondCycles = 4;


//bytes for keeping track of measurments
byte temp;  // Temperature
byte pH;    // pH
byte turb;  // Turbidity
byte cond;  // Conductivity
byte count; // if several sets of measurments are sent
            // in each packet this will be used for
            // calculating a rough time stamp

//array for the measurments
byte payload[5]{temp, pH, turb, cond, count};

//number of sleep cycles, sleep time '=' nCycles*8 seconds
//remove const if implementing system to update sleep length from ttn
int nCycles = 1;

//number of measurment sets in each packet
int nMeasurments = 1;


void setup() {
  ttn.onMessage(message);
  
  loraSerial.begin(57600);
  debugSerial.begin(9600);
      
    
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);
    
  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);
}

void loop() {  
  
  count = 0;
  //Enable ADC before measuring
  //ADCSRA |= (1 << 7);
  
  takeMeasurment(payload[0], payload[1], payload[2], payload[3], payload[4]);

  //Disable ADC after measuring
  //ADCSRA &= ~(1 << 7);
  
  
  // Send payload
  ttn.sendBytes(payload, sizeof(payload));
  delay(5000); //Wait for message

}

ISR(WDT_vect){
  //DON'T FORGET THIS!  Needed for the watch dog timer.  This is called after a watch dog timer timeout - this is the interrupt function called after waking up
}// watchdog interrupt

//Called when receiving message
void message(const byte* payload, int length, int port){
  Serial.println("--- MESSAGE");
  //
  if(length == 1){
    nCycles = payload[0];
    return;
  }
  //Making sure nMeasurments does not become too big to send
  else if(length == 2 && payload[1] < 10){
    nCycles = payload[0];
    nMeasurments = payload[1];
  }
  
}
