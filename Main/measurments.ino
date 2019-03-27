#include "measurments.h"



//Function takes measurment and
//returns measurment object
Measurment takeMeasurment(const byte gCount){
  Measurment m;

  //Enable sensor switch
  digitalWrite(sensorSwitch, HIGH);
  delay(500);
  m.temp = readTemp(); 
  m.cond = readConductivity();
  digitalWrite(sensorSwitch, LOW);
  
  //Enable ph, turb switch
  //digitalWrite(phTurbSwitch, HIGH);
  m.turb = readTurbidity();
  //Wait for pH to stabilize? Need more sophisticated solution
  //delay(60000);
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

void swap(int* a, int* b){
  int temp = *a;
  *a = *b;
  *b = temp;
}

void quicksort(int* arr, int l, int r){
  if (l >= r)
    return;
  
  int pivot = arr[r];
  int cnt = l;

  for(int i = l; i <= r; ++i){
    if(arr[i] <= pivot){
      swap(&arr[cnt], &arr[i]);
    }
    ++cnt;
  }

  quicksort(arr, l, cnt-2);
  quicksort(arr, cnt, r);
}

// Assumes sorted array
int median(int* arr, int len){
  int i = len/2;
  if(len%2){
    return (arr[i] + arr[i+1])/2;
  }
  else
    return arr[i+1];
}


byte readTemp(){
  // Allocate memory
  int* values = new int[nTemp];
  
  for(int i = 0; i < nTemp; ++i){
    values[i] = analogRead(tempPin);
  }

  quicksort(values, 0, nTemp-1);
  int med = median(values, nTemp)/2;

  // Release memory
  delete[] values;
  
  //check that temperature is in range (0, ???)
  if(med < 300){
    med = 300;
  }
  else if(med > 555){
    med = 555;
  }
  
  byte temp = byte{(med-300) % 256};
  
  Serial.print("Temp: ");
  Serial.println(temp);
  
  return temp;
}

byte readpH(){
  int* values = new int[nPh];
  for(int i = 0; i < nPh; i++){
    values[i] = analogRead(phPin);
    delay(phInterval); // Wait phInterval ms
  }

  quicksort(values, 0, nPh-1);
  int med = median(values, nPh);

  delete[] values;
  
  if(med < 300){
    med = 300;
  }
  else if(med > 555){
    med = 555;
  }
  

  Serial.print("pH: ");
  Serial.println(med-300);
  
  return byte{(med - 300) % 256};
}

// Maybe use temperature as input for more accuracy
byte readConductivity(){
  int* values = new int[nCond];
  
  for(int i = 0; i < nCond; ++i){
    values[i] = analogRead(condPin);
  }
  
  quicksort(values, 0, nCond-1);
  int med = median(values, nCond);
  
  delete[] values;

  if(med > 255*2){
    med = 255*2;
  }

  return byte{(med/2)%256};
}

byte readTurbidity(){
  int* values = new int[nTurb];

  for(int i = 0; i < nTurb; ++i){
    values[i] = analogRead(turbPin);
  }
  
  quicksort(values, 0, nTurb-1);
  int med = median(values, nTurb);
  
  delete[] values;
  
  Serial.print("Turb: ");
  Serial.println(med);
  if(med < 768){
    med = 768;
  }
  Serial.println(med - 768);
  
  return byte{(med - 768) % 256};
}
