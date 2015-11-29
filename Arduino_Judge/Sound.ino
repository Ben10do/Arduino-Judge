/*
 * Sound.ino
 * ARDUINO_JUDGE
 * 
 * Includes sound effects, and sound-related functions
 */

// Sound frequencies (_s_ means sharp)
const int A_2 = 110;
const int Fs4 = 370;
const int Gs4 = 415;
const int A_4 = 440;
const int B_4 = 494;
const int Cs5 = 554;
const int Ds5 = 622;
const int E_5 = 659;
const int Fs5 = 740;
const int Gs5 = 831;
const int A_5 = 880;
const int B_5 = 988;
const int Cs6 = 1109;
const int Ds6 = 1245;
const int E_6 = 1319;
const int Fs6 = 1480;
const int Gs6 = 1661;
const int A_6 = 1760;
const int B_6 = 1976;
const int Cs7 = 2217;
const int D_7 = 2349;
const int Ds7 = 2489;
const int E_7 = 2637;
const int A_8 = 7040;

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

void playCountdownSFX(int delayTime) {
  // Plays the countdown sound effect before a microgame starts.
  int pitch = E_7;

  if (amPlayerTwo) {
    switch (random(6)) {
      case 0:
        pitch = E_6;
        break;
      
      case 1:
        pitch = Fs6;
        break;
      
      case 2:
        pitch = Gs6;
        break;
      
      case 3:
        pitch = A_6;
        break;
      
      case 4:
        pitch = B_6;
        break;
  
      case 5:
        pitch = D_7;
        break;
    }
  }
  
  for (int i = 0; i < 3; i++) {
    tone(piezo, pitch, 40);
    delay(delayTime);
    noTone(piezo);
    delay(delayTime);
  }

  // Play the final note an octave lower, whilst starting the game!
  tone(piezo, pitch / 2, 40);
}

void playInstantOfVictorySFX(bool didWin) {
  // Played the moment the servo hits ±80.
  if (didWin) {
    for (int pitch = A_4; pitch <= A_8; pitch *= 2) {
      tone(piezo, pitch);
      delay(20);
    }
    noTone(piezo);
    
  } else {
    delay(5 * 20);
  }

  delay(750);
}

void playVictoryJingleSFX(bool didWin) {
  // A jingle to celebrate the victory!
  if (didWin) {
    int notes[] = {E_5, B_5, A_5, E_6, B_5, E_5};

    for (int i = 0; i < 4; i++) {
      tone(piezo, notes[i], 100);
      delay(200);
      tone(piezo, notes[i]);
      delay(100);
    }

    tone(piezo, notes[4], 100);
    delay(200);
    tone(piezo, notes[5]);
    delay(100);

    // Cheeky vibrato effect
    int vibratoOffset = 0;
    for (int i = 0; i <= 28; i++) {
      tone(piezo, notes[5] + vibratoOffset);
      vibratoOffset = round(sin(i) * 6);
      delay(25);
    }
    
  } else {
    delay(1400);
    tone(piezo, Fs4);
    delay(800);
  }

  noTone(piezo);
  delay(850);
}

void playGameOverSFX() {
  // For the game's conclusion.
  int notes[8];

  // Setting the notes; this is the easiest way without
  // modifying each element one-by-one.
  if (!amPlayerTwo) {
    int notesToCopy[] = {Gs5, Fs5, E_5, Ds5, Cs5, B_4, A_4, Gs4};
    memcpy(&notes, &notesToCopy, sizeof(int) * 8);
    
  } else {
    int notesToCopy[] = {E_6, Ds6, Cs6, B_5, A_5, Gs5, Fs5, E_5};
    memcpy(&notes, &notesToCopy, sizeof(int) * 8);
  }

  for (int i = 0; i < 5; i++) {
    tone(piezo, notes[i]);
    delay(130);
  }

  for (int i = 5; i < 8; i++) {
    tone(piezo, notes[i]);
    delay(260);
    noTone(piezo);
    delay(260);
  }
}

void playCommunicationErrorSFX() {
  // Indicates that a communication error has occured.
  tone(piezo, A_2);
  delay(250);
  noTone(piezo);
}

int generateNoteFreq(int n) {
  // Generates the approx. frequency of a musical note
  // n is the note number, where A4 (440Hz) is n = 49.
  // Formula from: en.wikipedia.org/wiki/Piano_key_frequencies

  // NOTE: Including pow() uses an extra 2,000 bytes!
  //       Replace if we run out of space.
  return round(pow(2, (n-49)/12) + 440);
}

