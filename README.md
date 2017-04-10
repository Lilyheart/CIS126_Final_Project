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

If any stage fails, reset the program. (Reset feedback) Make sure keep the status indicators convey the current state.

**Write Arduino:**
1. The user enters their ID code and presses a button to enter it.
2. Give feedback that user ID was successful or failed.
3. After user ID is validated, the user enters a message to send and presses a button to send it.
4. Encrypt the message, and display the encrypted message.
5. Send the encrypted message to the second arduino, using our custom protocol.

**Read Arduino:**
1. Poll for beginning of our custom protocol.
2. Read in the user ID and encrypted message from the protocol.
3. Validate that the message was received successfully (status indicator feedback).
4. After receiving a successful message, decrypt the message message and display it.

## **CRAP**

**C**ommunication **R**elay **A**rduino **P**rotocol

**Individual Byte:**

Each nibble of information is sent with a protocol code (the code is a nibble as well).

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
