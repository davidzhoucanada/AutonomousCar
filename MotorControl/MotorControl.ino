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

unsigned long distances[3];
bool tooClose;

long timeRight, timeLeft, timeCentre;
int distanceRight, distanceLeft, distanceCentre;

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

void autonomous(int distanceRight, int distanceLeft, int distanceCentre, int ENAspeed, int ENBspeed){
	speed = 150;
	if(distanceCentre >= 10 && distanceCentre <= 15){                   // if distanceCentre small move forward slowly
		speed = 120;
		moveForward();
	}else if(distanceCentre > 15) moveFoward();                         // if distanceCentre greater move forward normally
	else if(distanceCentre<=15 && distanceRight<=15 && distanceLeft>15) moveFowardLeft(); // if only distanceLeft big turn left
	else if(distanceCentre<=15 && distanceLeft<=15 && distanceRight>15) moveFowardRight(); // if only distanceRight big turn right
	else{                                                               // else turn right in place until distanceCentre is larger
		while(distanceCentre < 10){
			speed = 120;
			moveForwardRight();
			delay(20);
			moveBackwardLeft();
			delay(20);
		}
	}
}

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
	
	for(int currentSensor = 0; currentSensor < 3; currentSensor++){
		distances[currentSensor] = sensors[currentSensor].ping_cm();
		if(currentSensor == CENTRE_SENSOR) Serial.print("Distance Centre: ");
		else if(currentSensor == LEFT_SENSOR) Serial.print("Distance Left: ");
		else Serial.print("Distance Right: ");
		Serial.println(distances[currentSensor]);
	}
	
	for(int currentSensor=1; currentSensor<3; currentSensor++){
		if(distances[currentSensor]!=0 && distances[currentSensor] < 25) speed = 120;
		if(currentSensor==2 && distances[currentSensor]!=0 && distances[currentSensor] < 15){
			tooClose = true;
			// Put the autonomous function here
			
			speed = 200;
			moveBackward();
			delay(400);
			_stop();
			break;
		}
	}
	
	if(tooClose == false){
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

