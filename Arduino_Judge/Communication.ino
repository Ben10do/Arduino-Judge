/*
 * Communication.ino
 * ARDUINO_JUDGE
 * 
 * Includes all the functions that are related to communicating
 * between the two Arduinos.
 * 
 * The communication is relatively simple, and is state based
 * (both Arduinos should expect to send and recieve the same
 * kind of data at the same time).
 */

#include <SoftwareSerial.h>

// Communication constants
const byte handshakeInit = 0x64;
const byte handshakeResponse = 0x42;

// Serial object
SoftwareSerial arduinoSerial(serialRX, serialTX);

// Start/end functions

void beginSerial() {
  arduinoSerial.begin(115200); // About 14 KB/s
}

void endSerial() {
  arduinoSerial.end(); // TODO: Check if this is necessary
}

// Functions for initial communication

bool tryHandshake() {
  // Returns true if handshake was successful;
  // otherwise, returns false.
  arduinoSerial.write(handshakeInit);
  
  for (int i = 0; i < 20; i++) {
    // Wait for a response
    delay(5);
    if (arduinoSerial.available()) {
      byte readByte = arduinoSerial.read();
      
      if (readByte == handshakeInit) {
        // If the other device has sent a handshake,
        // acknowledge it, and start the game
        arduinoSerial.write(handshakeResponse);
        return true;
      } else if (readByte == handshakeResponse) {
        // If the other device has acknowledged a handshake,
        // start the game.
        return true;
      }
    }
  }

  return false;
}

bool determinePlayers() {
  // Returns false if P1, true if P2.
  // The players are guaranteed to have been determined once
  // this function returns, but if there is no response, the
  // Arduino will lock up. This is fine for this project.
  byte myRandomNum;
  byte otherArduinoRandomNum;
  
  communicateRandomNumbers(0x100, &myRandomNum, &otherArduinoRandomNum);
  
  if (myRandomNum > otherArduinoRandomNum) {
    // We're P1
    return false;
    
  } else if (myRandomNum < otherArduinoRandomNum) {
    // We're P2
    return true;
  }
}

// Game communication

GameID decideOnGame(GameID previousGame) {
  // Returns the Game ID of the next game, avoiding the
  // previous game. Decided by P1, P2 merely waits for
  // a response.
  if (!amPlayerTwo) {
    // Then it's up to us to decide the next game!
    for (;;) {
      GameID nextGame = (GameID)random(6);
      
      if (nextGame != previousGame) {
        
        if (nextGame == LEDNumber) {
          // Get another number; makes this game less common
          nextGame = (GameID)random(6);
        }
        
        arduinoSerial.write(nextGame);
        return nextGame;
      }
    }
  } else {
    // Wait to hear what the next game will be
    while (!arduinoSerial.available()) {
      // Wait until we recieve the game ID
    }

    return (GameID)arduinoSerial.read();
  }
}

void communicateRandomNumbers(int max, byte *myNumber, byte *otherNumber) {
  // The numbers are guaranteed to have been determined once
  // this function returns, but if there is no response, the
  // Arduino will lock up. This is fine for this project.
  byte myRandomNum;
  byte otherArduinoRandomNum;

  if (max > 0x100) {
    // Ensure that we're not making more than one byte
    max = 0x100;
  }
  
  do {
    myRandomNum = random(max);
    arduinoSerial.write(myRandomNum);
    while (!arduinoSerial.available()) {
      // Wait until we recieve the other Arduino's number
    }
    
    otherArduinoRandomNum = arduinoSerial.read();
  } while (myRandomNum == otherArduinoRandomNum);

  // Pass them back (checking for NULL in the process)
  
  if (myNumber) {
    *myNumber = myRandomNum;
  }

  if (otherNumber) {
    *otherNumber = otherArduinoRandomNum;
  }
}

