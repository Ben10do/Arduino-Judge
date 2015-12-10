/*
 * Microgames.ino
 * ARDUINO_JUDGE
 * 
 * Includes the code for the individual microgames.
 */

GameResult runMicrogame(GameID game, byte myNumber, byte otherNumber) {
  lowerButtonPressed = false;
  higherButtonPressed = false;
  enableInterrupts();
  millisAtGameStart = millis();
  
  switch (game) {
    case PiezoPitch:
      return runPiezoPitch(myNumber, otherNumber);

    case PiezoRhythm:
      return runPiezoRhythm(myNumber, otherNumber);

    case LEDNumber:
      return runLEDNumber(myNumber, otherNumber);

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

GameResult checkButtonPresses(bool higherNumberWins) {
  bool myNumberIsBetter = myNumber > otherNumber;
  if (!higherNumberWins) {
    myNumberIsBetter = !myNumberIsBetter;
  }
  
  if (lowerButtonPressed) {
      // Was the player correct to press the lower button?
      if (myNumberIsBetter) {
        return IncorrectDodge; // No
      } else {
        return CorrectDodge;   // Yes
      }
      
    } else if(higherButtonPressed) {
      // Was the player correct to press the higher button?
      if (myNumberIsBetter) {
        return CorrectAttack;   // Yes
      } else {
        return IncorrectAttack; // No
      }
      
    } else {
      return GameTied;
    }
}

GameResult runPiezoPitch(byte myNumber, byte otherNumber) {
  // Game variables.
  GameResult gameState = GameTied;

  tone(piezo, generateNoteFreq(myNumber + 40));
  
  // Keep going until the game is no longer tied.
  while (gameState == GameTied) {
    
    gameState = checkButtonPresses(true);
    
    // Comunicate the current game state.
    GameResult communicatedGameState = communicateGameStatus(gameState);
    if (communicatedGameState != GameTied) {
      noTone(piezo);
      return communicatedGameState;
    }
  }
}

GameResult runPiezoRhythm(byte myNumber, byte otherNumber) {
  // Game variables.
  GameResult gameState = GameTied;
  int period = (myNumber+2) * 30;
  
  //Keep going until the game is no longer tied.
  while (gameState == GameTied) {

    // Work out if the pizeo should be on or off.
    int timeSinceGameStart = millis() - millisAtGameStart;
    int phase =  timeSinceGameStart/period;
    if (phase % 2 == 0) {
      tone(piezo, 988); // B_5
    } else {
      noTone(piezo);
    }
    
    gameState = checkButtonPresses(false);
    
    // Comunicate the current game state.
    GameResult communicatedGameState = communicateGameStatus(gameState);
    if (communicatedGameState != GameTied) {
      noTone(piezo);
      return communicatedGameState;
    }
  }
}

GameResult runLEDNumber(byte myNumber, byte otherNumber) {
  // LED - 4-bit highest number
  GameResult gameState = GameTied;
  setFourBitLEDs(myNumber + 1);
  
  // Keep going until the game is no longer tied.
  while (gameState == GameTied) {
    gameState = checkButtonPresses(true);
    
    // Comunicate the current game state.
    GameResult communicatedGameState = communicateGameStatus(gameState);
    if (communicatedGameState != GameTied) {
      return communicatedGameState;
    }
  }
}

GameResult runLEDBrightest(byte myNumber, byte otherNumber) {
  // LED - brightest
  int brightness = 10 + (myNumber * 10);
  GameResult gameState = GameTied;
  analogWrite(analogLED, brightness);
  
  // Keep going until the game is no longer tied.
  while (gameState == GameTied) {
    gameState = checkButtonPresses(true);
    
    // Comunicate the current game state.
    GameResult communicatedGameState = communicateGameStatus(gameState);
    if (communicatedGameState != GameTied) {
      digitalWrite(analogLED, LOW);
      return communicatedGameState;
    }
  }
}

GameResult runLEDFrequency(byte myNumber, byte otherNumber) {
  //Game variables.
  GameResult gameState = GameTied;
  int period = (myNumber+2)*30;
  
  //Keep going until the game is no longer tied.
  while (gameState == GameTied) {

    //Work out if the LED should be on or off.
    unsigned int timeSinceGameStart = millis() - millisAtGameStart;
    unsigned int phase =  timeSinceGameStart/period;
    if (phase % 2 == 0) {
      digitalWrite(analogLED, HIGH);
    } else {
      digitalWrite(analogLED, LOW);
    }
    
    gameState = checkButtonPresses(false);
    
    // Comunicate the current game state.
    GameResult communicatedGameState = communicateGameStatus(gameState);
    if (communicatedGameState != GameTied) {
      digitalWrite(analogLED, LOW);
      return communicatedGameState;
    }
  }
}

GameResult runLDRCover() {
  // LDR - first to cover
  digitalWrite(whiteLED, HIGH);
  
  // Reaction time, this just lets the LED turn on. If the players
  // are reacting faster than this, they're probably cheating anyway.
  delay(150);
  
  // Obtain the initial value of the sensor pin
  int sensorValue = analogRead(LDRPin);
  GameResult gameState = GameTied;
  
  // Keep going until the game is no longer tied.
  while (gameState == GameTied) {
    int newValue = analogRead(LDRPin);

    if (lowerButtonPressed || higherButtonPressed){
      gameState = IncorrectAttack;
    }
    
    // Cheek to see if there is a noticable increase (i.e. less light) over the LED.
    if (newValue <= sensorValue / 2) {
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

