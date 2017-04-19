# CIS126 Final Project

## Project Description & Checklist

This is a paper first. The coded is just part of the content.

- [ ] **Write a paper about your Arduino based project.**  
The paper should include research about the Arduino platform (atmega328p), Architecture and Organization.  
In your paper, describe your design, the choices you made and what you did to implement the various sections.
  - [ ] memory
  - [ ] bus architecture
  - [ ] RISC features
  - [ ] register file
  - [ ] instructions
  - [ ] addressing
- [ ] **Your project should demonstrate Arduino programming and some of the coding must use assembly.**
  - [ ] arduino programming
  - [ ] assembly
- [ ] **It should include communication between two Arduinos, using a protocol of your own design.** (Serial over data pins)  
Since it is your implementation of a protocol, it may involve timing and pin state sampling, serial to parallel (shift register) elements etc.
- [ ] **It should have at least one table lookup somewhere in the solution written in assembly.**
- [ ] **It should make use of (some) motion, LED, and/or sound as status indicators.**
- [ ] **It should address the communication based on a cipher for security.** (Using Caesar Cipher)
- [ ] **It should include user validation.**
- [ ] **Use some visual display elements, to add to the overall presentation.**

**Example user story:**
1. Enter a user ID code and receive a confirmation (LEDs, flag waving, sound).
2. If valid return a cipher key
3. Send the cipher key back (confirming the shift with the user), encrypted, and confirm the exchange. This will demonstrate the key was received and the cipher agreed to.
4. Send the cipher encrypted text from the first Arduino to the second Arduino, where it will be decoded back to plain text and displayed.

## Program Flow

If any stage fails, reset the program. There will be feedback provided when the program resets. There will also be status indicators to display the progress.

**Write Arduino:**
1. The user enters their ID code on the 4x4 keypad, and then the user presses a button to submit it.
1. Give feedback that user ID was a successful or failed attempt.
1. After the user ID is validated, the user enters a message to send and presses a button to submit it.
1. Encrypt the message.
1. Send the encrypted message to the second Arduino, using the custom protocol outlined below.
1. Receive acknowledgement from the Read Arduino and update status indicator.
1. Return to beginning state.



**Read Arduino:**
1. Poll for beginning of the custom protocol outlined below.
2. Read in the user ID and encrypted message from the protocol.
3. Validate that the message was received successfully, making sure to provide user feedback.
4. After receiving a successful message according to the protocol, decrypt the message message and display it.
5. After decrypting and displaying the message, return an acknowledgement to the write Arduino.

## CRA-Protocol

**C**ommunication  
**R**elay  
**A**rduino  
**P**rotocol

**Individual Byte:**

Each nibble of information is sent with a protocol code (the code is a nibble as well). This means each nibble of information results in one byte of informaiton.

| Bits | What it is |
| --- | --- |
| 7 - 4 (most significant bits) | protocol codes |
| 3 - 0 (least significant bits) | information |

**Protocol Codes:**

| Code | Description |
| --- | --- |
| 0011 | User ID |
| 1100 | Message Length |
| 1111 | Encrypted Message |
| anything else | invalid or interupt |

**Full Protocol:**

| Bytes | Description |
| --- | --- |
| 0 - 3 | User ID |
| 4 - 5 | Length |
| 6 - N | Encrypted Message |
