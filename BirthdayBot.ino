#include <LiquidCrystal.h>
#include <SD.h>
#include <TMRpcm.h>
#include <pcmConfig.h>
#include <pcmRF.h>

//SD DEFINITIONS
#define SD_CS_PIN A0

//AUDIO DEFINITIONS & VARIABLES
#define SPEAKER_PIN 10
TMRpcm speaker;
unsigned long musicStartTime = 0;
bool musicPlaying = false;
unsigned int numSongs = 6;
unsigned int currentSong = numSongs;
char* songs[] = {"omg.wav", "superShy.wav", "hypeBoy.wav", "howSweet.wav", "eta.wav", "ditto.wav"};
char* songTitle[] = {"OMG", "Super Shy", "Hype Boy", "How Sweet", "ETA", "Ditto"};

//POTENTIOMETER DEFINITIONS
#define POT_PIN A1
int lastPotValue;
int potDeadband = 5;
int numPotValues = 10;
int potValues[10];
int potIndex = 0;
int potTotal = 0;

//LCD DEFINITIONS
//VSS to GND
//VDD to 5V
//RW to GND
//A to 3.3V
//K to GND
#define LCD_RS 2
#define LCD_EN 3
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
#define LCD_V0 9
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
unsigned int contrastValue = 50;

void setup() {
  Serial.begin(9600);

  //SD CARD
  pinMode(SD_CS_PIN, OUTPUT);
  SD.begin(SD_CS_PIN);
  
  //AUDIO
  speaker.speakerPin = SPEAKER_PIN;
  speaker.quality(1);
  speaker.volume(7);

  //POTENTIOMETER
  for(int i = 0; i < numPotValues; i++) {
    int value = analogRead(POT_PIN);
    potValues[i] = value;
    potTotal += value;
  }
  lastPotValue = potTotal / numPotValues;

  //LCD
  pinMode(LCD_V0, OUTPUT);  // Use PWM pin
  analogWrite(9, contrastValue);  // Adjust value (0-255) for best contrast
  lcd.begin(16, 2);
  lcd.print("ON");
}

void loop() {
  delay(100);
  int potValue = updatePotentiometer();
  unsigned long mappedValue = mapToSong(potValue, 0, 1020, 0, numSongs);

  if(mappedValue == numSongs) {
    speaker.stopPlayback();
    return;
  }

  if(potValue < lastPotValue - potDeadband || potValue > lastPotValue + potDeadband) {
    currentSong = mappedValue;
    lastPotValue = potValue;
    playAudioFile();
  } else if (!speaker.isPlaying()) {
    currentSong = (currentSong + 1) % numSongs;
    playAudioFile();
  }

  updateDisplay();
}

int updatePotentiometer() {
  potTotal -= potValues[potIndex];
  potValues[potIndex] = analogRead(POT_PIN);
  potTotal += potValues[potIndex];
  potIndex = (potIndex + 1) % numPotValues;
  return potTotal / numPotValues;
}

unsigned long mapToSong(long x, long in_min, long in_max, long out_min, long out_max) {
  if(x > in_max) return out_max;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void playAudioFile() {
  speaker.stopPlayback();
  musicStartTime = millis();
  musicPlaying = true;

  speaker.play(songs[currentSong]);

  lcd.clear();
}

void updateDisplay() {
  unsigned long elapsedTime = millis() - musicStartTime;
  unsigned long minutes = elapsedTime / 60000;
  unsigned long seconds = (elapsedTime % 60000) / 1000;

  lcd.clear();

  if(!speaker.isPlaying()) {
    musicPlaying = false;
    lcd.println("Happy Birthday");
    lcd.print("Ewan!");
  }

  lcd.println(songTitle[currentSong]);
  
  char formattedTime[6]; // M:SS\0
  sprintf(formattedTime, "%lu:%02lu", minutes, seconds);
  lcd.print(formattedTime);
}