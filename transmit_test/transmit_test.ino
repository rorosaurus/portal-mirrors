/*
  Example for different sending methods
  
  https://github.com/sui77/rc-switch/
  
*/
#define transmitPin 2
#define buttonPin 0 // connected between pin 1 and 3.3V
#define ledPin 1

bool oldButtonState = LOW;

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {

//  Serial.begin(9600);
  
  // Transmitter is connected to Arduino Pin #2
  mySwitch.enableTransmit(transmitPin);
  
  // Optional set protocol (default is 1, will work for most outlets)
  // mySwitch.setProtocol(2);

  // Optional set pulse length.
  // mySwitch.setPulseLength(320);
  
  // Optional set number of transmission repetitions.
//   mySwitch.setRepeatTransmit(15);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // read the state of the pushbutton value:
  bool buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && oldButtonState == HIGH) {
    mySwitch.send(5393, 24);
    
    digitalWrite(ledPin, HIGH); 
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH); 
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

  oldButtonState = buttonState;

//  /* See Example: TypeA_WithDIPSwitches */
//  mySwitch.switchOn("11111", "00010");
//  delay(1000);
//  mySwitch.switchOff("11111", "00010");
//  delay(1000);
//
//  /* Same switch as above, but using decimal code */
//  mySwitch.send(5393, 24);
//  delay(1000);  
//  mySwitch.send(5396, 24);
//  delay(1000);  
//
//  /* Same switch as above, but using binary code */
//  mySwitch.send("000000000001010100010001");
//  delay(1000);  
//  mySwitch.send("000000000001010100010100");
//  delay(1000);
//
//  /* Same switch as above, but tri-state code */ 
//  mySwitch.sendTriState("00000FFF0F0F");
//  delay(1000);  
//  mySwitch.sendTriState("00000FFF0FF0");
//  delay(1000);

//  delay(20000);
}
