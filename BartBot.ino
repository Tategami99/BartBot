#include <Servo.h>
#include <SD.h>
#include <TMRpcm.h>
#include <pcmConfig.h>
#include <pcmRF.h>

enum Songs {
  HAPPY_BIRTHDAY = 30,
  OMG = 60,
  ITS_NOT_LIKE_I_LIKE_YOU = 90
};
Songs song = Songs::HAPPY_BIRTHDAY;

//SD DEFINITIONS
#define SD_CS_PIN 2

//AUDIO DEFINITIONS & VARIABLES
#define SPEAKER_PIN 3
TMRpcm tmrpcm;

//SERVO DEFINITIONS & VARIABLES
#define SERVO_PIN 4
Servo arm;

void setup() {
  Serial.begin(9600);

  //SD CARD
  SD.begin(SD_CS_PIN);
  
  //AUDIO
  tmrpcm.speakerPin = SPEAKER_PIN;
  tmrpcm.quality(0);

  //SERVO
  arm.attach(SERVO_PIN);
}

void loop() {
  //TODO: adjust volume for specific songs 
  switch(song) {
    case HAPPY_BIRTHDAY:
      arm.write(Songs::HAPPY_BIRTHDAY);
      delay(1000);
      tmrpcm.stopPlayback();
      tmrpcm.setVolume(5);
      tmrpcm.play("happy_birthday.mp3");
      break;
    case OMG:
      arm.write(Songs::OMG);
      delay(1000);
      tmrpcm.stopPlayback();
      tmrpcm.setVolume(5);
      tmrpcm.play("omg.mp3");
      break;
    case ITS_NOT_LIKE_I_LIKE_YOU:
      arm.write(Songs::ITS_NOT_LIKE_I_LIKE_YOU);
      delay(1000);
      tmrpcm.stopPlayback();
      tmrpcm.setVolume(5);
      tmrpcm.play("its_not_like_i_like_you.mp3");
      break;
  }
}