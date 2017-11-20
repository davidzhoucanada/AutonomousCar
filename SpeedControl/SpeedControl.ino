#include <SoftwareSerial.h> 
#include <string.h>
#include <stdlib.h>

SoftwareSerial bluetoothConnection(12, 13); // RX, TX

#define IN1 6        // L298N Backward (blue)
#define IN2 3        // L298N Forward  (green)
#define IN3 4        // L298N Left (yellow)
#define IN4 5        // L298N Right (orange)

#define FORWARD '0'
#define BACKWARD '1'
#define LEFT '2'
#define RIGHT '3'
#define STRAIGHT '4'
#define STOP '9'

String fullMessageReceivedFromApp;
char characterReceivedFromApp;

char leftOrRight;
char forwardOrBackward;

String speedString;
int speed = 999;

void _stop(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void moveForwardLeft(){
  digitalWrite(IN1, LOW);
  analogWrite(IN2, speed);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void moveForwardRight(){
  digitalWrite(IN1, LOW);
  analogWrite(IN2, speed);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(50);
}
void moveForward(){
  digitalWrite(IN1, LOW);
  analogWrite(IN2, speed);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void moveBackward(){
  analogWrite(IN1, speed);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void moveBackwardLeft(){
  analogWrite(IN1, speed);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void moveBackwardRight(){
  analogWrite(IN1, speed);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void setup() {
  bluetoothConnection.begin(9600); // set up Serial library at 9600 bps

  pinMode(LED_BUILTIN, OUTPUT);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop(){
  if (bluetoothConnection.available()){
    fullMessageReceivedFromApp = "";
    while (bluetoothConnection.available()){
      characterReceivedFromApp = ((byte) bluetoothConnection.read());
      if (characterReceivedFromApp == '>'){
        break;
      }
      else {
        fullMessageReceivedFromApp += characterReceivedFromApp;
      }
      delay(1);
    }

    leftOrRight = fullMessageReceivedFromApp[0];
    forwardOrBackward = fullMessageReceivedFromApp[1];
    speedString = fullMessageReceivedFromApp.substring(2);
    speed = speedString.toInt();
    
    if (speed < 120){
      speed = 120;
    }

    switch (forwardOrBackward) {
      case '9':
        _stop();
        break;
      case FORWARD: 
        switch (leftOrRight) {
          case STRAIGHT: 
            moveForward();
            break;
          case LEFT: 
            moveForwardLeft();
            break;
          case RIGHT:
            moveForwardRight();
            break;
        }
        break;
      case BACKWARD: 
        switch (leftOrRight) {
          case STRAIGHT:
          moveBackward();
          break;
        case LEFT:
          moveBackwardLeft();
          break;
        case RIGHT:
          moveBackwardRight();
          break;  
        }
      break;
    }
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(2);
    digitalWrite(LED_BUILTIN, LOW);
    delay(2);
  }
}


