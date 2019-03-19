#ifndef MEASURMENTS_H_
#define MEASURMENTS_H_


//Struct for one set of measurments
typedef struct Measurment{
  byte temp;
  byte pH;
  byte turb;
  byte cond;  
  byte count;
}Measurment;


//Function takes measurment and
//updates global variables for measurments
Measurment takeMeasurment(const byte gCount);


void updatePayload(const int numM, byte* payload, const Measurment& m, const byte nCycles);


//Called when woken up by digital interrupt
void digitalInterrupt();

void swap(int* a, int* b);

void quicksort(int* arr, int l, int r);

int median(int* arr, int len);


byte readTemp();
void tempError();

byte readpH();
void phError();

byte readConductivity();
void condError();

byte readTurbidity();
void turbError();

#endif
