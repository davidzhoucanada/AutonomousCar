#include <SoftwareSerial.h> 
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <NewPing.h>

SoftwareSerial bluetoothConnection(12, 13); // RX, TX

#define IN2 3        // L298N Forward  (green)
#define IN3 4        // L298N Left (yellow)
#define IN4 5        // L298N Right (orange)
#define IN1 6        // L298N Backward (blue)

#define trigCentre A2  // HC-SR04 trig centre
#define echoCentre A3  // HC-SR04 echo centre
#define trigRight A0 // HC-SR04 trig right
#define echoRight A1 // HC-SR04 echo right
#define trigLeft A4  // HC-SR04 trig left
#define echoLeft A5  // HC-SR04 echo left

#define FORWARD '0'
#define BACKWARD '1'
#define LEFT '2'
#define RIGHT '3'
#define STRAIGHT '4'
#define STOP '9'

#define LEFT_SENSOR 0
#define CENTRE_SENSOR 1
#define RIGHT_SENSOR 2

String fullMessageReceivedFromApp;
char characterReceivedFromApp;

char leftOrRight;
char forwardOrBackward;
String speedString;
int speed = 0;
bool isStopped = false;

NewPing sensors[3] = {
  NewPing(trigLeft, echoLeft),
  NewPing(trigCentre, echoCentre),
  NewPing(trigRight, echoRight)
};

unsigned long distancesFromObstacle[3];
bool tooCloseToObstacle;

void _stop(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveForward(){
  digitalWrite(IN1, LOW);
  analogWrite(IN2, speed);
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

int smallestDistanceFromObstacle(unsigned long distancesFromObstacle[]){
  int smallestDistanceIndex = 0;
  for (int currentSensor = 1; currentSensor < 3; currentSensor++){
    if (distancesFromObstacle[currentSensor] < distancesFromObstacle[smallestDistanceIndex]){
      smallestDistanceIndex = currentSensor;
    }
  }
  return smallestDistanceIndex;
}

void avoidObstacle(unsigned long distancesFromObstacle[]){
  speed = 255;
  int smallestDistanceIndex = smallestDistanceFromObstacle(distancesFromObstacle);
  switch (smallestDistanceIndex) {
    case LEFT_SENSOR:
      moveBackwardLeft();
      delay(400);
      _stop();
      break;
    case CENTRE_SENSOR:
      moveBackward();
      delay(400);
      _stop();
      break;
    case RIGHT_SENSOR:
      moveBackwardRight();
      delay(400);
      _stop();
      break;      
  }   
}

void autonomousMovement(unsigned long distancesFromObstacle[]){
  speed = 255;
  int smallestDistanceIndex = smallestDistanceFromObstacle(distancesFromObstacle);
  switch (smallestDistanceIndex) {
    case LEFT_SENSOR:
      moveBackwardLeft();
      delay(700);
      speed = 210;
      moveForwardRight();
      delay(700);
      break;
    case CENTRE_SENSOR:
      moveBackwardLeft();
      delay(700);
      speed = 210;
      for(int currentSensor = 0; currentSensor < 3; currentSensor++){
        distancesFromObstacle[currentSensor] = sensors[currentSensor].ping_cm();
      }
      if (distancesFromObstacle[LEFT_SENSOR] > distancesFromObstacle[RIGHT_SENSOR]) moveForwardLeft();          
      else moveForwardRight();
      delay(700);
      break;
    case RIGHT_SENSOR:
      moveBackwardRight();
      delay(700);
      speed = 210;
      moveForwardLeft();
      delay(700);
      break;      
  }   
}

void setup(){
  bluetoothConnection.begin(9600); // set up Serial library at 9600 bps
  Serial.begin(9600);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  
  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(trigCentre, OUTPUT);
  pinMode(echoCentre, INPUT);
}

void loop(){

  tooCloseToObstacle = false;

  // Get distance from obstacle
  for(int currentSensor = 0; currentSensor < 3; currentSensor++){
    distancesFromObstacle[currentSensor] = sensors[currentSensor].ping_cm();
  }

  for (int currentSensor = 0; currentSensor < 3; currentSensor++){
    if(distancesFromObstacle[currentSensor] < 40){
      speed = 120;
    }
    if(distancesFromObstacle[currentSensor] < 25 && distancesFromObstacle[currentSensor] != 0){
      tooCloseToObstacle = true;
      break;
    }
  }
  Serial.println(tooCloseToObstacle);
  
  if (bluetoothConnection.available()){
    if (tooCloseToObstacle == false){
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
          isStopped = true;
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
      avoidObstacle(distancesFromObstacle);
    }
  }
  else if (isStopped == false) {
    if (tooCloseToObstacle == true){
      autonomousMovement(distancesFromObstacle);  
    }
    else {
      speed = 130;
      moveForward();
    }
  }
}

