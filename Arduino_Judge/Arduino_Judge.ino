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
  beginArduinoSerial();
  Serial.begin(115200);
  randomSeed(analogRead(randomPin));
  updateServo(0);
  
  initCommunication();
}

void initCommunication() {
  // Handshake; ensures that both Arduinos are in sync
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

  // Indicate who's who, and play SFX
  analogWrite(analogLED, 0);
  setFourBitLEDs(!amPlayerTwo ? 0b1100 : 0b0011);
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
  int countdownDelay = 80 + (getSharedRandomNumber(8) * 10);

  // Start the next game, with a count-in.
  playCountdownSFX(countdownDelay);
  GameResult result = runMicrogame(currentGame, myNumber, otherNumber);

  // Clearing up after the game, if it used the piezo.
  noTone(piezo);
  
  // TODO: Play relevant SFX/Animations
  updateScore(result);
  setAllLEDs(LOW);
}

// Button interrupts
// Feel free to expand if they need expanding
// Return as quickly as possible, to avoid missing serial inputs

void lowerButtonPressedInterrupt() {
  lowerButtonPressed = true;
  higherButtonPressed = false;
  millisAtButtonPress = millis();
  disableInterrupts(); // Prevents repeated presses
}

void higherButtonPressedInterrupt() {
  higherButtonPressed = true;
  lowerButtonPressed = false;
  millisAtButtonPress = millis();
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
      break;

    case WasCorrectlyAttacked:
      score -= correctAttackPoints;
      hadHigherNumber = false;
      break;

    case IncorrectAttack:
      score -= incorrectAttackPoints;
      hadHigherNumber = false;
      break;

    case WasIncorrectlyAttacked:
      score += incorrectAttackPoints;
      hadHigherNumber = true;
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

  if (score >= 80) {
    handleVictory(true);
  } else if (score <= -80) {
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
  // Maps between -80 to +80.
  score = constrain(score, -80, 80);
  score = map(score, -80, 80, 10, 170);
  servo.attach(servoPin);
  servo.write(score);
  delay(250); // Give it a little time to move
  servo.detach();
}

void reset() {
  // Restarts the sketch
  // However, we have to reset all the hardware ourselves
  disableInterrupts();
  endArduinoSerial();
  Serial.end();
  setAllLEDs(LOW);
  noTone(piezo);
  servo.detach();
  
  // The actual reset happens here:
  asm("jmp 0");
}

