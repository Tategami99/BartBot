#include <LiquidCrystal.h>
#include <SD.h>
#include <TMRpcm.h>
#include <pcmConfig.h>
#include <pcmRF.h>

//SD DEFINITIONS
//GND to GND
//VCC to 5V
//MISO to 12
//MOSI to 11
//SCK to 13
#define SD_CS_PIN A0

//AUDIO DEFINITIONS & VARIABLES
//other to GND
#define SPEAKER_PIN 10
TMRpcm speaker;
unsigned long musicStartTime = 0;
const unsigned int numSongs = 6;
unsigned int currentSong = 0;
unsigned int songsPlayedWithoutChange = 0;
const char* songs[] = {"omg.wav", "superShy.wav", "hypeBoy.wav", "howSweet.wav", "eta.wav", "ditto.wav"};
const char* songTitle[] = {
  "OMG             ", 
  "Super Shy       ", 
  "Hype Boy        ", 
  "How Sweet       ", 
  "ETA             ", 
  "Ditto            "
  };

//POTENTIOMETER DEFINITIONS
//1 edge to 5V
//1 edge to GND
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
#define LCD_D4 7
#define LCD_D5 6
#define LCD_D6 5
#define LCD_D7 4
#define LCD_V0 9
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
const unsigned int contrastValue = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 3000;

void setup() {
  Serial.begin(9600);

  //SD CARD
  pinMode(SD_CS_PIN, OUTPUT);
  SD.begin(SD_CS_PIN);
  
  //AUDIO
  speaker.speakerPin = SPEAKER_PIN;
  speaker.quality(1);
  speaker.volume(0);

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
  lcd.setCursor(0, 0);
}

void loop() {
  int potValue = updatePotentiometer();
  unsigned long mappedValue = mapToSong(potValue, 0, 1020, 0, numSongs);

  if(mappedValue == numSongs) {
    currentSong = numSongs;
    speaker.stopPlayback();
  }
  else if(potValue < lastPotValue - potDeadband || potValue > lastPotValue + potDeadband) {
    speaker.stopPlayback();
    currentSong = mappedValue;
    lastPotValue = potValue;
    songsPlayedWithoutChange = 0;
  } else if (!speaker.isPlaying()) {
    if(songsPlayedWithoutChange >= 1) currentSong = (currentSong + 1) % numSongs;
    playAudioFile();
  }

  unsigned long currentMillis = millis();
  if(millis() - previousMillis >= interval && speaker.isPlaying()) {
    previousMillis = currentMillis;
    updateDisplay(currentMillis);
  }
  else if(millis() - previousMillis >= interval) {
    previousMillis = currentMillis;
    lcd.setCursor(0, 0);
    lcd.print(F("HBD Ewan!       "));
    lcd.setCursor(0, 1);
    lcd.print(F("                "));
  }
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
  songsPlayedWithoutChange++;

  speaker.play(songs[currentSong]);
  lcd.clear();
}

void updateDisplay(unsigned long currentMillis) {
  unsigned long elapsedTime = currentMillis - musicStartTime;

  if(elapsedTime < 2000) return;

  unsigned long minutes = elapsedTime / 60000;
  unsigned long seconds = (elapsedTime % 60000) / 1000;

  lcd.setCursor(0, 0);
  lcd.print(songTitle[currentSong]);

  lcd.setCursor(0, 1);
  lcd.print(minutes);
  lcd.print(F(":"));
  if(seconds < 10) lcd.print(F("0"));
  lcd.print(seconds);
  lcd.print(F("            "));
}