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
  //Game variables.
  GameResult gameState = GameTied;

  tone(piezo, generateNoteFreq(myNumber) + 52);
  
  //Keep going until the game is no longer tied.
  while (gameState == GameTied) {
    
    if (lowerButtonPressed) {
      //Was the player correct to press the lower button.
      if (myNumber < otherNumber) {
        //Yes.
        gameState = CorrectDodge;
      } else {
        //No.
        gameState = IncorrectDodge;
      }
    } else if(higherButtonPressed) {
      //Was the player correct to press the higher button.
      if (myNumber < otherNumber) {
        //No.
        gameState = IncorrectAttack;
      } else {
        //Yes.
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

GameResult runPiezoRhythm(byte myNumber, byte otherNumber) {
  //Game variables.
  GameResult gameState = GameTied;
  int Period = (myNumber+1)*200;
  
  //Keep going until the game is no longer tied.
  while (gameState == GameTied) {

    //Work out if the pizeo should be on or off.
    int timeSinceGameStart = millis() - millisAtGameStart;
    int x =  timeSinceGameStart/2; //TODO: Name this better.
    if (x%2 == 0) {
      tone(piezo, 988); // B_5
    } else {
      noTone(piezo);
    }
    if (lowerButtonPressed) {
      //Was the player correct to press the lower button.
      if (myNumber < otherNumber) {
        //Yes.
        gameState = CorrectDodge;
      } else {
        //No.
        gameState = IncorrectDodge;
      }
    } else if(higherButtonPressed) {
      //Was the player correct to press the higher button.
      if (myNumber < otherNumber) {
        //No.
        gameState = IncorrectAttack;
      } else {
        //Yes.
        gameState = CorrectAttack;
      }
    }
    
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
  //Game variables.
  GameResult gameState = GameTied;
  int Period = (myNumber+1)*200;
  
  //Keep going until the game is no longer tied.
  while (gameState == GameTied) {

    //Work out if the LED should be on or off.
    int timeSinceGameStart = millis() - millisAtGameStart;
    int x =  timeSinceGameStart/2; //TODO: Name this better.
    if (x%2 == 0) {
      digitalWrite(analogLED, HIGH);
    } else {
      digitalWrite(analogLED, LOW);
    }
    if (lowerButtonPressed) {
      //Was the player correct to press the lower button.
      if (myNumber < otherNumber) {
        //Yes.
        gameState = CorrectDodge;
      } else {
        //No.
        gameState = IncorrectDodge;
      }
    } else if(higherButtonPressed) {
      //Was the player correct to press the higher button.
      if (myNumber < otherNumber) {
        //No.
        gameState = IncorrectAttack;
      } else {
        //Yes.
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

