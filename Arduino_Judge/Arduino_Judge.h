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

// The maximum RNG numbers for each game (0 = N/A)
byte gameMaxNumbers[6] = {36, 10, 15, 5, 10, 0};

// The possible game outcomes
typedef enum GameResult {
  CorrectAttack = 0,
  IncorrectAttack,
  CorrectDoge,
  IncorrectDodge
};

// Game variables
bool amPlayerTwo;
GameID currentGame = (GameID)2; // Won't start with the numbers game

// The randomly generated numbers used by each Arduino
byte myNumber = 0;
byte otherNumber = 0;

volatile bool higherButtonPressed = false;
volatile bool lowerButtonPressed = false;

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

