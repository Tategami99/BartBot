#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

enum States {
  FORWARD = 45,
  BACKWARD = 135,
  TURN = 1,
  STOP = 90
};

States state = States::TURN;

//ULTRASONIC SENSOR DEFINITIONS
#define ECHO_PIN 2
#define TRIG_PIN 3

//RFID SENSOR VARIABLES & DEFINITIONS
#define RST_PIN 4
#define IRQ_PIN 5
#define MISO_PIN 6
#define MOSI_PIN 7
#define SCK_PIN 8
#define SDA_PIN 9
MFRC522 mfrc522(SDA_PIN, RST_PIN);
// byte targetUID[];
const unsigned long detectionInterval = 5000; //5 seconds

//MOTOR VARIABLES & DEFINITIONS
#define LEFT_MOTOR_1_PIN 13
#define LEFT_MOTOR_2_PIN 11
#define RIGHT_MOTOR_1_PIN 12
#define RIGHT_MOTOR_2_PIN 10
Servo leftMotor1;
Servo leftMotor2;
Servo rightMotor1;
Servo rightMotor2;

float getDistance() { // in cm
  //trigger sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 20000);
  if(duration == 0) return -1;
  return duration * 0.034 / 2; //Speed of Sound: 0.034 cm per microsecond
}

// bool matchesUID(byte* readUID, byte length) {

// }

String readUID(byte* uidByte, byte length) {
  String uid = "";
  for(byte i = 0; i < length; i++) {
    if(uidByte[i] < 0x10) {
      uid += "0";
    }
    uid += String(uidByte[i], HEX);
  }
  uid.toUpperCase();
  return uid;
}

int getRFIDReadCount() {
  unsigned long startTime = millis();
  int readCount = 0;

  while(millis() - startTime < detectionInterval) {
    if(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      readCount++;
      String uid = readUID(mfrc522.uid.uidByte, mfrc522.uid.size);
      Serial.println("UID: " + uid);
    }
    mfrc522.PICC_HaltA();
  }

  return readCount;
}

void turnRight(unsigned long duration) {
  leftMotor1.write(States::FORWARD);
  rightMotor1.write(States::BACKWARD);
  delay(duration);
  leftMotor1.write(States::STOP);
  rightMotor1.write(States::STOP);
}

void turnLeft(unsigned long duration) {
  leftMotor1.write(States::BACKWARD);
  rightMotor1.write(States::FORWARD);
  delay(duration);
  leftMotor1.write(States::STOP);
  rightMotor1.write(States::STOP);
}

void setup() {
  //TESTING
  Serial.begin(9600);

  //ULTRASONIC SENSOR
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  //RFID SENSOR
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RFID reader intialized");

  //MOTORS
  leftMotor1.attach(LEFT_MOTOR_1_PIN);
  rightMotor1.attach(RIGHT_MOTOR_1_PIN);
}

void loop() {
  //RFID DETECTION

  //DISTANCE DETECTION
  float distance = getDistance();
  if(distance > -1.0) {
    if(distance <= 10.0) state = States::BACKWARD;
    Serial.println(distance);
  }

  //MOVEMENT
  switch(state) {
    case FORWARD:
      leftMotor1.write(States::FORWARD);
      rightMotor1.write(States::FORWARD);
      break;
    case BACKWARD:
      leftMotor1.write(States::BACKWARD);
      rightMotor1.write(States::BACKWARD);
      delay(2000);
      state = States::TURN;
      break;
    case TURN:
      int count = getRFIDReadCount();
      Serial.println(count);
      // state = States::FORWARD;
      break;
    case STOP:
      leftMotor1.write(States::STOP);
      rightMotor1.write(States::STOP);
      break;
  }
}
