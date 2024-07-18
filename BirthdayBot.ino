#include <LiquidCrystal.h>
#include <Servo.h>
#include <SD.h>
#include <TMRpcm.h>
#include <pcmConfig.h>
#include <pcmRF.h>

enum Songs {
  HAPPY_BIRTHDAY = 130000,
  OMG = 140000,
  ITS_NOT_LIKE_I_LIKE_YOU = 150000
};
Songs song = Songs::HAPPY_BIRTHDAY;

//SD DEFINITIONS
#define SD_CS_PIN A0

//AUDIO DEFINITIONS & VARIABLES
#define SPEAKER_PIN 10
TMRpcm speaker;
unsigned long musicStartTime = 0;
bool musicPlaying = false;
unsigned long currentSongDuration = 0;

//SERVO DEFINITIONS & VARIABLES
#define SERVO_PIN 9
Servo arm;

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
  speaker.quality(0);

  //SERVO
  arm.attach(SERVO_PIN);

  //LCD
  lcd.begin(16, 2);
  lcd.print("ON");
}

void loop() {
  //TODO: adjust volume for specific songs 
  switch(song) {
    case HAPPY_BIRTHDAY:
      delay(1000);
      lcd.clear();
      lcd.print("Happy Birthday!");
      playAudioFile("happy_birthday.mp3", Songs::HAPPY_BIRTHDAY, 5);
      break;
    case OMG:
      delay(1000);
      lcd.clear();
      lcd.print("OMG");
      playAudioFile("omg.mp3", Songs::OMG, 5);
      break;
    case ITS_NOT_LIKE_I_LIKE_YOU:
      delay(1000);
      lcd.clear();
      lcd.println("It's Not Like I");
      lcd.print("Like You");
      playAudioFile("its_not_like_i_like_you.mp3", Songs::ITS_NOT_LIKE_I_LIKE_YOU, 5);
      break;
  }

  updateArm(100);
}

void playAudioFile(const char* filename, unsigned long duration, unsigned int volume) {
  speaker.stopPlayback();
  arm.write(0);
  currentSongDuration = duration;
  musicStartTime = millis();
  musicPlaying = true;

  speaker.setVolume(volume);
  speaker.play(filename);
}

void updateArm(unsigned long updateDelay) {
  unsigned long elapsedTime = millis() - musicStartTime;

  int angle = map(elapsedTime, 0, currentSongDuration, 0, 180);
  arm.write(angle);

  if(!speaker.isPlaying()) {
    musicPlaying = false;
    arm.write(180);
  }

  delay(updateDelay);
}