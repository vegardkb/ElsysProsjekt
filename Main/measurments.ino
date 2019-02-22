#include "measurments.h"



//Function takes measurment and
//updates global variables for measurments
Measurment takeMeasurment(const byte gCount){
  Measurment m;
  m.temp = readTemp(); 
  delay(500);
  m.cond = readConductivity();
  delay(500);
  //analogReference(EXTERNAL) 5V for turbiditet og pH
  m.turb = readTurbidity();
  delay(500);
  m.count = gCount;
  m.pH = readpH();
  return m;
}


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
  float avg = 0;
  for(int i = 0; i < 4; ++i){
    avg += analogRead(tempPin);
  }
  avg /= 4;
  
  //check that temperature is in range (0, ???)
  if(avg < 450 || avg > 705){
    //Handle this case
  }
  avg +=0.5;
  
  byte temp = byte{static_cast<int>(avg)-450};
  /*
  Serial.print("Temp: ");
  Serial.println(temp);
  */
  return temp;
}

byte readpH(){
  int average = 0;
  for(int i = 0; i < nPh; i++){
    average += analogRead(phPin);
    delay(phInterval); // Wait phInterval ms
  }

  average /= nPh;

  if(average < 300){
    //Handle case if pH less than 5.13
  }
  if(average > 555){
    //Handle case if pH greater than 9.48
  }
/*
  Serial.print("pH: ");
  Serial.println(average-300);
  */
  return byte{average - 300};
}

byte readConductivity(){
  
  int reading = 0;
  
  for(int i = 0; i < nCondCycles; i++){
    
    digitalWrite(condDig_1, HIGH);
    digitalWrite(condDig_2, LOW);
    delay(1);

    reading += analogRead(condPin);

    digitalWrite(condDig_1, LOW);
    digitalWrite(condDig_2, HIGH);
    delay(1);

    //Dette vil vel ikke fungere???
    //reading += 1023 - analogRead(condPin);
  }
  reading = reading / (nCondCycles);

  digitalWrite(condDig_1, LOW);
  digitalWrite(condDig_2, LOW);
/*
  Serial.print("Cond: ");
  Serial.println(map(reading, 0, 1023, 0, 255));
*/
  return map(reading, 0, 1023, 0, 255);

}

byte readTurbidity(){
  
  int sensorValue = 0;

  for(int i = 0; i < 5; ++i){
    sensorValue += analogRead(turbPin);
  }
  sensorValue /= 5;
  
  
  Serial.print("Turb: ");
  Serial.println(sensorValue);
  if(sensorValue < 768){
    //dostuff
  }
  Serial.println(sensorValue - 768);
  
  return byte{sensorValue - 768};
}
