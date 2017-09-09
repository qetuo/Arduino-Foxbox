// Arduino controlled Fox Hunting Xmtr and / or Beacon
// Code cobbled together from various sources
// Pin 6 is the square wave output for the Morse Code
// Pin 7 is Push to Talk pin (low = PTT)

#include "DTMF.h"
#include "pitches.h"

int sensorPin = A0;  //The following block used by DTMF
float n = 128.0;
float sampling_rate = 8926.0;
DTMF dtmf = DTMF(n, sampling_rate);
float d_mags[8];
char thischar;

int xmit = 0;  // if xmit = 0, don't transmit
int pause = 220;  // pause between transmissions, 22 = 1 second (approx)

// notes in the melody:
int melody[] = {
  NOTE_G4, NOTE_E4, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_E4
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  8, 8, 4, 4, 8, 8, 8, 8, 4, 4, 4, 4
};

#define PTT  7
#define RELAY  3

#define BUZZER_PIN 6  // send music and cw out this pin
#define BUZZER_FREQUENCY 700  // cw pitch
char beaconstring[] = "CQ KC9UNS/FOX";
char idstring[] = "DE KC9UNS";
int randNumber;

char chartemp[4] ;
char incomingByte = 0;
//int morseOutput = 4; //Pin for receiving Morse output - attach LED to this pin
int interval = 60;   //length of time for one dot - basic unit of measurement;  one interval occurs between each dot or dash; dash is equivalent to 3 dots; 7 dots between words

String alphabet = "abcdefghijklmnopqrstuvwxyz1234567890/ ";
String ALPHABET = alphabet;

String morseCode[] = {
  ".-", // A
  "-...", // B
  "-.-.", // C
  "-..", // D
  ".", // E
  "..-.", // F
  "--.", // G
  "....", // H
  "..", // I
  ".---", // J
  "-.-", // K
  ".-..", // L
  "--", // M
  "-.", // N
  "---",  // O
  ".--.", // P
  "--.-", // Q
  ".-.", // R
  "...",  // S
  "-", // T
  "..-", // U
  "...-", // V
  ".--", // W
  "-..-", // X
  "-.--", // Y
  "--..", // Z
  ".----", // 1
  "..---", // 2
  "...--", // 3
  "....-", // 4
  ".....", // 5
  "-....", // 6
  "--...", // 7
  "---..", // 8
  "----.", // 9
  "-----", // 0
  "-..-.", // forward slash
  " "   //space character
};

