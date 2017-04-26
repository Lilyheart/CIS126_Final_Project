// Communication Relay Arduino Protocol
#include <Arduino.h>

class CRAP {

    private:

        // variables for tracking protocol state
        unsigned short crapUserId = 0;
        unsigned short crapMessage = 0;
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

        // CRAP Codes
        #define USER_ID 0x4
        #define ENCRYPTED_MESSAGE 0x6
        #define SUCCESSFUL_ACKNOWLEDGEMENT 0x8
        #define FAILED_ACKNOWLEDGEMENT 0x9
        #define INVALID_USER_ID 0xa

        // constructor
        CRAP();

        // used for convert
        static void sendMessage(unsigned short userId, unsigned short message);
        static void sendResponse(byte crapCode);
        static byte getProtocolCode(byte protocolByte);
        static byte getProtocolData(byte protocolByte);
        void processIncomingByte(byte incomingByte);
        bool isMessageComplete();
        bool hasMessageFailed();
        unsigned short getUserId();
        unsigned short getMessage();
};
