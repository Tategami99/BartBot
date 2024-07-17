#include <SD.h>
#include <TMRpcm.h>
#include <pcmConfig.h>
#include <pcmRF.h>

enum Songs {
  HAPPY_BIRTHDAY,
  OMG,
  ITS_NOT_LIKE_I_LIKE_YOU
};
Songs song = Songs::HAPPY_BIRTHDAY;

//SD DEFINITIONS
#define SD_CS_PIN 2

//DEFINITIONS & VARIABLES
#define SPEAKER_PIN 3
TMRpcm tmrpcm;

void setup() {
  Serial.begin(9600);

  //SD CARD
  SD.begin(SD_CS_PIN);
  
  //AUDIO
  tmrpcm.speakerPin = SPEAKER_PIN;
  tmrpcm.quality(0);
}

void loop() {
  //TODO: adjust volume for specific songs 
  switch(song) {
    case HAPPY_BIRTHDAY:
      tmrpcm.stopPlayback();
      tmrpcm.setVolume(5);
      tmrpcm.play("happy_birthday.mp3");
      break;
    case OMG:
      tmrpcm.stopPlayback();
      tmrpcm.setVolume(5);
      tmrpcm.play("omg.mp3");
      break;
    case ITS_NOT_LIKE_I_LIKE_YOU:
      tmrpcm.stopPlayback();
      tmrpcm.setVolume(5);
      tmrpcm.play("its_not_like_i_like_you.mp3");
      break;
  }
}