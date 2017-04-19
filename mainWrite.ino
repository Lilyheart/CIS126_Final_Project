#include <Wire.h>
#include "Adafruit_Trellis.h"

Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0);
uint8_t writeArduinoState; //TODO uint8_t?
boolean isSubmitButtonReleased;

/*
 *  Code to control Trellis LEDs
 */

void trellisAllLEDsOff() {
  for (uint8_t i=0; i<16; i++) {
    trellis.clrLED(i);
  }
  trellis.writeDisplay();
}

//TODO Is it on release or press?
void trellisButtonRelease() {
  if (trellis.readSwitches()) { // if a button was released
    for (uint8_t i=0; i<16; i++) { // determine which button
      if (trellis.justPressed(i)) {
        //TODO Test LED toggles
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
 * Read trellis LEDs
 */

 //TODO

void setup() {
  // initilze serial connection
  Serial.begin(9600);
  Serial.println("Trellis Setup");

  // initlize trellis board
  trellis.begin(0x70);

  // TODO Test LED sequential LEDs
  // light up trellis board sequentially to indicate initalization
  int trellisLEDBoard[] = {12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3};
  for (uint8_t i=0; i<16; i++) {
    trellis.setLED(trellisLEDBoard);
    trellis.writeDisplay();
    delay(50);
  }

  //TODO Test trellisAllLEDsOff
  trellisAllLEDsOff();

  //Initalize variables
  //TODO Test writeArduinoState init
  writeArduinoState = 1;
  isSubmitButtonReleased = false;
}

void loop() {
  // Background code

  // State Code
  switch (writeArduinoState) {
    case 1: //Waiting for user ID
      //TODO Test trellisButtonRelease
      trellisButtonRelease();
      if (isSubmitButtonReleased){
        // getMatrixInput();
        // authenticateUserID();
        // if (userIsAuthenticated){
          // resetMatrix();{
          // updateReadArduinoStatus(INPUTSTATE);
          // move to state 2
        // } else {
          // updateErrorLED(INPUTSTATE);
          // resetMatrix();
          // stay in state 1
        // }
      }
      break;
    // case 2: //Waiting for message
      // trellisButtonRelease();
      // if (isSubmitButtonReleased){
        // getMatrixInput();
        // encodeMessage();
        // sendMessage();{
        // updateReadArduinoStatus(INPUTSTATE);
        // move to state 3
      // }
      // break;
    // case 3: // Waiting for acknowledge
      // if(isMessageReceived) {
        // parseCRAP();
        // if(isMessageSuccessful) {
          // move to state 1
        // } else {
          // light error led
          // flash 4x4 a few times
          // move to state 1
        // }
      // }
      // break;
    default:
      break;
  }

}
