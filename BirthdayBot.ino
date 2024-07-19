#include <LiquidCrystal.h>
#include <Servo.h>
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
unsigned long songLength[] = {93000, 155000, 179000, 219000, 151000, 186000};

//SERVO DEFINITIONS & VARIABLES
#define SERVO_PIN 9
Servo arm;

//POTENTIOMETER DEFINITIONS
#define POT_PIN A1
int lastPotValue;
int potDeadband = 5;
int numPotValues = 10;
int potValues[10];
int potIndex = 0;
int potTotal = 0;

//LCD DEFINITIONS
#define LCD_RS 2
#define LCD_EN 3
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
  Serial.begin(9600);

  //SD CARD
  pinMode(SD_CS_PIN, OUTPUT);
  SD.begin(SD_CS_PIN);
  
  //AUDIO
  speaker.speakerPin = SPEAKER_PIN;
  speaker.quality(1);
  speaker.volume(7);

  //SERVO
  arm.attach(SERVO_PIN);

  //POTENTIOMETER
  for(int i = 0; i < numPotValues; i++) {
    int value = analogRead(POT_PIN);
    potValues[i] = value;
    potTotal += value;
  }
  lastPotValue = potTotal / numPotValues;

  //LCD
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

  updateArm();
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
  arm.write(0);
  musicStartTime = millis();
  musicPlaying = true;

  speaker.play(songs[currentSong]);

  lcd.clear();
}

void updateArm() {
  unsigned long elapsedTime = millis() - musicStartTime;

  int angle = map(elapsedTime, 0, songLength[currentSong], 0, 180);
  Serial.println(angle);
  arm.write(angle);

  if(!speaker.isPlaying()) {
    musicPlaying = false;
    arm.write(180);
  }
}