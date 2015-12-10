/*
 * Sound.ino
 * ARDUINO_JUDGE
 * 
 * Includes sound effects, and sound-related functions
 */

// Sound frequencies (_s_ means sharp)
const int A_2 = 110;
const int Ds3 = 156;
const int E_3 = 165;
const int A_3 = 220;
const int Ds4 = 311;
const int E_4 = 330;
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
const int D_7 = 2349;
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
  delay(750);
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

void playScoreSFX(bool didWin) {
  // Plays a sound effect about who won the point
  int pitches[] = {A_4, E_5};
  
  if (didWin && !amPlayerTwo) {
    // P1 (me) won
    pitches[0] = A_4;
    pitches[1] = E_5;

  } else if (!didWin && amPlayerTwo) {
    // P1 (other player) won
    pitches[0] = E_4;
    pitches[1] = A_3;
    
  } else if (didWin && amPlayerTwo) {
    // P2 (me) won
    pitches[0] = E_5;
    pitches[1] = B_5;
    
  } else if (!didWin && !amPlayerTwo) {
    // P2 (other player) won
    pitches[0] = B_4;
    pitches[1] = E_4;
  }

  tone(piezo, pitches[0]);
  delay(90);
  tone(piezo, pitches[1]);
  delay(90);
  noTone(piezo);
  delay(500);
}

void playCorrectAttackSFX(bool didAttack) {
  // Played if you rightfully attack.
  if (didAttack) {
    tone(piezo, E_5);
    delay(30);

    for (int pitch = E_5; pitch >= E_4; pitch -= 15) {
      tone(piezo, pitch);
      delay(1); // Takes 23ms
    }

    tone(piezo, E_4, 60);
    delay(127);
    
  } else {
    tone(piezo, E_7, 30);
    delay(60);
    
    tone(piezo, E_3);
    delay(120);
  }

  noTone(piezo);
  delay(700);
  playScoreSFX(didAttack);
}

void playIncorrectAttackSFX(bool didAttack) {
  if (didAttack) {
    tone(piezo, E_5);
    delay(30);


    for (int pitch = E_5; pitch >= Ds3; pitch -= 5) {
      // 102ms
      tone(piezo, pitch);
      delay(1);
    }

    tone(piezo, Ds3);
    delay(152);
    
  } else {
    tone(piezo, E_7, 30);
    delay(60);
    
    tone(piezo, E_3, 120);
    delay(224);
    
  }
  
  noTone(piezo);
  delay(700);
  playScoreSFX(!didAttack);
}

void playCorrectDodgeSFX(bool didDodge) {
  // Played when you rightfully dodge.
  if (didDodge) {
    // Play SFX
    tone(piezo, E_5);
    delay(30);

    // Slide down to B_4
    for (int pitch = E_5; pitch >= B_4; pitch -= 5) {
      tone(piezo, pitch);
      delay(1);
    }

    delay(40);

    // Slide back up to E_5
    for (int pitch = B_4; pitch <= E_5; pitch += 5) {
      tone(piezo, pitch);
      delay(1);
    }

    delay(50);
    noTone(piezo);
    
  } else {
    delay(188);
  }

  delay(700);
  playScoreSFX(didDodge);
}

void playIncorrectDodgeSFX(bool didDodge) {
  // Played when you dodge, but could've attacked!
  if (didDodge) {
    // Play SFX
    tone(piezo, E_5);
    delay(30);

    // Slide down to B_5
    for (int pitch = E_5; pitch >= B_4; pitch -= 4) {
      tone(piezo, pitch);
      delay(1); // 42ms
    }

    delay(40);

    // Slide down further to exaggerate that you've messed up
    for (int pitch = B_4; pitch >= Ds4; pitch--) {
      tone(piezo, pitch);
      delay(1); // 183ms
    }

    delay(30);

  } else {
    // Wait for the other player to finish doing what on
    // earth they're doing
    delay(335);
  }

  noTone(piezo);
  delay(700);
  playScoreSFX(!didDodge);
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
    // Let the winner have their glory
    delay(100);
  }

  delay(750);
}

void playVictoryJingleSFX(bool didWin) {
  // A jingle to celebrate the victory!
  if (didWin) {
    int notes[] = {E_5, B_5, A_5, E_6, B_5, E_5};

    for (int i = 0; i < 4; i++) {
      // Play the first four notes
      tone(piezo, notes[i], 100);
      delay(200);
      tone(piezo, notes[i]);
      delay(100);
    }

    // Play the last two notes with a different rhythm
    tone(piezo, notes[4], 100);
    delay(200);
    tone(piezo, notes[5]);
    delay(100);

    // Adds some vibrato to the last note!
    int vibratoOffset = 0;
    for (int i = 0; i <= 28; i++) {
      tone(piezo, notes[5] + vibratoOffset);
      vibratoOffset = round(sin(i) * 6);
      delay(25);
    }
    
  } else {
    // Harmonise with the winner's last note
    // (but don't go all vibrato-y about it)
    delay(1400);
    tone(piezo, Fs4);
    delay(825);
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
  return round(pow(2, (n-49.0)/12.0) * 440.0);
}

