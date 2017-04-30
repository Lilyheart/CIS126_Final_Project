#include <LiquidCrystal.h>
#include <Crypt.h>
#include <CustomProtocol.h>

#define DEBUG_MODE false

#define STATUS_SUCCESS 11 // green led
#define STATUS_WAITING 12 // yellow led
#define STATUS_FAILED 13 // red led

#define DISPLAY_TIME 5000 // time to show status indicators

LiquidCrystal lcd(2, 3, 4, 5, 6 , 7);
CustomProtocol* listner = new CustomProtocol();

byte nextByte = 0;
unsigned short lastSuccessfulUserId = 0;
unsigned short lastSuccessfulMessage = 0;
unsigned long lastSuccessfulMessageTime;

// helper function for decoding the message
unsigned short decodeMessage(unsigned short encodedMessage) {
    Serial.println(encodedMessage, HEX);
    for (int position = 0; position < 16; position += 4) {
        unsigned short mask = 0xf << position;
        byte oldNibble = (encodedMessage & mask) >> position;
        byte newNibble = cryptDecodeNibble(oldNibble);

        if (DEBUG_MODE) {
            Serial.print("converting ");
            Serial.print(oldNibble, HEX);
            Serial.print(" to ");
            Serial.println(newNibble, HEX);
        }

        encodedMessage = (encodedMessage & (~mask)) | (newNibble << position);
    }
    return encodedMessage;
}

// helper function for updating status indicators
void updateStatusIndicators(int status) {
    digitalWrite(STATUS_SUCCESS, LOW);
    digitalWrite(STATUS_WAITING, LOW);
    digitalWrite(STATUS_FAILED, LOW);
    digitalWrite(status, HIGH);
}

// helper function for writing to the lcd screen
void writeMessageToLCD(String topRowMessage, String bottomRowMessage) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(topRowMessage);
    lcd.setCursor(0,1);
    lcd.print(bottomRowMessage);
}

// helper function for converting a short to a hex string
String getShortHexString(unsigned short hexNumber) {
    String output = String(hexNumber, HEX);
    while (output.length() < 4) { output = "0" + output; }
    return output;
}

void setup() {
    // initialize pins and serial connection
    Serial.begin(9600);
    pinMode(STATUS_SUCCESS, OUTPUT);
    pinMode(STATUS_WAITING, OUTPUT);
    pinMode(STATUS_FAILED, OUTPUT);

    // initialize lcd screen
    lcd.begin(16, 2);
    writeMessageToLCD("Waiting...", "");

    if (DEBUG_MODE) { Serial.println("Starting"); }
}

void loop() {
    // waiting for incoming data
    if (Serial.available()) {
        // reading the next byte and sending it to the CRA Protocol
        nextByte = Serial.read();
        listner->processIncomingByte(nextByte);

        if (DEBUG_MODE) {
            Serial.print("Incoming Byte: 0x");
            Serial.println(nextByte, HEX);
        }

        // check to see if the message is complete
        if (listner->isMessageComplete()) {
            updateStatusIndicators(STATUS_SUCCESS);

            // send back a response that the message was successful
            delay(100); // delay for serial line to clear

            // saving the last successful message
            lastSuccessfulUserId = listner->getUserId();
            lastSuccessfulMessage = listner->getMessage();
            lastSuccessfulMessage = decodeMessage(lastSuccessfulMessage);

            if (DEBUG_MODE) {
                Serial.println("Message Received...");
                Serial.print("UserId: 0x");
                Serial.println(lastSuccessfulUserId, HEX);
                Serial.print("Message: 0x");
                Serial.println(lastSuccessfulMessage, HEX);
            }

            // writing results to the lcd screen
            writeMessageToLCD("UserId: 0x" + getShortHexString(lastSuccessfulUserId),
                "Message: 0x" + getShortHexString(lastSuccessfulMessage));

            // updating status indicators
            CustomProtocol::sendCode(SUCCESSFUL_ACKNOWLEDGEMENT);
            lastSuccessfulMessageTime = millis();
        }
    }
    bool hasReachedDisplayTimeLimit = (millis() - lastSuccessfulMessageTime) > DISPLAY_TIME;
    if (listner->hasMessageFailed()) {
        updateStatusIndicators(STATUS_FAILED);
    } else if (!(listner->isMessageComplete()) || hasReachedDisplayTimeLimit) {
        updateStatusIndicators(STATUS_WAITING);
    }
}