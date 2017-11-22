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
int speed = 999;

NewPing sensors[3] = {
  NewPing(trigLeft, echoLeft),
  NewPing(trigCentre, echoCentre),
  NewPing(trigRight, echoRight)
};

unsigned long distanceFromObstacle[3];
bool tooClose;

long rightSensorUltrasonicSignalTravelTime, leftSensorUltrasonicSignalTravelTime, centreSensorUltrasonicSignalTravelTime;
int rightSensorDistanceFromObstacle, leftSensorDistanceFromObstacle, centreSensorDistanceFromObstacle;

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
  delay(50);
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

int determineDistanceFromObstacle(int trigSensor, int echoSensor){
    digitalWrite(trigSensor, LOW);
    delayMicroseconds(2);
    digitalWrite(trigSensor, HIGH);
    delayMicroseconds(5);
    digitalWrite(trigSensor, LOW);
    int sensorUltrasonicSignalTravelTime = pulseIn(echoSensor, HIGH);
    int sensorDistanceFromObstacle = sensorUltrasonicSignalTravelTime*0.034/2;
    return sensorDistanceFromObstacle;
}

/*void autonomous(int rightSensorDistanceFromObstacle, int leftSensorDistanceFromObstacle, int centreSensorDistanceFromObstacle, int ENAspeed, int ENBspeed){
  if(rightSensorDistanceFromObstacle >= 5 && leftSensorDistanceFromObstacle >= 5){
    if(centreSensorDistanceFromObstacle >= 5) {
      analogWrite(ENA, 150);
    }
    else {
      analogWrite(ENA, 100);
    }
    moveForward();
  }else if(rightSensorDistanceFromObstacle < 5 && leftSensorDistanceFromObstacle >= 5){
    while(rightSensorDistanceFromObstacle < 5){
      analogWrite(ENB, 100);
      moveForwardLeft();
      delay(5);
      moveBackwardRight();
      delay(5);
    }
  }else if(rightSensorDistanceFromObstacle >= 5 && leftSensorDistanceFromObstacle < 5){
    while(leftSensorDistanceFromObstacle < 5){
      analogWrite(ENB, 100);
      moveForwardRight();
      delay(5);
      moveBackwardLeft();
      delay(5);
    }
  }else{
    if(centreSensorDistanceFromObstacle >= 5){
      while(rightSensorDistanceFromObstacle <5 && leftSensorDistanceFromObstacle < 5){
        analogWrite(ENB, 100);
        moveForwardRight();
        delay(5);
        analogWrite(ENB, 150);
        moveBackwardLeft();
        delay(5);
      }
    }else{
      while(rightSensorDistanceFromObstacle <5 && leftSensorDistanceFromObstacle < 5 && centreSensorDistanceFromObstacle < 5){
        analogWrite(ENB, 100);
        moveForwardRight();
        delay(5);
        moveBackwardLeft();
        delay(5);
      }
    }
  }
}*/
void setup(){
  Serial.begin(9600);
  bluetoothConnection.begin(9600); // set up Serial library at 9600 bps
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(trigCentre, OUTPUT);
  pinMode(echoCentre, INPUT);
}
void loop(){

  tooClose = false;
  
  if(bluetoothConnection.available()){
    fullMessageReceivedFromApp = "";
    while(bluetoothConnection.available()){
      characterReceivedFromApp = ((byte) bluetoothConnection.read());
      if(characterReceivedFromApp == '>') break;
      else fullMessageReceivedFromApp += characterReceivedFromApp;
      delay(1);
    }
    
    leftOrRight = fullMessageReceivedFromApp[0];
    forwardOrBackward = fullMessageReceivedFromApp[1];
    speedString = fullMessageReceivedFromApp.substring(2);
    speed = speedString.toInt();
    
    if(speed < 120) speed = 120;
  }
  
  for (int currentSensor = 0; currentSensor < 3; currentSensor++){
    distanceFromObstacle[currentSensor] = sensors[currentSensor].ping_cm();
    if (currentSensor == LEFT_SENSOR){
      Serial.print("Distance Left: ");
    }
    else if (currentSensor == CENTRE_SENSOR){
      Serial.print("Distance Centre: ");
    }
    else {
      Serial.print("Distance Right: ");
    }
    Serial.println(distanceFromObstacle[currentSensor]);
  }

  for (int currentSensor = 1; currentSensor < 3; currentSensor++) {
    if (distanceFromObstacle[currentSensor] != 0 && distanceFromObstacle[currentSensor] < 25){
      speed = 120;
    }
    if (distanceFromObstacle[currentSensor] != 0 && distanceFromObstacle[currentSensor] < 15){
      tooClose = true;
      // Put the autonomous function here
      
      speed = 200;
      moveBackward();
      delay(400);
      _stop();
      break;
    }
  }
  
  if (tooClose == false) {    
    switch(forwardOrBackward){
      case STOP:
        _stop();
        break;
      case FORWARD: 
        switch(leftOrRight){
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
        switch(leftOrRight){
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
}

