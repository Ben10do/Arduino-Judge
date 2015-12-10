/*
 * Arduino_Judge.ino
 * ARDUINO_JUDGE
 * 
 * Is your piezo higher than their's? Is your LED flashing
 * the fastest? Good - time to whack them with a hammer!
 * (...metaphorically speaking, of course)
 * Partially based on Nintendo's Game & Watch Judge game.
 * 
 * By: Daniel Barlow and Ben Hetherington
 */

#include <Servo.h>
#include "Arduino_Judge.h"

const int maxScore = 40;

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
  
  delay(2000);

  // TODO: Maybe make a sound effect or play a light show
  // (just while the LCD is displaying its stuff)
  
  // Set up serial, random seed, and servo
  beginArduinoSerial();
  randomSeed(analogRead(randomPin));
  updateServo(0);
  
  initCommunication();
}

void initCommunication() {
  // Handshake; ensures that both Arduinos are in sync
  clearSerialBuffer();
  bool handshakeDone = false;

  // Animation variables
  byte fadeValue = 1;
  bool isGoingUpwards = true;

  while (!handshakeDone) {
    fadeStatusLED(&fadeValue, &isGoingUpwards);
    
    handshakeDone = tryHandshake();
  }
  
  // Decide on who should each player
  amPlayerTwo = determinePlayers();

  // Indicate who's who on the LED
  analogWrite(analogLED, 0);
  setFourBitLEDs(!amPlayerTwo ? 0b1100 : 0b0011);

  // Play SFX
  playHandshakeCompleteSFX();
  setFourBitLEDs(0b0000);
  
  score = 0;
  // Ready to start the game!
}

void fadeStatusLED(byte *fadeValue, bool *isGoingUpwards) {
  // Fading a status LED, to indicate we're ready
  analogWrite(analogLED, *fadeValue);
    
    if (*isGoingUpwards) {
      *fadeValue *= 2;
      if (*fadeValue == 0) {
        *fadeValue = 128;
        *isGoingUpwards = false;
      }
      
    } else {
      *fadeValue /= 2;
      if (*fadeValue == 0) {
        *fadeValue = 1;
        *isGoingUpwards = true;
      }
    }
}

void loop() {
  // Setting the next game and variables
  currentGame = decideOnGame(currentGame);
  communicateRandomNumbers(gameMaxNumbers[currentGame], &myNumber, &otherNumber);
  int countdownDelay = 95 + (getSharedRandomNumber(8) * 15);

  // Start the next game, with a count-in.
  playCountdownSFX(countdownDelay);
  GameResult result = runMicrogame(currentGame, myNumber, otherNumber);
  
  updateScore(result);
  setAllLEDs(LOW);
  delay(800);
}

// Button interrupts
// Feel free to expand if they need expanding
// Return as quickly as possible, to avoid missing serial inputs

void lowerButtonPressedInterrupt() {
  if (!higherButtonPressed && !lowerButtonPressed) {
    lowerButtonPressed = true;
    higherButtonPressed = false;
    millisAtButtonPress = millis();
  }
}

void higherButtonPressedInterrupt() {
  if (!higherButtonPressed && !lowerButtonPressed) {
    higherButtonPressed = true;
    lowerButtonPressed = false;
    millisAtButtonPress = millis();
  }
}

void enableInterrupts() {
  attachInterrupt(digitalPinToInterrupt(lowerButton), lowerButtonPressedInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(higherButton), higherButtonPressedInterrupt, FALLING);
}

// Additional functions

void setFourBitLEDs(byte value) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(fourBitLEDs[i], (value >> (3 - i)) & 1);
  }
}

void setAllLEDs(bool value) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(fourBitLEDs[i], value);
  }
  digitalWrite(whiteLED, value);
  digitalWrite(analogLED, value);
}

void flashHigherPlayersLED(bool hadHigherNumber) {
  // Flashes the white LED of the player that had the higher
  // random number (and so had a higher note, etc.)
  if (hadHigherNumber) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(whiteLED, HIGH);
      delay(250);
      digitalWrite(whiteLED, LOW);
      delay(250);
    }
  } else {
    delay(500 * 3);
  }
}

void updateScore(GameResult result) {
  // Each equivalent case must mirror each other!
  // e.g. CorrectAttack increases the score by correctAttackPoints,
  // whilst WasCorrectlyAttacked decreases the score by correctAttackPoints.
  bool hadHigherNumber;
  
  switch (result) {
    case CorrectAttack:
      score += correctAttackPoints;
      hadHigherNumber = true;
      playCorrectAttackSFX(true);
      break;

    case WasCorrectlyAttacked:
      score -= correctAttackPoints;
      hadHigherNumber = false;
      playCorrectAttackSFX(false);
      break;

    case IncorrectAttack:
      score -= incorrectAttackPoints;
      hadHigherNumber = false;
      playIncorrectAttackSFX(true);
      break;

    case WasIncorrectlyAttacked:
      score += incorrectAttackPoints;
      hadHigherNumber = true;
      playIncorrectAttackSFX(false);
      break;
    
    case CorrectDodge:
      score += correctDodgePoints;
      hadHigherNumber = false;
      playCorrectDodgeSFX(true);
      break;

    case WasCorrectlyDodged:
      score -= correctDodgePoints;
      hadHigherNumber = true;
      playCorrectDodgeSFX(false);
      break;
    
    case IncorrectDodge:
      score -= incorrectDodgePoints;
      hadHigherNumber = true;
      playIncorrectDodgeSFX(true);
      break;
      
    case WasIncorrectlyDodged:
      score += incorrectDodgePoints;
      hadHigherNumber = false;
      playIncorrectDodgeSFX(false);
      break;

    default:
      // No change to score.
      break;
  }

  updateServo(score);

  if (score >= maxScore) {
    handleVictory(true);
  } else if (score <= -maxScore) {
    handleVictory(false);
  } else {
    flashHigherPlayersLED(hadHigherNumber);
  }
}

void handleVictory(bool didWin) {
  // Play the sound effects and switch on an LED
  playInstantOfVictorySFX(didWin);
  
  digitalWrite(whiteLED, didWin ? HIGH : LOW);
  playVictoryJingleSFX(didWin);
  
  digitalWrite(whiteLED, LOW);
  playGameOverSFX();

  // Resetting the servo, before restarting the game
  score = 0;
  updateServo(score);
}

void updateServo(int score) {
  // Maps between -40 to +40.
  score = constrain(score, -maxScore, maxScore);
  score = map(score, -maxScore, maxScore, 10, 170);
  servo.attach(servoPin);
  servo.write(score);
  delay(250); // Give it a little time to move
  servo.detach();
}

void reset() {
  // Restarts the sketch
  // However, we have to reset all the hardware ourselves
  endArduinoSerial();
  setAllLEDs(LOW);
  noTone(piezo);
  servo.detach();
  
  // The actual reset happens here:
  asm("jmp 0");
}

