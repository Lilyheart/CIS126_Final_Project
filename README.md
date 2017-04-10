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
- [ ] **It should address the communication based on a cipher for security.**
- [ ] **It should include user validation.**
- [ ] **Use some visual display elements, to add to the overall presentation.**

**Example user story:**
1. Enter a user ID code and receive a confirmation (LEDs, flag waving, sound).
2. If valid return a cipher key
3. Send the cipher key back, encrypted, and confirm the exchange. This will demonstrate the key was received and the cipher agreed to.
4. Send the cipher encrypted text from the first Arduino to the second Arduino, where it will be decoded back to plain text and displayed. 
