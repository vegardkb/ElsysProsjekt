#include <TheThingsNetwork.h>
#include <OneWire.h>

const char *appEui = "70B3D57ED0016ADA";
const char *appKey = "9BA875FB5C9B81205BFE16AB99D8D22C";
    
#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868
    
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
OneWire ds(10); //on pin 10
    
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
}

void loop() {
  //TempSensor
  byte i, present = 0;
  byte type_s;
  byte tdata[12];
  byte addr[8];
  float celsius;

  if(!ds.search(addr)){
    debugSerial.println("No more addresses.\n");
    ds.reset_search();
    delay(250);
    return;
  }

  debugSerial.print("ROM =");
  for(i = 0; i < 8; i++){
    debugSerial.write(' ');
    debugSerial.print(addr[i], HEX);
  }

  if( OneWire::crc8(addr, 7) != addr[7]){
    debugSerial.println("CRC is not valid!");
    return;
  }
  debugSerial.println();


  switch(addr[0]){
    case 0x28:
      debugSerial.println(" Chip = DS18B20");
      type_s = 0;
      break;
    default:
      debugSerial.println("Device is not a DS18B20 device.");
      return;   
  }
  

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  debugSerial.print("  Data = ");
  debugSerial.print(present, HEX);
  debugSerial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    tdata[i] = ds.read();
    debugSerial.print(tdata[i], HEX);
    debugSerial.print(" ");
  }
  debugSerial.print(" CRC=");
  debugSerial.print(OneWire::crc8(tdata, 8), HEX);
  debugSerial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (tdata[1] << 8) | tdata[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (tdata[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - tdata[6];
    }
  } else {
    byte cfg = (tdata[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  debugSerial.print("  Temperature = ");
  debugSerial.print(celsius);
  debugSerial.print(" Celsius, ");
  
  
  debugSerial.println("-- LOOP");
    
  // Prepare array of 1 byte to indicate LED status
  byte data[1];
  data[0] = (digitalRead(LED_BUILTIN) == HIGH) ? 1 : 0;
    
  // Send it off
  ttn.sendBytes(data, sizeof(data));
      
  delay(5000);

}
