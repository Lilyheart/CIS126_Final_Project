#include "CustomProtocol.h"
#include <Arduino.h>

// constructor
CustomProtocol::CustomProtocol() {
    // initializing instance properties
    protocolUserId = 0;
    protocolMessage = 0;
    incomingByteIndex = 0;
    didLastMessageFail = false;
    isWaiting = true;
}

// helper function for formatting nibbles into bytes
byte CustomProtocol::createByte(byte protocolCode, byte data) {
    return (protocolCode << 4) | data;
}

// returns a nibble from a short
// index 0 returns the most significant nibble
// index 3 returns the least significant nibble
byte CustomProtocol::getNibbleFromShort(unsigned short shortData, int index) {
    int position = (3 - index) * 4;
    int mask = 0b1111 << position;
    return (shortData & mask) >> position;
}

// checks if the incoming byte was for a successful acknowledgement
bool CustomProtocol::isSuccessfulAcknowledgement(byte incomingByte) {
    return incomingByte == createByte(SUCCESSFUL_ACKNOWLEDGEMENT, 0);
}

// sends a full message
void CustomProtocol::sendMessage(unsigned short userId, unsigned short message) {
    // sending an interupt to make sure the receiver resets
    sendCode(INTERUPT);
    for (int i = 0; i < 4; i++) {
        byte data = createByte(USER_ID, getNibbleFromShort(userId, i));
        Serial.write(data);
    }
    for (int i = 0; i < 4; i++) {
        byte data = createByte(ENCRYPTED_MESSAGE, getNibbleFromShort(message, i));
        Serial.write(data);
    }
    delay(50); // delay for serial line to clear
}

// sends a code
void CustomProtocol::sendCode(byte protocolCode) {
    Serial.write(createByte(protocolCode, 0));
}

// helper function for reading the code off an incoming byte
byte CustomProtocol::getProtocolCode(byte protocolByte) {
    return (protocolByte >> 4) & 0b1111;
}

// helper function for reading the data off an incoming byte
byte CustomProtocol::getProtocolData(byte protocolByte) {
    return protocolByte & 0b1111;
}

// processes incoming byte for the receiver of incoming messages
void CustomProtocol::processIncomingByte(byte incomingByte) {
    byte code = getProtocolCode(incomingByte);
    byte data = getProtocolData(incomingByte);
    if (0 <= incomingByteIndex && incomingByteIndex <= 3 && code == USER_ID) {
        // successful incoming user id
        didLastMessageFail = false;
        isWaiting = true;
        int userIdPosition = (3 - incomingByteIndex) * 4;
        protocolUserId = protocolUserId | (data << userIdPosition);
        incomingByteIndex++;
    } else if (4 <= incomingByteIndex && incomingByteIndex <= 7 && code == ENCRYPTED_MESSAGE) {
        // successful incoming message
        didLastMessageFail = false;
        isWaiting = true;
        int messagePositon = (7 - incomingByteIndex) * 4;
        protocolMessage = protocolMessage | (data << messagePositon);
        incomingByteIndex++;
        if (incomingByteIndex == 8) {
            // message complete
            incomingByteIndex = 0;
            isWaiting = false;
        }
    } else {
        // invalid data, reset protocol and signal that the message failed
        protocolUserId = 0;
        protocolMessage = 0;
        incomingByteIndex = 0;
        didLastMessageFail = true;
    }
}

// functions for reading protocol state
bool CustomProtocol::isMessageComplete() { return !isWaiting; }
bool CustomProtocol::hasMessageFailed() { return didLastMessageFail; }
unsigned short CustomProtocol::getUserId() { return protocolUserId; }
unsigned short CustomProtocol::getMessage() { return protocolMessage; }