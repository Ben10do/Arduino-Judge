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

// Sound Effects
// Many of these take advantage of each Arduino
// having its own piezo, allowing for some polyphony!

void playHandshakeCompleteSFX() {
  tone(piezo, !amPlayerTwo ? B_4 : E_5);
  delay(125);
  tone(piezo, !amPlayerTwo ? E_5 : Gs5);
  delay(125);
  noTone(piezo);
}

void playCommunicationErrorSFX() {
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
