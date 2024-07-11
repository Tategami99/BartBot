
enum States {
  FORWARD,
  TURN,
  STOP
};

States state = States::STOP;

void turnRight(unsigned long duration) {
  //TODO: move motors in opposite directions
  delay(duration);
  //TODO: stop motors
}

void turnRight(unsigned long duration) {
  //TODO: move motors in opposite directions
  delay(duration);
  //TODO: stop motors
}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  switch(state) {
    case FORWARD:
      break;
    case TURN:
      break;
    case STOP:
      break;
  }
}