void setup() {

  pinMode(PTT, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(PTT, HIGH);
  Serial.begin(9600);
  //  pinMode(morseOutput, OUTPUT);

  ALPHABET.toUpperCase();
  randomSeed(analogRead(0));  // in case random delays between
  // transmissions are used
}

void loop() {

  for (int x = 0; x < pause; x ++)  // while waiting between transmissions, look for DTMF
  {
    //  Serial.println(x);
    dtmf.sample(sensorPin);
    dtmf.detect(d_mags, 506);
    thischar = dtmf.button(d_mags, 1800.);

    if (thischar) {  // decide what to do if DTMF tone is received
      switch (thischar) {
        case 49:  // the number 1
          xmit = 1;  // set the flag to enable transmissions
          break;
        case 53:  //number 5
          digitalWrite(PTT, LOW);
          delay(2000);
          sendLetter('h');
          sendLetter('i');
          digitalWrite(PTT, HIGH);
          delay(2000);
          break;
        case 54:  //number 6
          digitalWrite(PTT, LOW);
          delay(2000);
          for (int i = 0; i < sizeof(idstring); i++) {
            sendLetter(idstring[i]);
          }
          digitalWrite(PTT, HIGH);
          delay(2000);
          break;
        case 55:  //number 7
          digitalWrite(PTT, LOW);
          delay(2000);
          sendLetter('7');
          sendLetter('3');
          digitalWrite(PTT, HIGH);
          delay(2000);
          break;
        case 56:  //number 8
          digitalWrite(RELAY, LOW);           // Turns ON Relay 1 for antenna 1
          delay(2000);                        // Wait 2 seconds
          digitalWrite(RELAY, HIGH);          // Turns Relay Off
          digitalWrite(PTT, LOW);
          delay(2000);
          sendLetter('b');
          sendLetter('y');
          sendLetter('y');
          sendLetter('o');
          sendLetter('u');
          sendLetter('r');
          sendLetter('c');
          sendLetter('o');
          sendLetter('m');
          sendLetter('m');
          sendLetter('a');
          sendLetter('n');
          sendLetter('d');
          sendLetter('1');
          digitalWrite(PTT, HIGH);
          delay(2000);
          break;
        case 57:  //number 9
          digitalWrite(RELAY, HIGH);          // Turns ON Relays 1 for antenna 2
          delay(2000);                        // Wait 2 seconds
          digitalWrite(RELAY, LOW);           // Turns Relay Off
          digitalWrite(PTT, LOW);
          delay(2000);
          sendLetter('b');
          sendLetter('y');
          sendLetter('y');
          sendLetter('o');
          sendLetter('u');
          sendLetter('r');
          sendLetter('c');
          sendLetter('o');
          sendLetter('m');
          sendLetter('m');
          sendLetter('a');
          sendLetter('n');
          sendLetter('d');
          sendLetter('2');
          digitalWrite(PTT, HIGH);
          delay(2000);
          break;
        default:  // any other number, turn off transmissions
          xmit = 0;  // set the flag to disable transmissions
          break;
      }
    }
    delay(1);
  }

  if (xmit == 1)
  {
    digitalWrite(PTT, LOW);
    delay(2000); // delay 2 seconds after PTT to account for race condition

    // play a little melody
    for (int thisNote = 0; thisNote < 12; thisNote++) {
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(BUZZER_PIN, melody[thisNote], noteDuration);
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(BUZZER_PIN);
    }

    delay(1000);  //one second between melody and beaconstring

    for (int i = 0; i < sizeof(beaconstring); i++) {
      sendLetter(beaconstring[i]);
    }

    delay(500);
    digitalWrite(PTT, HIGH);  // drop the PTT and wait a while
    //  randNumber = random(10000, 20000);  // remove comments and add comments to delay(5000)
    //  Serial.print(randNumber);           // to use random delays
    //  Serial.println();
    //  delay(randNumber);
    //  delay(5000);
  }
}

// End of Loop()
// Functions follow

void sendLetter (char c) {
  int i;
  for (i = 0; i < alphabet.length(); i = i + 1) {
    if (alphabet[i] == c || ALPHABET[i] == c) {
      //Serial.print(c);
      sendMorseCode(morseCode[i]);
      return;
    }
  }
  if (c == '\n')
    Serial.println();
}

void sendMorseCode (String tokens) {
  int i;
  for (i = 0; i < tokens.length(); i = i + 1) {
    switch (tokens[i]) {
      case '-':
        sendDash();
        break;
      case '.':
        sendDot();
        break;
      case ' ':
        sendEndOfWord();
        break;
    }
  }
  morseOutputOff(2);
  //   Serial.print(" ");
}

void sendEndOfWord() {
  morseOutputOff(4);
  //   Serial.print("  ");
}

//basic functions - Morse code concepts
void sendDot() {
  tone(BUZZER_PIN, BUZZER_FREQUENCY);
  morseOutputOn(1);
  noTone(BUZZER_PIN);
  morseOutputOff(1);
  //   Serial.print(".");
}
void sendDash() {
  tone(BUZZER_PIN, BUZZER_FREQUENCY);
  morseOutputOn(3);
  noTone(BUZZER_PIN);
  morseOutputOff(1);
  //   Serial.print("-");
}

//Low level functions - how the actions are accomplished
// n = number of intervals
// interval is a fixed length of time determined at start, for example 200 milliseconds
void morseOutputOn (int n) {
  //   example: morseOutputOn(1) means turn output on and keep it on for 1 interval
  //            morseOutputOn(3) means turn output on and keep it on for 3 intervals
  //
  //   digitalWrite(morseOutput, HIGH);
  delay(n * interval);
}

void morseOutputOff (int n) {
  //   digitalWrite(morseOutput, LOW);
  delay(n * interval);
}

