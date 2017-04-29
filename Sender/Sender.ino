#include <CustomProtocol.h>

void send() {
  CustomProtocol::sendMessage(0xffff, 0xffff);
}

byte nextByte = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  send();
}

void loop() {
  if (Serial.available()) {
    nextByte = Serial.read();
    if (CustomProtocol::isSuccessfulAcknowledgement(nextByte)) {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}
