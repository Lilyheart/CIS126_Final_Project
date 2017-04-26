#include "CRAP.h"
#include <Arduino.h>

// constructor
CRAP::CRAP() {
    // initializing instance properties
    crapUserId = 0;
    crapMessage = 0;
    incomingByteIndex = 0;
    didLastMessageFail = false;
    isWaiting = true;
}

// helper function for formatting nibbles into bytes
byte CRAP::createByte(byte protocolCode, byte data) {
    return (protocolCode << 4) | data;
}

// returns a nibble from a short
// index 0 returns the most significant nibble
// index 3 returns the least significant nibble
byte CRAP::getNibbleFromShort(unsigned short shortData, int index) {
    int position = (3 - index) * 4;
    int mask = 0b1111 << position;
    return (shortData & mask) >> position;
}

// sends a full message
void CRAP::sendMessage(unsigned short userId, unsigned short message) {
    for (int i = 0; i < 4; i++) {
        byte data = createByte(USER_ID, getNibbleFromShort(userId, i));
        Serial.write(data);
    }
    for (int i = 0; i < 4; i++) {
        byte data = createByte(ENCRYPTED_MESSAGE, getNibbleFromShort(message, i));
        Serial.write(data);
    }
}

// sends a response
void CRAP::sendResponse(byte protocolCode) {
    Serial.write(createByte(protocolCode, 0));
}

// helper function for reading the code off an incoming byte
byte CRAP::getProtocolCode(byte protocolByte) {
    return (protocolByte >> 4) & 0b1111;
}

// helper function for reading the data off an incoming byte
byte CRAP::getProtocolData(byte protocolByte) {
    return protocolByte & 0b1111;
}

// processes incoming byte for the receiver of incoming messages
void CRAP::processIncomingByte(byte incomingByte) {
    byte code = getProtocolCode(incomingByte);
    byte data = getProtocolData(incomingByte);
    if (0 <= incomingByteIndex && incomingByteIndex <= 3 && code == USER_ID) {
        // successful incoming user id
        didLastMessageFail = false;
        isWaiting = true;
        int userIdPosition = (3 - incomingByteIndex) * 4;
        crapUserId = crapUserId | (data << userIdPosition);
        incomingByteIndex++;
    } else if (4 <= incomingByteIndex && incomingByteIndex <= 7 && code == ENCRYPTED_MESSAGE) {
        // successful incoming message
        didLastMessageFail = false;
        isWaiting = true;
        int messagePositon = (7 - incomingByteIndex) * 4;
        crapMessage = crapMessage | (data << messagePositon);
        incomingByteIndex++;
        if (incomingByteIndex == 8) {
            // message complete
            incomingByteIndex = 0;
            isWaiting = false;
        }
    } else {
        // invalid data, reset protocol and signal that the message failed
        crapUserId = 0;
        crapMessage = 0;
        incomingByteIndex = 0;
        didLastMessageFail = true;
    }
}

// functions for reading protocol state
bool CRAP::isMessageComplete() { return !isWaiting; }
bool CRAP::hasMessageFailed() { return didLastMessageFail; }
unsigned short CRAP::getUserId() { return crapUserId; }
unsigned short CRAP::getMessage() { return crapMessage; }
