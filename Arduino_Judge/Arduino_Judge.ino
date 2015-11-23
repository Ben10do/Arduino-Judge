/*
 * Arduino_Judge.ino
 * ARDUINO_JUDGE
 * 
 * Partially based on Nintendo's Game & Watch Judge game.
 * Is your piezo higher than their's? Is your LED flashing
 * the fastest? Good - time to whack them with a hammer!
 * (...metaphorically speaking, of course)
 * 
 * By: Daniel Barlow and Ben Hetherington
 */

#include <Servo.h>
#include "Arduino_Judge.h"

void setup() {
  // Setup all the pins
  pinMode(lowerButton, INPUT);
  pinMode(higherButton, INPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(fourBitLEDs[i], OUTPUT);
  }
  pinMode(whiteLED, OUTPUT);
  pinMode(analogLED, OUTPUT);
  pinMode(piezo, OUTPUT);
  pinMode(servoPin, OUTPUT);
  
  // Set up serial, random seed, and servo
  beginSerial();
  randomSeed(analogRead(randomPin));
  servo.attach(servoPin);
  
  // Handshake; ensures that both Arduinos are in sync
  bool handshakeDone = false;
  while (!handshakeDone) {
    // Flashing a status LED, to indicate we're ready
    digitalWrite(fourBitLEDs[0], !digitalRead(fourBitLEDs[0]));
    
    handshakeDone = tryHandshake();
  }
  
  // Decide on who should be player one
  amPlayerTwo = determinePlayers();

  // Indicate who's who, and play SFX
  setFourBitLEDs(!amPlayerTwo ? 0b1100 : 0b0011);
  playHandshakeCompleteSFX();
  setFourBitLEDs(0b0000);
  
  // Ready to start the game!
  
  static int a = generateNoteFreq(49); // TODO: Remove this
  // (only there to include the extra bulk of that function)
}

void loop() {
  // Resetting these variables before starting again
  lowerButtonPressed = false;
  higherButtonPressed = false;

  // Setting the next game and variables
  currentGame = decideOnGame(currentGame);
  communicateRandomNumbers(gameMaxNumbers[currentGame], &myNumber, &otherNumber);

  // Test code
  Serial.print("Game No.: ");
  Serial.println(currentGame);
  Serial.print("My number: ");
  Serial.println(myNumber);
  Serial.print("Other number: ");
  Serial.println(otherNumber);

  delay(5000);
}

// Button interrupts
// Feel free to expand if they need expanding
// Return as quickly as possible, to avoid missing serial inputs

void lowerButtonPressedInterrupt() {
  lowerButtonPressed = true;
  disableInterrupts(); // Prevents repeated presses
}

void higherButtonPressedInterrupt() {
  higherButtonPressed = true;
  disableInterrupts(); // Prevents repeated presses
}

void enableInterrupts() {
  attachInterrupt(digitalPinToInterrupt(lowerButton), lowerButtonPressedInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(higherButton), higherButtonPressedInterrupt, FALLING);
}

void disableInterrupts() {
  detachInterrupt(digitalPinToInterrupt(lowerButton));
  detachInterrupt(digitalPinToInterrupt(higherButton));
}

// Additional functions

void setFourBitLEDs(byte value) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(fourBitLEDs[i], (value >> (3 - i)) & 1);
  }
}

