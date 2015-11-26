/*
 * Microgames.ino
 * ARDUINO_JUDGE
 * 
 * Includes the code for the individual microgames.
 */

GameResult runMicrogame(GameID game, byte myNumber, byte otherNumber) {
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
      // TODO: Should this have a return statement?
      // It'll never be reached!
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
  // TODO: Must write this microgame!
  return GameTied;
}

GameResult runLEDBrightest(byte myNumber, byte otherNumber) {
  // LED - brightest
  // TODO: Must write this microgame!
  return GameTied;
}

GameResult runLEDFrequency(byte myNumber, byte otherNumber) {
  // LED - fastest frequency
  // TODO: Must write this microgame!
  return GameTied;
}

GameResult runLDRCover() {
  // LDR - first to cover
  digitalWrite(whiteLED,HIGH);
  delay(100);
  int sensorValue = analogRead(LDRPin);
  GameResult gameState = GameTied;
  while (gameState == GameTied){
    //Darkness is a high value, well lit is a low value.
    newValue = analogRead(LDRPin);
    if (newValue>=sensorValue + 300 || newValue==1023){
      millisAtButtonPress = millis(); // Added this, since it's necessary for resolving conflicts!
      gameState = CorrectAttack;
    }
    GameResult communicatedGameState = communicateGameStatus(gameState);
    if (communicatedGameState != GameTied){
      return communicatedGameState;
    }
  }
}

