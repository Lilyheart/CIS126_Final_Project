#include <Wire.h>
#include "Adafruit_Trellis.h"

#define STATE_UID 0
#define STATE_MSG 1
#define STATE_ACK 2

Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0);

uint16_t matrixValue;
uint8_t matrixNibbles[4];

uint8_t writeArduinoState;

boolean isSubmitButtonPressed;
uint8_t buttonState = 0;
uint8_t lastButtonState;
const uint8_t ledPin = 99;
const uint8_t LED_UID = 3;
const uint8_t LED_MSG = 4;
const uint8_t LED_ERR = 13;
const uint8_t bigButtonPin = 2;

uint16_t validUserIDs[3][2] = {
  {4680, 3},
  {38505, 6},
  {42405, 8}
};
uint8_t authenticatedUser;

uint8_t userShift;


/*
 *  Code to control Trellis LEDs
 */

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

uint32_t encodeNibble(uint16_t val, uint8_t k) {
  uint8_t shift = validUserIDs[authenticatedUser][1];
  return (((val >> (4*k)) + shift) % 16) & 0x0F;
}

uint16_t encodeMessage() {
  uint16_t encodedMessage;

  for (int i = 0; i < 4 ; i++) {
    uint8_t position = (4*i);
    encodedMessage = (encodeNibble(matrixValue, i) << position) | encodedMessage;
  }

  return encodedMessage;
}

/*
 *  Big Button Press and Big Button LED control
 */
void bigButtonPress() {
  buttonState = digitalRead(bigButtonPin);
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    if(lastButtonState != buttonState) {
      isSubmitButtonPressed = true;
    }
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
  lastButtonState = buttonState;
}

/*
 *  Authentication methods
 */
boolean authenticateUserID() {
  for (uint8_t i=0; i<(sizeof validUserIDs / sizeof validUserIDs[0]); i++) {
    if(matrixValue == validUserIDs[i][0]){
      userShift = validUserIDs[i][1];
      authenticatedUser = i;
      return true;
    }
  }
  authenticatedUser = 0;
  return false;
}

/*
 *  Authentication methods
 */
void updateReadArduinoStatus(uint8_t newStatus) {
  writeArduinoState = newStatus;
  switch (writeArduinoState) {
    case STATE_UID: //Waiting for user ID
      Serial.println("STATE_UID");
      digitalWrite(LED_UID, HIGH);
      digitalWrite(LED_MSG, LOW);
      break;
    case STATE_MSG:
      Serial.println("STATE_MSG");
      digitalWrite(LED_UID, LOW);
      digitalWrite(LED_MSG, HIGH);
      break;
    case STATE_ACK:
      Serial.println("STATE_ACK");
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
  Serial.println("Trellis Setup");


  // initlize trellis board & LEDs
  trellis.begin(0x70);
  pinMode(LED_UID, OUTPUT);
  pinMode(LED_MSG, OUTPUT);

  // light up trellis board sequentially to indicate initalization
  for (uint8_t i=0; i<16; i++) {
    trellis.setLED(i);
    trellis.writeDisplay();
    delay(50);
  }

  delay(150);
  trellisAllLEDsOff();

  //Initalize BigButton
  pinMode(bigButtonPin, INPUT);
  pinMode(ledPin, OUTPUT);



  //Initalize variables
  //TODO Test writeArduinoState init
  updateReadArduinoStatus(STATE_UID);
  isSubmitButtonPressed = false;
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
        isSubmitButtonPressed = false; //TODO Put this somewhere better.
        getMatrixInput();
        if (authenticateUserID()){
          trellisAllLEDsOff();
          updateReadArduinoStatus(STATE_MSG);
        } else {
          digitalWrite(LED_ERR, HIGH);
          // updateErrorLED(INPUTSTATE); TODO
          // resetMatrix(); TODO
          // stay in state 1 TODO
        }
      }
      break;
    case STATE_MSG: //Waiting for message
      trellisButtonPress();
        if (isSubmitButtonPressed){
          getMatrixInput();
          Serial.println(encodeMessage(), HEX);
        // sendMessage();{ TODO
          updateReadArduinoStatus(STATE_ACK);
        }
      break;
    case STATE_ACK: // Waiting for acknowledge
//      if(isMessageReceived) {
        // parseCRAP();
//        if(isMessageSuccessful) {
          updateReadArduinoStatus(STATE_UID);
//        } else {
          // light error led
          // flash 4x4 a few times
          updateReadArduinoStatus(STATE_UID);
          // move to state 1
//        }
//      }
      break;
    default:
      break;
  }

}
