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

void beginArduinoSerial() {
  arduinoSerial.begin(9600);
}

void endArduinoSerial() {
  arduinoSerial.end();
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
    waitForResponse();

    return (GameID)arduinoSerial.read();
  }
}

void waitForResponse() {
  for (int i = 0; i < 50000; i++) {
    // Call to arduinoSerial.available() takes ≈20µs
    // Consequently, the timeout is ≈1s.
    if (arduinoSerial.available()) {
      return;
    }
  }

  // If we've reached here, we've timed out.
  Serial.println("Timeout!");
  handleCommunicationError();
}

void communicateRandomNumbers(int max, byte *myNumber, byte *otherNumber) {
  // The numbers are guaranteed to have been determined once
  // this function returns.

  while (arduinoSerial.available()) {
    // Discard any excess bytes that we don't need
    arduinoSerial.read();
  }

  delay(5);

  byte myRandomNum;
  byte otherArduinoRandomNum;

  if (max > 0x100 || max < 2) {
    // Ensure that we're not making more than one byte
    // and that we're not always going to get the same number!
    max = 0x100;
  }

  do {
    myRandomNum = random(max);
    arduinoSerial.write(myRandomNum);

    waitForResponse();
    otherArduinoRandomNum = arduinoSerial.read();
  } while (myRandomNum == otherArduinoRandomNum);

  if (otherArduinoRandomNum >= max) {
    Serial.println("Hang on, the other Arduino's number is wrong!");
    Serial.print("max: ");
    Serial.println(max);
    Serial.print("My number: ");
    Serial.println(myRandomNum);
    Serial.print("Other number: ");
    Serial.println(otherArduinoRandomNum);
    handleCommunicationError();
  }

  // Pass the numbers back (checking for NULL in the process)

  if (myNumber) {
    *myNumber = myRandomNum;
  }

  if (otherNumber) {
    *otherNumber = otherArduinoRandomNum;
  }
}

int getSharedRandomNumber(int max) {
  // Returns a random number that will be shared by
  // both Arduinos. Decided by player one.
  if (!amPlayerTwo) {
    // Then it's up to us to get the number!
    int number = random(max);
    arduinoSerial.write(number);
    return number;

  } else {
    // Wait to hear what the number is
    waitForResponse();

    int number = arduinoSerial.read();

    if (number >= max) {
      handleCommunicationError();
    }

    return number;
  }
}

GameResult communicateGameStatus(GameResult myStatus) {
  // Send GameTied if the game's not yet been won or lost
  // Otherwise, send: CorrectAttack,   IncorrectAttack,
  //                  CorrectDodge, or IncorrectDodge.

  arduinoSerial.write(myStatus);
  waitForResponse();
  GameResult otherStatus = (GameResult)arduinoSerial.read();

  if (myStatus == GameTied && otherStatus == GameTied) {
    // Then keep the game going!
    return GameTied;

  } else if (myStatus == GameTied && otherStatus != GameTied) {
    // Then we'll return the opposite status
    return (GameResult)((int)otherStatus + 2);

  } else if (myStatus != GameTied && otherStatus == GameTied) {
    // Then we'll return our status
    return myStatus;

  } else {
    // If both players reacted before the Arduinos could
    // communicate, we're going to have to work out who was first.
    unsigned int myResponseTime = millisAtButtonPress - millisAtGameStart;

    arduinoSerial.write(myResponseTime & 0xFF); // Send least significant byte first
    arduinoSerial.write(myResponseTime >> 8);   // (i.e. little-endian)

    waitForResponse();
    unsigned int otherResponseTime = arduinoSerial.read();
    otherResponseTime |= arduinoSerial.read() << 8;

    if (myResponseTime < otherResponseTime) {
      // If this Arduino was fastest, then it's as we originally thought.
      return myStatus;

    } else if (myResponseTime > otherResponseTime) {
      // If this Arduino was slower, then I'll have to base it on the other Arduino.
      return (GameResult)((int)otherStatus + 2);

    } else {
      // If both are the same, we'll let player one decide who wins.
      bool winningPlayerIsPlayerTwo = getSharedRandomNumber(2);

      if (winningPlayerIsPlayerTwo ^ !amPlayerTwo) {
        return myStatus;
      } else {
        return (GameResult)((int)otherStatus + 2);
      }

    }
  }
}

void handleCommunicationError() {
  // If the Arduino recieves unexpected data, or doesn't
  // recieve any data after a timeout, we'll just reset
  // the sketch, and wait for an initial connection again.
  Serial.println("Communication Error.\n\n");
  playCommunicationErrorSFX();
  delay(1000);
  reset();
}

