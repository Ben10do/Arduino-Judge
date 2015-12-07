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

GameResult runPiezoPitch(byte myNumber, byte otherNumber) {
  updateLCD(F("Attack if your"), 0);
  updateLCD(F("note is higher!"), 1);
  
  //Game variables.
  GameResult gameState = GameTied;

  tone(piezo, generateNoteFreq(myNumber + 40));
  
  //Keep going until the game is no longer tied.
  while (gameState == GameTied) {
    
    if (lowerButtonPressed) {
      Serial.println("lowerButtonPressed");
      //Was the player correct to press the lower button.
      if (myNumber < otherNumber) {
        //Yes.
        gameState = CorrectDodge;
      } else {
        //No.
        gameState = IncorrectDodge;
      }
    } else if (higherButtonPressed) {
      Serial.println("higherButtonPressed");
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
      Serial.println("Exit loop");
      noTone(piezo);
      return communicatedGameState;
    }
  }
}

GameResult runPiezoRhythm(byte myNumber, byte otherNumber) {
  updateLCD(F("Attack if your"), 0);
  updateLCD(F("rhythm's faster!"), 1);
  
  //Game variables.
  GameResult gameState = GameTied;
  int period = (myNumber+1)*200;
  
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
    } else {
    }
  }
}

GameResult runLEDNumber(byte myNumber, byte otherNumber) {
  updateLCD(F("Attack if your"), 0);
  updateLCD(F("number's higher!"), 1);
  
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
  updateLCD(F("Attack if your"), 0);
  updateLCD(F("LED's brighter!"), 1);
  
  // LED - brightest
  int brightness = 10 + (myNumber * 10);
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
  updateLCD(F("Attack if your"), 0);
  updateLCD(F("LED's faster!"), 1);
  
  //Game variables.
  GameResult gameState = GameTied;
  int period = (myNumber+1)*30;
  
  //Keep going until the game is no longer tied.
  while (gameState == GameTied) {

    //Work out if the LED should be on or off.
    int timeSinceGameStart = millis() - millisAtGameStart;
    int x =  timeSinceGameStart/period; //TODO: Name this better.
    if (x%2 == 0) {
      digitalWrite(analogLED, HIGH);
    } else {
      digitalWrite(analogLED, LOW);
    }
    if (lowerButtonPressed) {
      //Was the player correct to press the lower button.
      if (myNumber > otherNumber) {
        //Yes.
        gameState = CorrectDodge;
      } else {
        //No.
        gameState = IncorrectDodge;
      }
    } else if(higherButtonPressed) {
      //Was the player correct to press the higher button.
      if (myNumber > otherNumber) {
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
  updateLCD(F("Quick! Cover"), 0);
  updateLCD(F("the LDR!"), 1);
  
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
    Serial.println("Goin' loopy");
    int newValue = analogRead(LDRPin);

    if (lowerButtonPressed || higherButtonPressed){
      gameState = IncorrectAttack;
    }
    
    Serial.println(sensorValue);
    Serial.println(newValue);
    Serial.println();
    
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

