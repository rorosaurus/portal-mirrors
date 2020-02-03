/*
  Simple example for receiving
  
  https://github.com/sui77/rc-switch/
*/
#define LED_PIN 1
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
  mySwitch.enableReceive(2);
//  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
   
  if (mySwitch.available()) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    Serial.print("Received: ");
    Serial.println( mySwitch.getReceivedValue() );
//    Serial.print(" / ");
//    Serial.print( mySwitch.getReceivedBitlength() );
//    Serial.print("bit ");
//    Serial.print("Protocol: ");
//    Serial.println( mySwitch.getReceivedProtocol() );

    mySwitch.resetAvailable();
  }
}
