#include <Crypt.h>

void setup() {
    Serial.begin(9600);
    for (byte i = 0; i <= 16; i++) {
        Serial.print("i = ");
        Serial.println(i);
        byte encodedNibble = cryptEncodeNibble(i);
        Serial.print("encodedNible: ");
        Serial.print(encodedNibble);
        byte decodedNibble = cryptDecodeNibble(encodedNibble);
        Serial.print(", decodedNibble: ");
        Serial.println(decodedNibble);
    }
}

void loop() {

}