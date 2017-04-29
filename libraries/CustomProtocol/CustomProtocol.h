// Custom Protocol
#include <Arduino.h>

class CustomProtocol {

    private:

        // variables for tracking protocol state
        unsigned short protocolUserId = 0;
        unsigned short protocolMessage = 0;
        int incomingByteIndex = 0;
        bool didLastMessageFail = false;
        bool isWaiting = true;

        // helper methods for the class
        static byte createByte(byte protocolCode, byte data);

        // returns a nibble from a short
        // index 0 returns the most significant nibble
        // index 3 returns the least significant nibble
        static byte getNibbleFromShort(unsigned short shortData, int index);

    public:

        // Protocol Codes
        #define INTERUPT 0xf
        #define USER_ID 0x4
        #define ENCRYPTED_MESSAGE 0x6
        #define SUCCESSFUL_ACKNOWLEDGEMENT 0x8

        // constructor
        CustomProtocol();

        // used for convert
        static bool isSuccessfulAcknowledgement(byte incomingByte);
        static void sendMessage(unsigned short userId, unsigned short message);
        static void sendCode(byte protocolCode);
        static byte getProtocolCode(byte protocolByte);
        static byte getProtocolData(byte protocolByte);
        void processIncomingByte(byte incomingByte);
        bool isMessageComplete();
        bool hasMessageFailed();
        unsigned short getUserId();
        unsigned short getMessage();
};