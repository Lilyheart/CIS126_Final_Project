#include <Wire.h>
#include "Adafruit_Trellis.h"
#include <Crypt.h>
#include <CustomProtocol.h>

#define STATE_UID 0
#define STATE_MSG 1
#define STATE_ACK 2

#define DEBUG_MODE false
#define WAIT_TIME 1000 // time to wait for response

Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0);

unsigned short matrixValue;
uint8_t matrixNibbles[4];

uint8_t writeArduinoState;

boolean isSubmitButtonPressed;
uint8_t buttonState = 0;
uint8_t lastButtonState;
const uint8_t buttonledPin = 10;
const uint8_t LED_UID = 11;
const uint8_t LED_MSG = 12;
const uint8_t LED_ERR = 13;
const uint8_t bigButtonPin = 8;

uint16_t validUserIDs[3] = {4680, 38505, 42405};
uint8_t authenticatedUser;

uint8_t userShift;
unsigned long sendTime;

/*
 *  Code to control Trellis LEDs
 */

void trellisFlashBoard(unsigned short repeat) {
  trellisAllLEDsOff();
  for(unsigned short i = 0; i < repeat; i++) {
    for (uint8_t i=0; i<16; i++) {
      trellis.setLED(i);
      trellis.writeDisplay();
      delay(50);
    }
    trellisAllLEDsOff();
  }
}

void trellisAllLEDsOff() {
  for (uint8_t i=0; i<16; i++) {
    trellis.clrLED(i);
  }
  trellis.writeDisplay();
}

void trellisButtonPress() {
  if (trellis.readSwitches()) { // if a button was released
    digitalWrite(LED_ERR, LOW);
    for (uint8_t i=0; i<16; i++) { // determine which button
      if (trellis.justPressed(i)) {
        if (trellis.isLED(i)){
          trellis.clrLED(i);
        } else {
          trellis.setLED(i);
        }
        trellis.writeDisplay();
      }
    }
  }
}

/*
 * Read & Process trellis LEDs
 */

void getMatrixInput() {
  for (uint8_t i=0; i<16; i++) {
    uint8_t position = 15 - i;
    if (trellis.isLED(i)) {
      matrixValue = (1 << position) | matrixValue;
    } else {
      matrixValue = (~(1 << position)) & matrixValue;
    }
  }
}

/*
 *  Big Button Press and Big Button LED control
 */
void bigButtonPress() {
  buttonState = digitalRead(bigButtonPin);
  if (buttonState == HIGH) {
    digitalWrite(buttonledPin, LOW);
    if(lastButtonState != buttonState) {
      isSubmitButtonPressed = true;
    }
  } else {
    digitalWrite(buttonledPin, HIGH);
  }
  lastButtonState = buttonState;
}

/*
 *  Authentication methods
 */
boolean authenticateUserID() {
  for (uint8_t i=0; i<(sizeof validUserIDs / sizeof validUserIDs[0]); i++) {
    if(matrixValue == validUserIDs[i]){
      authenticatedUser = i;
      return true;
    }
  }
  authenticatedUser = 0;
  return false;
}

void sendMessage() {
  if(DEBUG_MODE) {
    Serial.print(matrixValue, HEX);
    Serial.print(" = ");
  }

  CustomProtocol::sendMessage(validUserIDs[authenticatedUser], encryptMessage(matrixValue));
}

unsigned short encryptMessage(unsigned short message) {
  for (int position = 0; position < 16; position += 4) {
    unsigned short mask = 0xf << position;
    byte oldNibble = (message & mask) >> position;
    byte newNibble = cryptEncodeNibble(oldNibble);

    message = (message & (~mask)) | (newNibble << position);
  }
  if(DEBUG_MODE) {
    Serial.println(message, HEX);
  }
  return message;
}

void updateReadArduinoStatus(uint8_t newStatus) {
  writeArduinoState = newStatus;
  switch (writeArduinoState) {
    case STATE_UID: //Waiting for user ID
      if(DEBUG_MODE) {
        Serial.println("STATE_UID");
      }
      digitalWrite(LED_UID, HIGH);
      digitalWrite(LED_MSG, LOW);
      break;
    case STATE_MSG:
      if(DEBUG_MODE) {
        Serial.println("STATE_MSG");
      }
      digitalWrite(LED_UID, LOW);
      digitalWrite(LED_MSG, HIGH);
      break;
    case STATE_ACK:
      if(DEBUG_MODE) {
        Serial.println("STATE_ACK");
      }
      digitalWrite(LED_UID, LOW);
      digitalWrite(LED_MSG, LOW);
      authenticatedUser = 0;
      break;
    default:
      break;
  }
}

void setup() {
  // initilze serial connection
  Serial.begin(9600);
  if(DEBUG_MODE) {
    Serial.println("Trellis Setup");
  }

  // initlize trellis board & LEDs
  trellis.begin(0x70);
  pinMode(LED_UID, OUTPUT);
  pinMode(LED_MSG, OUTPUT);

  // light up trellis board sequentially to indicate initalization
  trellisFlashBoard(1);

  //Initalize BigButton
  pinMode(bigButtonPin, INPUT);
  pinMode(buttonledPin, OUTPUT);

  //Initalize variables
  updateReadArduinoStatus(STATE_UID);
  isSubmitButtonPressed = false;
  lastButtonState = 1;
  digitalWrite(LED_ERR, LOW);
}

void loop() {
  // Background code
  delay(20);
  bigButtonPress();

  // State Code
  switch (writeArduinoState) {
    case STATE_UID: //Waiting for user ID
      trellisButtonPress();
      if (isSubmitButtonPressed){
        isSubmitButtonPressed = false;
        getMatrixInput();
        if (authenticateUserID()){
          trellisAllLEDsOff();
          updateReadArduinoStatus(STATE_MSG);
        } else {
          digitalWrite(LED_ERR, HIGH);
        }
      }
      break;
    case STATE_MSG: //Waiting for message
      trellisButtonPress();
        if (isSubmitButtonPressed){
          isSubmitButtonPressed = false;
          getMatrixInput();
          sendMessage();
          sendTime = millis();
          updateReadArduinoStatus(STATE_ACK);
        }
        break;
    case STATE_ACK: // Waiting for acknowledge
      if ((millis() - sendTime) > WAIT_TIME) {
        trellisFlashBoard(5);
        updateReadArduinoStatus(STATE_UID);
        digitalWrite(LED_ERR, HIGH);
      }

      // waiting for response
      if (Serial.available()) {
        // reading incoming bytes
        byte nextByte = Serial.read();
        if (CustomProtocol::isSuccessfulAcknowledgement(nextByte)) {
          trellisFlashBoard(1);
          updateReadArduinoStatus(STATE_UID);
        }
      }
      break;
    default:
      break;
  }

}
