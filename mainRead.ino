bool isSender = false;
byte nextByte = 0;

CRAP* listner = new CRAP();

void updateStatusIndicators() {}

void setup() {
    // initialize
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    if (isSender) {
      CRAP::sendMessage(0xaa, 0xaa);
    }
}

  void loop() {
    if (!isSender) {
      // waiting for incoming data
      if (Serial.available()) {
          // reading the next byte and sending it to the CRA Protocol
          nextByte = Serial.read();
          listner->processIncomingByte(nextByte);
          Serial.print("code: ");
          Serial.print(CRAP::getProtocolCode(nextByte), BIN);
          Serial.print(", data: ");
          Serial.print(CRAP::getProtocolData(nextByte), BIN);
          Serial.println();

          // check to see if the message is complete
          if (listner->isMessageComplete()) {
            unsigned short userId = listner->getUserId();
            unsigned short message = listner->getMessage();
            Serial.println(userId, BIN);
            Serial.println(message, BIN);
          }
      }
      if (listner->hasMessageFailed()) {
        digitalWrite(LED_BUILTIN, HIGH);
      } else {
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
}
