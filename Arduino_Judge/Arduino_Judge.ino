#include <SoftwareSerial.h>
#include <Servo.h>

#define randomByte() random(0x100)

// Game variables
bool amPlayerTwo;

// Digital pin constants
const int lowerButton = 2;
const int higherButton = 3;
const int fourBitLEDs[] = {4, 5, 6, 7};
const int whiteLED = 8;
const int analogLED = 9;
const int piezo = 10;
const int servoPin = 11;
const int serialRX = 12;
const int serialTX = 13;

// Analog pin constants
const int LDRPin = 0;
const int randomPin = 5;

// Communication constants
const byte handshakeInit = 0x64;
const byte handshakeResponse = 0x42;

// Other things
SoftwareSerial arduinoSerial(serialRX, serialTX);
Servo servo;

void setup() {
  // Setup all the pins
  pinMode(lowerButton, INPUT);
  pinMode(higherButton, INPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(fourBitLEDs[i], OUTPUT);
  }
  pinMode(whiteLED, OUTPUT);
  pinMode(analogLED, OUTPUT);
  pinMode(piezo, OUTPUT);
  pinMode(servoPin, OUTPUT);
  
  // Set up serial, random seed, and servo
  arduinoSerial.begin(115200); // About 14 KB/s
  randomSeed(analogRead(randomPin));
  servo.attach(servoPin);
  
  // Handshake; ensures that both Arduinos are in sync
  bool handshakeDone = false;
  while (!handshakeDone) {
    // Flashing a status LED, to indicate we're ready
    digitalWrite(fourBitLEDs[0], !digitalRead(fourBitLEDs[0]));
    
    handshakeDone = tryHandshake();
  }
  
  // Decide on who should be player one
  amPlayerTwo = determinePlayers();

  // Switch off the status LED
  digitalWrite(fourBitLEDs[0], LOW);
  
  // Ready to start the game!
}

bool tryHandshake() {
  // Returns true if handshake was successful;
  // otherwise, returns false.
  arduinoSerial.write(handshakeInit);
  
  for (int i = 0; i < 10; i++) {
    // Wait for a response
    delay(10);
    if (arduinoSerial.available()) {
      byte readByte = arduinoSerial.read();
      
      if (readByte == handshakeInit) {
        // If the other device has sent a handshake,
        // acknowledge it, and start the game
        arduinoSerial.write(handshakeResponse);
        return true;
      } else if (readByte == handshakeResponse) {
        // If the other device has acknowledged a handshake,
        // start the game.
        return true;
      }
    }
  }

  return false;
}

bool determinePlayers() {
  // Returns false if P1, true if P2.
  // The players are guaranteed to have been determined once
  // this function returns, but if there is no response, the
  // Arduino will lock up. This is fine for this project.
  for (;;) {
    byte myRandomNum = randomByte();
    arduinoSerial.write(myRandomNum);
    while (!arduinoSerial.available()) {
      // Wait until we recieve the other Arduino's number
    }
    
    byte otherArduinoRandomNum = arduinoSerial.read();
    if (myRandomNum > otherArduinoRandomNum) {
      // We're P1
      return false;
      
    } else if (myRandomNum < otherArduinoRandomNum) {
      // We're P2
      return true;
    }
    
    // If they're both the same, try again.
  }
}

void loop() {
  // This code tests that the handshake was successful.
  digitalWrite(whiteLED, HIGH);
  digitalWrite(analogLED, amPlayerTwo ? HIGH : LOW);
  delay(60000);
  // TODO: Replace this with the game itself!
}

void setFourBitLEDs(byte value) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(fourBitLEDs[i], (value >> (3 - i)) & 1);
  }
}

