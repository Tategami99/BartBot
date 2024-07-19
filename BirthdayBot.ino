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
unsigned long potOffset = 1;

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

  //LCD
  lcd.begin(16, 2);
  lcd.print("ON");
}

void loop() {
  delay(100);
  int potValue = analogRead(POT_PIN);
  unsigned long mappedValue = mapToSong(potValue, 0, 1020, 0, numSongs);
  Serial.print(potValue);
  Serial.print(" : ");
  Serial.println(mappedValue);

  if(mappedValue == numSongs) {
    speaker.stopPlayback();
    return;
  }

  if(mappedValue != currentSong) {
    currentSong = mappedValue;
    playAudioFile();
  } else if (!speaker.isPlaying()) {
    currentSong = (currentSong + 1) % numSongs;
    playAudioFile();
  }

  updateArm();
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
  arm.write(angle);

  if(!speaker.isPlaying()) {
    musicPlaying = false;
    arm.write(180);
  }
}