/*
 * Microgames.ino
 * ARDUINO_JUDGE
 * 
 * Includes the code for the individual microgames.
 */

GameResult runMicrogame(GameID game, byte myNumber, byte otherNumber) {
  lowerButtonPressed = false;
  higherButtonPressed = false;
  millisAtGameStart = millis();
  
  switch (game) {
    case PiezoPitch:
      return runPiezoPitch(myNumber, otherNumber);

    case PiezoRhythm:
      return runPiezoRhythm(myNumber, otherNumber);

    case LEDNumber:
      return runPiezoRhythm(myNumber, otherNumber);

    case LEDBrightest:
      return runLEDBrightest(myNumber, otherNumber);

    case LEDFrequency:
      return runLEDFrequency(myNumber, otherNumber);

    case LDRCover:
      return runLDRCover();

    default:
      handleCommunicationError();
  }
}

GameResult runPiezoPitch(byte myNumber, byte otherNumber) {
  // Piezo - highest note
  // TODO: Must write this microgame!
  return GameTied;
}

GameResult runPiezoRhythm(byte myNumber, byte otherNumber) {
  // Piezo - fastest rhythm
  // TODO: Must write this microgame!
  return GameTied;
}

GameResult runLEDNumber(byte myNumber, byte otherNumber) {
  // LED - 4-bit highest number
  GameResult gameState = GameTied;
  setFourBitLEDs(myNumber + 1);
  
  // Keep going until the game is no longer tied.
  while (gameState == GameTied) {
    if (lowerButtonPressed) {
      // Were they corrrect to push the dodge button?
      if (myNumber < otherNumber) {
        // yes
        gameState = CorrectDodge;
        
      } else {
        // No
        gameState = IncorrectDodge;
      }
      
    } else if (higherButtonPressed) {
      // Were they right to push the attack button?
      if (myNumber < otherNumber) {
        // no
        gameState = IncorrectAttack;
      }
      else {
        // yes
        gameState = CorrectAttack;
      }
    }
    
    // Comunicate the current game state.
    GameResult communicatedGameState = communicateGameStatus(gameState);
    if (communicatedGameState != GameTied) {
      return communicatedGameState;
    }
  }
}

GameResult runLEDBrightest(byte myNumber, byte otherNumber) {
  // LED - brightest
  int brightness = 55 + (myNumber * 40);
  GameResult gameState = GameTied;
  analogWrite(analogLED, brightness);
  
  // Keep going until the game is no longer tied.
  while (gameState == GameTied) {
    if (lowerButtonPressed) {
      //Were they corrrect to push the dodge button?
      if (myNumber < otherNumber) {
        // yes
        gameState = CorrectDodge;
      }
      else {
        // No
        gameState = IncorrectDodge;
        
      }
    } else if (higherButtonPressed) {
      // Were they right to push the attack button?
      if (myNumber < otherNumber) {
        // no
        gameState = IncorrectAttack;
        
      } else {
        // yes
        gameState = CorrectAttack;
      }
    }
    
    // Comunicate the current game state.
    GameResult communicatedGameState = communicateGameStatus(gameState);
    if (communicatedGameState != GameTied) {
      digitalWrite(analogLED, LOW);
      return communicatedGameState;
    }
  }
}

GameResult runLEDFrequency(byte myNumber, byte otherNumber) {
  // LED - fastest frequency
  // TODO: Must write this microgame!
  return GameTied;
}

GameResult runLDRCover() {
  // LDR - first to cover
  digitalWrite(whiteLED, HIGH);
  
  // Reaction time, this just lets the LED turn on. If the players
  // are reacting faster than this, they're probably cheating anyway.
  delay(100);
  
  // Obtain the initial value of the sensor pin
  int sensorValue = analogRead(LDRPin);
  GameResult gameState = GameTied;
  
  // Keep going until the game is no longer tied.
  while (gameState == GameTied) {
    int newValue = analogRead(LDRPin);
    
    // Cheek to see if there is a noticable increase (i.e. less light) over the LED.
    if (newValue >= sensorValue + 300 || newValue == 1023) {
      // Change game state to correct attack and record the time of this.
      millisAtButtonPress = millis();
      gameState = CorrectAttack;
    }
    
    // Comunicate the current game state.
    GameResult communicatedGameState = communicateGameStatus(gameState);
    if (communicatedGameState != GameTied) {
      digitalWrite(whiteLED, LOW);
      return communicatedGameState;
    }
  }
}

