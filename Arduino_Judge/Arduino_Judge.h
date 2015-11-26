/*
 * Arduino_Judge.h
 * ARDUINO_JUDGE
 * 
 * Includes all of the main game variables, as well as
 * constants for pins and Game IDs.
 */

#include <Arduino.h>

// All of the microgames!
typedef enum GameID {
  PiezoPitch = 0, // Piezo - highest note
  PiezoRhythm,    // Piezo - fastest rhythm
  LEDNumber,      // LED   - 4-bit highest number
  LEDBrightest,   // LED   - brightest
  LEDFrequency,   // LED   - fastest frequency
  LDRCover        // LDR   - first to cover
};

// The maximum RNG numbers for each game (1 = N/A)
byte gameMaxNumbers[6] = {36, 10, 15, 5, 10, 2};

// The possible game outcomes
typedef enum GameResult {
  GameTied = 0,            // No-one's done anything (yet, at least).
  CorrectAttack,          // If you rightfully attack, and hit the other player
  IncorrectAttack,        // If you wrongly try to attack the other player
  WasCorrectlyAttacked,   // If the other player rightfully attacks you
  WasIncorrectlyAttacked, // If the other player wrongly attacks you
  CorrectDodge,           // If you successfully dodge the other player's attack
  IncorrectDodge,         // If you dodged when you could've attacked
  WasCorrectlyDodged,     // If the other player dodges your attack
  WasIncorrectlyDodged    // If the other player dodges when they could've attacked 
};

// Game variables
bool amPlayerTwo;
GameID currentGame = (GameID)2; // Won't start with the numbers game
byte score = 0;

// The randomly generated numbers used by each Arduino
byte myNumber = 0;
byte otherNumber = 0;

volatile bool higherButtonPressed = false;
volatile bool lowerButtonPressed = false;

// To work out the time when the button was pressed
<<<<<<< HEAD
unsigned int millisAtGameStart = 0;
volatile unsigned int millisAtButtonPress = 0;
=======
int millisAtGameStart = 0;
volatile int millisAtButtonPress = 0;
>>>>>>> 155d73db9f60d14c7e70c6fcd6d553efa91ebeb7

// Scoring constants
const byte correctAttackPoints = 5;
const byte incorrectAttackPoints = 8;
const byte correctDodgePoints = 2;
const byte incorrectDodgePoints = 1;

// Digital pin constants
const int lowerButton = 2;
const int higherButton = 3;
const int fourBitLEDs[] = {4, 5, 6, 7};
const int whiteLED = 8;
const int analogLED = 9;
const int piezo = 10;
const int servoPin = 11;
const int serialRX = 12;
const int serialTX = 13;

// Analog pin constants
const int LDRPin = 0;
const int randomPin = 5;

// Object for servo
Servo servo;

