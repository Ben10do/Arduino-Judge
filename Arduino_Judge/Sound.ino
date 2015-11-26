/*
 * Sound.ino
 * ARDUINO_JUDGE
 * 
 * Includes sound effects, and sound-related functions
 */

// Sound frequencies (_s_ means sharp)
const int A_2 = 110;
const int B_4 = 494;
const int E_5 = 659;
const int Gs5 = 831;
const int E_6 = 1319;
const int Fs6 = 1480;
const int Gs6 = 1661;
const int A_6 = 1760;
const int B_6 = 1976;
const int D_7 = 2349;
const int E_7 = 2637;

// Sound Effects
// Many of these take advantage of each Arduino
// having its own piezo, allowing for some polyphony!

void playHandshakeCompleteSFX() {
  // Indicates that the initial handshake was successful.
  tone(piezo, !amPlayerTwo ? B_4 : E_5);
  delay(125);
  tone(piezo, !amPlayerTwo ? E_5 : Gs5);
  delay(125);
  noTone(piezo);
  delay(250);
}

void playCountdownSFX(GameID game, int delayTime) {
  // Plays the countdown sound effect before a microgame starts.
  int pitch = E_7;

  if (amPlayerTwo) {
    switch (game) {
      case PiezoPitch:
        pitch = Gs6;

      case PiezoRhythm:
        pitch = B_6;
  
      case LEDNumber:
        pitch = D_7;
  
      case LEDBrightest:
        pitch = A_6;
  
      case LEDFrequency:
        pitch = Fs6;
  
      case LDRCover:
        pitch = E_6;
    }
  }
  
  for (int i = 0; i < 3; i++) {
    tone(piezo, pitch);
    delay(delayTime);
    noTone(piezo);
    delay(delayTime);
  }

  // Play the final note an octave lower, whilst starting the game!
  tone(piezo, pitch / 2, delayTime);
}

void playCommunicationErrorSFX() {
  // Indicates that a communication error has occured.
  tone(piezo, A_2);
  delay(250);
}

int generateNoteFreq(int n) {
  // Generates the approx. frequency of a musical note
  // n is the note number, where A4 (440Hz) is n = 49.
  // Formula from: en.wikipedia.org/wiki/Piano_key_frequencies

  // NOTE: Including pow() uses an extra 2,000 bytes!
  //       Replace if we run out of space.
  return round(pow(2, (n-49)/12) + 440);
}

