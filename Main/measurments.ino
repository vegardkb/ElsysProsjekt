#include "measurments.h"



//Function takes measurment and
//returns measurment object
Measurment takeMeasurment(const byte gCount){
  Measurment m;

  //Enable sensor switch
  digitalWrite(sensorSwitch, HIGH);
  m.temp = readTemp(); 
  //m.cond = readConductivity();
  m.cond = 0;
  digitalWrite(sensorSwitch, LOW);
  
  //Enable ph, turb switch
  digitalWrite(phTurbSwitch, HIGH);
  m.turb = readTurbidity();
  //Wait for pH to stabilize?
  m.count = gCount;
  m.pH = readpH();
  digitalWrite(phTurbSwitch, LOW);
  
  return m;
}


// Update payload with measurment data
void updatePayload(const int numM, byte* payload, const Measurment& m, const byte nCycles){
    payload[numM*nVariables] = m.temp;
    payload[numM*nVariables+1] = m.pH;
    payload[numM*nVariables+2] = m.turb;
    payload[numM*nVariables+3] = m.cond;
    payload[numM*nVariables+4] = m.count;
    payload[numM*nVariables+5] = nCycles;
}



//Called when woken up by digital interrupt
void digitalInterrupt(){
  //needed for the digital input interrupt
}


byte readTemp(){
  //Turn on supply voltage
  
  float avg = 0;
  for(int i = 0; i < 4; ++i){
    avg += analogRead(tempPin);
  }

  //Turn off supply voltage
  
  avg /= 4;
  
  //check that temperature is in range (0, ???)
  if(avg < 510){
    avg = 510;
  }
  else if(avg > 765){
    avg = 765;
  }
  avg +=0.5;
  
  byte temp = byte{static_cast<int>(avg)-510};
  
  Serial.print("Temp: ");
  Serial.println(temp);
  
  return temp;
}

void tempError(){
  //Turn on supply voltage
  
  int sensorValue = 0;
  for(int i = 0; i < 4; ++i){
    sensorValue += analogRead(tempPin);
  }

  //Turn off supply voltage
  sensorValue /= 4;

  byte payload[3];
  payload[0] = highByte(sensorValue);
  payload[1] = lowByte(sensorValue);
  payload[2] = 0;
  ttn.sendBytes(payload, 3);//Fungerer dette? static ttn??
  
}

byte readpH(){
  //Turn on supply voltage
  //Wait one minute
  
  int sensorValue = 0;
  for(int i = 0; i < nPh; i++){
    sensorValue += analogRead(phPin);
    delay(phInterval); // Wait phInterval ms
  }

  //Turn off supply voltage

  sensorValue /= nPh;

  if(sensorValue < 300){
    sensorValue = 300;
  }
  else if(sensorValue > 555){
    sensorValue = 555;
  }
  

  Serial.print("pH: ");
  Serial.println(sensorValue-300);
  
  return byte{sensorValue - 300};
}

void phError(){
  //Turn on supply voltage
  //Wait one minute
  
  int sensorValue = 0;
  for(int i = 0; i < nPh; i++){
    sensorValue += analogRead(phPin);
    delay(phInterval); // Wait phInterval ms
  }

  //Turn off supply voltage

  sensorValue /= nPh;

  byte payload[3];
  payload[0] = highByte(sensorValue);
  payload[1] = lowByte(sensorValue);
  payload[2] = 1;
  ttn.sendBytes(payload, 3);//Fungerer dette? static ttn??
}

byte readConductivity(){
  
  int sensorValue = 0;
  
  for(int i = 0; i < nCondCycles; i++){
    
    digitalWrite(condDig_1, HIGH);
    digitalWrite(condDig_2, LOW);
    delay(1);

    sensorValue += analogRead(condPin);

    digitalWrite(condDig_1, LOW);
    digitalWrite(condDig_2, HIGH);
    delay(1);

    //Dette vil vel ikke fungere???
    sensorValue += 1023 - analogRead(condPin);
  }
  sensorValue = sensorValue / (nCondCycles);

  digitalWrite(condDig_1, LOW);
  digitalWrite(condDig_2, LOW);

  Serial.print("Cond: ");
  Serial.println(map(sensorValue, 0, 1023, 0, 255));

  return map(sensorValue, 0, 1023, 0, 255);//More sophisticated solution
}

void condError(){
  int sensorValue = 0;
  
  for(int i = 0; i < nCondCycles; i++){
    
    digitalWrite(condDig_1, HIGH);
    digitalWrite(condDig_2, LOW);
    delay(1);

    sensorValue += analogRead(condPin);

    digitalWrite(condDig_1, LOW);
    digitalWrite(condDig_2, HIGH);
    delay(1);

    //Dette vil vel ikke fungere???
    sensorValue += 1023 - analogRead(condPin);
  }
  sensorValue = sensorValue / (nCondCycles);

  digitalWrite(condDig_1, LOW);
  digitalWrite(condDig_2, LOW);

  byte payload[3];
  payload[0] = highByte(sensorValue);
  payload[1] = lowByte(sensorValue);
  payload[2] = 3;
  ttn.sendBytes(payload, 3);//Fungerer dette? static ttn??
}

byte readTurbidity(){
  //Turn on supply voltage
  int sensorValue = 0;

  for(int i = 0; i < 5; ++i){
    sensorValue += analogRead(turbPin);
  }
  //Turn off supply voltage
  
  sensorValue /= 5;
  
  
  Serial.print("Turb: ");
  Serial.println(sensorValue);
  if(sensorValue < 768){
    sensorValue = 768;
  }
  Serial.println(sensorValue - 768);
  
  return byte{sensorValue - 768};
}

void turbError(){
  //Turn on supply voltage
  int sensorValue = 0;

  for(int i = 0; i < 5; ++i){
    sensorValue += analogRead(turbPin);
  }
  //Turn off supply voltage
  
  sensorValue /= 5;


  byte payload[3];
  payload[0] = highByte(sensorValue);
  payload[1] = lowByte(sensorValue);
  payload[2] = 2;
  ttn.sendBytes(payload, 3);//Fungerer dette? static ttn??
}
