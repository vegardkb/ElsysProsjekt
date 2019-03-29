#include "measurments.h"



//Function takes measurment and
//returns measurment object
Measurment takeMeasurment(){
  Measurment m;

  //Enable sensor switch
  digitalWrite(sensorSwitch, HIGH);
  delay(500);
  m.temp = readTemp(100); 
  m.cond = readConductivity(0);
  digitalWrite(sensorSwitch, LOW);
  
  //Enable ph, turb switch
  //digitalWrite(phTurbSwitch, HIGH);
  m.turb = readTurbidity(768);
  //Wait for pH to stabilize? Need more sophisticated solution
  //delay(60000);
  m.pH = readpH(300);
  digitalWrite(phTurbSwitch, LOW);
  
  return m;
}


// Update payload with measurment data
void updatePayload(const int numM, byte* payload, const Measurment& m){
  int base = 1+numM*nVariables;
  payload[base] = m.temp;
  payload[base+1] = m.pH;
  payload[base+2] = m.turb;
  payload[base+3] = m.cond;
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


byte readTemp(int aMin){
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
  if(med < aMin){
    med = aMin;
  }
  else if(med > aMin+255){
    med = aMin + 255;
  }
  
  byte temp = byte{(med-aMin) % 256};
  
  Serial.print("Temp: ");
  Serial.println(temp);
  
  return temp;
}

byte readpH(int aMin){
  int* values = new int[nPh];
  for(int i = 0; i < nPh; i++){
    values[i] = analogRead(phPin);
    delay(phInterval); // Wait phInterval ms
  }

  quicksort(values, 0, nPh-1);
  int med = median(values, nPh);

  delete[] values;
  
  if(med < aMin){
    med = aMin;
  }
  else if(med > aMin+255){
    med = aMin+255;
  }
  

  Serial.print("pH: ");
  Serial.println(med-aMin);
  
  return byte{(med - aMin) % 256};
}

// Maybe use temperature as input for more accuracy
byte readConductivity(int aMin){
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

byte readTurbidity(int aMin){
  int* values = new int[nTurb];

  for(int i = 0; i < nTurb; ++i){
    values[i] = analogRead(turbPin);
  }
  
  quicksort(values, 0, nTurb-1);
  int med = median(values, nTurb);
  
  delete[] values;
  
  Serial.print("Turb: ");
  Serial.println(med);
  if(med < aMin){
    med = aMin;
  }
  Serial.println(med - aMin);
  
  return byte{(med - aMin) % 256};
}
