#include <Crypt.h>
#include <CustomProtocol.h>

#define DEBUG_MODE false

#define STATUS_SUCCESS 11 // green led
#define STATUS_WAITING 12 // yellow led
#define STATUS_FAILED 13 // red led

#define STATE_SENDING 1
#define STATE_WAITING 2

#define WAIT_TIME 1000 // time to wait for response
#define DISPLAY_TIME 500 // time to show status indicators

int currentState;
unsigned long sendTime;

unsigned short dataToSend = 0;

// helper function for encoding the message
unsigned short encryptMessage(unsigned short message) {
    for (int position = 0; position < 16; position += 4) {
        unsigned short mask = 0xf << position;
        byte oldNibble = (message & mask) >> position;
        byte newNibble = cryptEncodeNibble(oldNibble);

        if (DEBUG_MODE) {
            Serial.print("converting ");
            Serial.print(oldNibble, HEX);
            Serial.print(" to ");
            Serial.println(newNibble, HEX);
        }

        message = (message & (~mask)) | (newNibble << position);
    }
    return message;
}

// send an encrypted message and rotates through tests
void sendMessage() {
    if (DEBUG_MODE) {
        Serial.print(dataToSend, HEX);
        Serial.print(" = ");
        Serial.println(encryptMessage(dataToSend), HEX);
    }
    CustomProtocol::sendMessage(dataToSend, encryptMessage(dataToSend));
    if (dataToSend == 0xffff) { // max value
        dataToSend = 0;
    } else {
        dataToSend += 0x1111;
    }
}

// helper function for updating status indicators
void updateStatusIndicators(int status) {
    digitalWrite(STATUS_SUCCESS, LOW);
    digitalWrite(STATUS_WAITING, LOW);
    digitalWrite(STATUS_FAILED, LOW);
    digitalWrite(status, HIGH);
}

void setup() {
    // initialize pins and serial connection
    Serial.begin(9600);
    pinMode(STATUS_SUCCESS, OUTPUT);
    pinMode(STATUS_WAITING, OUTPUT);
    pinMode(STATUS_FAILED, OUTPUT);
    updateStatusIndicators(STATUS_FAILED);
    currentState = STATE_SENDING;
}

void loop() {
    switch (currentState) {
        case STATE_SENDING:
            sendMessage();
            sendTime = millis();
            currentState = STATE_WAITING;
            updateStatusIndicators(STATUS_WAITING);
            break;
        case STATE_WAITING:
            // response took too long, more than 5 seconds
            if ((millis() - sendTime) > WAIT_TIME) {
                currentState = STATE_SENDING;
                updateStatusIndicators(STATUS_FAILED);
                delay(DISPLAY_TIME); // wait 3 seconds
            }

            // waiting for response
            if (Serial.available()) {
                // reading incoming bytes
                byte nextByte = Serial.read();
                if (CustomProtocol::isSuccessfulAcknowledgement(nextByte)) {
                    currentState = STATE_SENDING;
                    updateStatusIndicators(STATUS_SUCCESS);
                    delay(DISPLAY_TIME);
                }
            }
            break;
        default:
            break;
    }

}