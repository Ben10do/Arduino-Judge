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
  beginArduinoSerial();
  Serial.begin(115200);
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
  
  score = 0;
  // Ready to start the game!
}

void loop() {
  // Resetting these variables before starting again
  lowerButtonPressed = false;
  higherButtonPressed = false;

  // Setting the next game and variables
  currentGame = decideOnGame(currentGame);
  int countdownDelay = 80 + (getSharedRandomNumber(8) * 10);
  communicateRandomNumbers(gameMaxNumbers[currentGame], &myNumber, &otherNumber);

  // Start the next game, with a count-in.
  playCountdownSFX(currentGame, countdownDelay);
  GameResult result = runMicrogame(currentGame, myNumber, otherNumber);

  // TODO: Handle the result!
  // Play relevant SFX/Animations
  updateScore(result);
  flashHigherPlayersLED(myNumber, otherNumber);

  // Test code
  Serial.print("Game No.: ");
  Serial.println(currentGame);
  Serial.print("My number: ");
  Serial.println(myNumber);
  Serial.print("Other number: ");
  Serial.println(otherNumber);

  delay(500);
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

void flashHigherPlayersLED(byte myNumber, byte otherNumber) {
  // Flashes the white LED of the player that had the higher
  // random number (and so had a higher note, etc.)
  if (myNumber >= otherNumber) {
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
  switch (result) {
    case CorrectAttack:
      score += correctAttackPoints;
      break;

    case WasCorrectlyAttacked:
      score -= correctAttackPoints;
      break;

    case IncorrectAttack:
      score -= incorrectAttackPoints;
      break;

    case WasIncorrectlyAttacked:
      score += incorrectAttackPoints;
      break;
    
    case CorrectDodge:
      score += correctDodgePoints;
      break;

    case WasCorrectlyDodged:
      score -= correctDodgePoints;
      break;
    
    case IncorrectDodge:
      score -= incorrectDodgePoints;
      break;
      
    case WasIncorrectlyDodged:
      score += incorrectDodgePoints;
      break;

    case GameTied:
    default:
      // No change to score.
      break;
  }

  updateServo(score);

  if (score >= 80) {
    // TODO: Handle the victory!
  } else if (score <= -80) {
    // TODO: Handle the loss!
  }
}

void updateServo(int score) {
  // Maps between -80 to +80.
  score = constrain(score, -80, 80);
  servo.write(map(score, -80, 80, 10, 170));
  delay(200); // Give it a little time to move
}

void reset() {
  // Restarts the sketch
  // However, we have to reset all the hardware ourselves
  endArduinoSerial();
  Serial.end();
  for (int i = 0; i < 4; i++) {
    digitalWrite(fourBitLEDs[i], LOW);
  }
  digitalWrite(whiteLED, LOW);
  digitalWrite(analogLED, LOW);
  noTone(piezo);
  servo.detach();
  
  // The actual reset happens here:
  asm("jmp 0");
}

