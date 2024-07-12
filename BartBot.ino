#include <Servo.h>

enum States {
  FORWARD = 45,
  BACKWARD = 135,
  TURN = 1,
  STOP = 90
};

States state = States::STOP;

//ULTRASONIC SENSOR VARIABLES
const int echoPin = 2;
const int trigPin = 3;

//MOTOR VARIABLES
const int leftMotor1Pin = 10;
const int leftMotor2Pin = 8;
const int rightMotor1Pin = 9;
const int rightMotor2Pin = 7;
Servo leftMotor1;
Servo leftMotor2;
Servo rightMotor1;
Servo rightMotor2;

float getDistance() { // in cm
  //trigger sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000);
  if(duration == 0) return -1;
  return duration * 0.034 / 2; //Speed of Sound: 0.034 cm per microsecond
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
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //MOTORS
  leftMotor1.attach(leftMotor1Pin);
  rightMotor1.attach(rightMotor1Pin);
}

void loop() {
  float distance = getDistance();
  if(distance > -1.0) {
    if(distance <= 10.0) state = States::BACKWARD;
    Serial.println(distance);
  }

  switch(state) {
    case FORWARD:
      leftMotor1.write(States::FORWARD);
      rightMotor1.write(States::FORWARD);
      break;
    case BACKWARD:
      leftMotor1.write(States::BACKWARD);
      rightMotor1.write(States::BACKWARD);
      delay(2000);
      state = States::STOP;
      break;
    case TURN:
      turnRight(2000);
      state = States::STOP;
      break;
    case STOP:
      leftMotor1.write(States::STOP);
      rightMotor1.write(States::STOP);
      break;
  }
}
