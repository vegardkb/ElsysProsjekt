


//Struct for one set of measurments
struct measurment{
  byte count;
  byte temp;
  byte cond;
  byte turb;
  byte pH;
};



//Function takes measurment and
//updates global variables for measurments
void takeMeasurment(byte& temp, byte& pH, byte& turbidity, byte& conductivity, byte& count){
  temp = readTemp(); 
  delay(500);
  turbidity = readTurbidity();
  delay(500);
  conductivity = readConductivity();
  delay(500);
  count++;
  pH = readpH();
}

//Called when woken up by digital interrupt
void digitalInterrupt(){
  //needed for the digital input interrupt
}


byte readTemp(){
  int tempReading = analogRead(tempPin);
  
  //check that temperature is in range (-2, 30)
  if(tempReading < 148 || tempReading > 248){
    //Handle this case
  }

  Serial.print("Temp: ");
  Serial.println(tempReading);

  
  tempReading = tempReading % 148;
  byte temp ={tempReading};
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

  Serial.print("pH: ");
  Serial.println(average);
  
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

    reading += 1023 - analogRead(condPin);
  }
  reading = reading / (2*nCondCycles);

  digitalWrite(condDig_1, LOW);
  digitalWrite(condDig_2, LOW);

  Serial.print("Cond: ");
  Serial.println(reading);

  return map(reading, 0, 1023, 0, 255);

}

byte readTurbidity(){

  int sensorValue = analogRead(turbPin);

  Serial.print("Turb: ");
  Serial.println(sensorValue);
  
  return byte{map(sensorValue, 0, 1023, 0, 255)};
}

//Setup for sleeping
void sleepInit(){
  
  //Save Power by writing all Digital IO LOW - note that pins just need to be tied one way or another, do not damage devices!
  //Use exceptions for pins which need to be used for measurments or other stuff
  for (int i = 0; i < 14; i++) {
    if(i != 2)
      pinMode(i, OUTPUT);
  }

  attachInterrupt(0, digitalInterrupt, FALLING); //interrupt for waking up, attach button and stuff

  //SETUP WATCHDOG TIMER
  WDTCSR = (24);//change enable and WDE - also resets
  WDTCSR = (33);//prescalers only - get rid of the WDE and WDCE bit
  WDTCSR |= (1<<6);//enable interrupt mode

  //Disable ADC - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
  ADCSRA &= ~(1 << 7);

  //ENABLE SLEEP - this enables the sleep mode
  SMCR |= (1 << 2); //power down mode
  SMCR |= 1;//enable sleep
}


void sleep(int nCycles){
  // sleep for nCycles*8 sec
  for(int i = 0; i < nCycles; i++){
    //delay();
    
    //BOD DISABLE - this must be called right before the __asm__ sleep instruction
    MCUCR |= (3 << 5);                      //set both BODS and BODSE at the same time
    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
    __asm__  __volatile__("sleep");         //in line assembler to go to sleep
  }
}
