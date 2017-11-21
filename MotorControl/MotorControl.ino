#include <SoftwareSerial.h> 
#include <string.h>
#include <stdlib.h>

SoftwareSerial bluetoothConnection(12, 13); // RX, TX

#define IN2 3        // L298N Forward  (green)
#define IN3 4        // L298N Left (yellow)
#define IN4 5        // L298N Right (orange)
#define IN1 6        // L298N Backward (blue)

#define FORWARD '0'
#define BACKWARD '1'
#define LEFT '2'
#define RIGHT '3'
#define STRAIGHT '4'
#define STOP '9'

#define trigCentre A2  // HC-SR04 trig centre
#define echoCentre A3  // HC-SR04 echo centre
#define trigRight A0 // HC-SR04 trig right
#define echoRight A1 // HC-SR04 echo right
#define trigLeft A4  // HC-SR04 trig left
#define echoLeft A5  // HC-SR04 echo left
#define ENA 9 // PWM control for forward/backward
#define ENB 10 // PWM control for right/left

String fullMessageReceivedFromApp;
char characterReceivedFromApp;

char leftOrRight;
char forwardOrBackward;

String speedString;
int speed = 999;

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
	if(distanceRight >= 5 && distanceLeft >= 5){
		if(distanceCentre >= 5) analogWrite(ENA, 150);
		else analogWrite(ENA, 100);
		moveForward();
	}else if(distanceRight < 5 && distanceLeft >= 5){
		while(distanceRight < 5){
			analogWrite(ENB, 100);
			moveForwardLeft();
			delay(5);
			moveBackwardRight();
			delay(5);
		}
	}else if(distanceRight >= 5 && distanceLeft < 5){
		while(distanceLeft < 5){
			analogWrite(ENB, 100);
			moveForwardRight();
			delay(5);
			moveBackwardLeft();
			delay(5);
		}
	}else{
		if(distanceCentre >= 5){
			while(distanceRight <5 && distanceLeft < 5){
				analogWrite(ENB, 100);
				moveForwardRight();
				delay(5);
				analogWrite(ENB, 150);
				moveBackwardLeft();
				delay(5);
			}
		}else{
			while(distanceRight <5 && distanceLeft < 5 && distanceCentre < 5){
				analogWrite(ENB, 100);
				moveForwardRight();
				delay(5);
				moveBackwardLeft();
				delay(5);
			}
		}
	}
}

void setup(){
	Serial.begin(9600);
	bluetoothConnection.begin(9600); // set up Serial library at 9600 bps
	
	pinMode(LED_BUILTIN, OUTPUT);
	
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
	pinMode(ENA, OUTPUT);
	pinMode(ENB, OUTPUT);
}

void loop(){
	Serial.println("fuck");
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
	}else{
		digitalWrite(trigRight, LOW);
		delayMicroseconds(2);
		digitalWrite(trigRight, HIGH);
		delayMicroseconds(10);
		digitalWrite(trigRight, LOW);
		timeRight = pulseIn(echoRight, HIGH);
		distanceRight = timeRight*0.034/2;
		Serial.print("Distance right: ");
		if(distanceRight < 2 || distanceRight > 200) Serial.println("Out of range.");
		else Serial.println(distanceRight);
		
		digitalWrite(trigLeft, LOW);
		delayMicroseconds(2);
		digitalWrite(trigLeft, HIGH);
		delayMicroseconds(10);
		digitalWrite(trigLeft, LOW);
		timeLeft = pulseIn(echoLeft, HIGH);
		distanceLeft = timeLeft*0.034/2;
		Serial.print("Distance left: ");
		if(distanceLeft < 2 || distanceLeft > 200) Serial.println("Out of range.");
		else Serial.println(distanceLeft);
		
		digitalWrite(trigCentre, LOW);
		delayMicroseconds(2);
		digitalWrite(trigCentre, HIGH);
		delayMicroseconds(10);
		digitalWrite(trigCentre, LOW);
		timeCentre = pulseIn(echoCentre, HIGH);
		distanceCentre = timeCentre*0.034/2;
		Serial.print("Distance centre: ");
		if(distanceCentre < 2 || distanceCentre > 200) Serial.println("Out of range.");
		else Serial.println(distanceCentre);
		
		//autonomous(distanceRight, distanceLeft, distanceCentre, 0, 0);
	}
}


