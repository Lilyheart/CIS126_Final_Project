// CRAP Codes
#define USER_ID 0b0110
#define ENCRYPTED_MESSAGE 0b0100
#define SUCCESSFUL_ACKNOWLEDGEMENT 0b0111
#define FAILED_ACKNOWLEDGEMENT 0b1111
#define INVALID_USER_ID 0b0101

// variables for tracking protocol state
uint16_t crapUserId = 0;
uint16_t crapMessage = 0;
int incomingByteIndex = 0;
bool didLastMessageFail = false;
bool isWaiting = true;

// helper function for creating a byte for the protocol
uint8_t createByte(uint8_t protocolCode, uint8_t data) {
    uint8_t byte = (protocolCode << 4) | data;
    return byte;
}

// converts a user id and message into something that can be sent with a serial communication
/*uint8_t* convertMessage(uint16_t userId, uint16_t message) {
    uint8_t[8] convertedMessage = new uint8_t [8];
    // converting the userId into in the protocol
    for (int i = 0; i < 4; i++) {
        int userIdPositon = (3 - i) * 4;
        uint16_t userIdMask = 0b1111 << userIdPosition;
        uint8_t data = (userId & userIdMask) >> userIdPosition;
        convertedMessage[i] = createByte(USER_ID, data);
    }
    // converting the message into in the protocol
    for (int i = 0; i < 4; i++) {
        int messagePositon = (3 - i) * 4;
        uint16_t messageMask = 0b1111 << messagePosition;
        uint8_t data = (message & messageMask) >> messagePosition;
        convertedMessage[i+4] = createByte(ENCRYPTED_MESSAGE, data);
    }
    return convertedMessage;
}*/

// helper function for reading the code off an incoming byte
uint8_t getProtocolCode(uint8_t protocolByte) {
    return (protocolByte >> 4) & 0b1111;
}

// helper function for reading the data off an incoming byte
uint8_t getProtocolData(uint8_t protocolByte) {
    return protocolByte & 0b1111;
}

// processes incoming byte for the receiver of incoming messages
void processIncomingByte(uint8_t incomingByte) {
    uint8_t code = getProtocolCode(incomingByte);
    uint8_t data = getProtocolData(incomingByte);
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
bool isMessageComplete() {
    return !isWaiting;
}

bool hasMessageFailed() {
    return didLastMessageFail;
}

uint16_t getUserId() {
    return crapUserId;
}

uint16_t getMessage() {
    return crapMessage;
}

uint8_t nextByte = 0;

void updateStatusIndicators() {}

void setup() {
    // initialize
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

  void loop() {
    // waiting for incoming data
    if (Serial.available()) {
        // reading the next byte and sending it to the CRA Protocol
        nextByte = Serial.read();
        processIncomingByte(nextByte);
        Serial.print("code: ");
        Serial.print(getProtocolCode(nextByte), BIN);
        Serial.print(", data: ");
        Serial.print(getProtocolData(nextByte), BIN);
        Serial.println();

        // check to see if the message is complete
        if (isMessageComplete()) {
          uint16_t userId = getUserId();
          uint16_t message = getMessage();
          Serial.println(userId, BIN);
          Serial.println(message, BIN);
        }
    }
    if (hasMessageFailed()) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
}
