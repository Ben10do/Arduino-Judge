/*
 * Microgames.ino
 * ARDUINO_JUDGE
 * 
 * Includes the code for the individual microgames.
 */

GameResult runMicrogame(GameID game, byte myNumber, byte otherNumber) {
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
      // TODO: Handle unexpected Game ID
      return IncorrectDodge;
  }
}

GameResult runPiezoPitch(byte myNumber, byte otherNumber) {
  // Piezo - highest note
  // TODO: Must write this microgame!
  return CorrectAttack;
}

GameResult runPiezoRhythm(byte myNumber, byte otherNumber) {
  // Piezo - fastest rhythm
  // TODO: Must write this microgame!
  return CorrectAttack;
}

GameResult runLEDNumber(byte myNumber, byte otherNumber) {
  // LED - 4-bit highest number
  // TODO: Must write this microgame!
  return CorrectAttack;
}

GameResult runLEDBrightest(byte myNumber, byte otherNumber) {
  // LED - brightest
  // TODO: Must write this microgame!
  return CorrectAttack;
}

GameResult runLEDFrequency(byte myNumber, byte otherNumber) {
  // LED - fastest frequency
  // TODO: Must write this microgame!
  return CorrectAttack;
}

GameResult runLDRCover() {
  // LDR - first to cover
  // TODO: Must write this microgame!
  return CorrectAttack;
}

