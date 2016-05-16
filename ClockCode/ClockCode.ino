#include <TinyWireM.h>
#define RTC_ADDR 0x68

//location of HC595 pins
const int dataPin = 1;   
const int latchPin = 3;
const int clockPin = 4;

//global vars
byte nMin;
byte hours;

//vars to shift out
byte hOutA;
byte hOutB;
byte minOut;

//vars for switch debouncing
bool prevH;
bool currH;
bool prevM;
bool currM;

//Helper functions
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}

//function for shifting out data
void shiftOut(byte myDataOut) {
  int i=0;
  int pinState;
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, 0);
  digitalWrite(clockPin, 0);
  for (i=7; i>=0; i--)  {
    digitalWrite(clockPin, 0);
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {  
      pinState= 0;
    }
    digitalWrite(dataPin, pinState);
    digitalWrite(clockPin, 1);
    digitalWrite(dataPin, 0);
  }
  digitalWrite(clockPin, 0);
}

//Fetch the time from the RTC
void fetchTime(){
  byte wireRet = 0;
  TinyWireM.beginTransmission(RTC_ADDR);
  TinyWireM.send(1);
  wireRet = TinyWireM.endTransmission();
  wireRet = TinyWireM.requestFrom(RTC_ADDR, 2);
  nMin = bcdToDec(TinyWireM.receive());
  hours = bcdToDec(TinyWireM.receive());
}

//Write time to the RTC
void writeTime(byte hour, byte minute){
  TinyWireM.beginTransmission(RTC_ADDR);
  TinyWireM.send(0);
  TinyWireM.send(decToBcd(0));
  TinyWireM.send(decToBcd(minute));
  TinyWireM.send(decToBcd(hour));
  TinyWireM.endTransmission();
}

//which minutes to shift out
byte getMinutes(void){  
  if (nMin >= 0 && nMin <= 2)       {minOut = 0x0;}
  else if (nMin >= 3 && nMin <= 7)  {minOut = 0b00001110;}
  else if (nMin >= 8 && nMin <= 12) {minOut = 0b10000110;}
  else if (nMin >= 13 && nMin <= 18){minOut = 0b00100010;}
  else if (nMin >= 19 && nMin <= 22){minOut = 0b00010110;}
  else if (nMin >= 23 && nMin <= 27){minOut = 0b00011110;}
  else if (nMin >= 28 && nMin <= 32){minOut = 0b01000010;}
  else if (nMin >= 33 && nMin <= 37){hOutB = 0b10000000; minOut = 0b00011100;}
  else if (nMin >= 38 && nMin <= 42){hOutB = 0b10000000; minOut = 0b00010100;}
  else if (nMin >= 43 && nMin <= 47){hOutB = 0b10000000; minOut = 0b00100000;}
  else if (nMin >= 48 && nMin <= 53){hOutB = 0b10000000; minOut = 0b10000100;}
  else if (nMin >= 54 && nMin <= 57){hOutB = 0b10000000; minOut = 0b00001100;}
  else if (nMin >= 58)              {minOut = 0x0;}
}

//which hour to shift out
void getHours(void){
  if(nMin > 32){
    hours = hours+1;
  } 
  switch (hours){
    case  0:
    case 12:
    case 24:
      hOutA = 0b00000100; 
      break;
    case  1:
    case 13:
      hOutB += 0b01000000;
      break;
    case  2:
    case 14:
      hOutB += 0b00100000;
      break;
    case  3:
    case 15:
      hOutB += 0b00010000;
      break;
    case  4:
    case 16:
      hOutB += 0b00001000;      
      break;
    case  5:
    case 17:
      hOutB += 0b00000100;
      break;
    case  6:
    case 18:
      hOutB += 0b00000010;
      break;
    case  7:
    case 19:
      hOutA = 0b10000000;
      break;
    case  8:
    case 20:
      hOutA = 0b01000000;
      break;
    case  9:
    case 21:
      hOutA = 0b00100000;      
      break;
    case 10:
    case 22:
      hOutA = 0b00010000;
      break;
    case 11:
    case 23:
      hOutA = 0b00001000;
      break;
  }     
}

//logic to shift out time to LEDs
void shiftTimeOut(void){
  //clear the previous shiftings
  hOutA = 0x0;
  hOutB = 0x0;
  minOut = 0x0;
  //fetch time from RTC
  fetchTime();
  //work out what to display
  getMinutes();
  getHours();
  //add the oclock if necessary
  if(minOut==0x0){
    hOutA += 0b00000010;
  }  
  //shift out the time
  digitalWrite(latchPin, 0);
  shiftOut(minOut);
  shiftOut(hOutB);  
  shiftOut(hOutA);
  digitalWrite(latchPin, 1);
}

//only runs once - initial set up for TinyWireLib abd pin Modes
void setup() {
  TinyWireM.begin();
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

//runs continuously
void loop() {
  shiftTimeOut();

  //Check for changes in time
  currH = digitalRead(5);  
  currM = digitalRead(6);
  
  //check for switch changes
  if (prevH == 1 && currH == 0){
    fetchTime();
    hours += 1;
    if (hours >= 24){
      hours = 0;
    }    
    writeTime(hours, nMin);
  }
  prevH = currH;
  delay(20);  

if (prevM == 1 && currM == 0){
    fetchTime();
    nMin += 1;
    if (nMin >= 60){
      nMin = 0;
    }    
    writeTime(hours, nMin);
  }
  prevM = currM;
  delay(20);
  
}
