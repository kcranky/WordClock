const int latchPin = 3;
const int clockPin = 4;
const int dataPin = 1;

void setup() {
  delay(500);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  digitalWrite(latchPin, LOW);
  shiftOut(0x0);
  shiftOut(0x0);
  shiftOut(0x0);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
  shiftOut(0xFF);
  shiftOut(0xFF);
  shiftOut(0xFF);
  digitalWrite(latchPin, HIGH);
}

void loop() {

}

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

