#include <SoftwareSerial.h> 
SoftwareSerial mySerial(12, 13); // RX, TX

#define back 2	// L298N back (blue)
#define forward 3	// L298N forward (green)
#define left 4	// L298N left (yellow)
#define right 5	// L298N right (orange)
#define trigRight 8 // HC-SR04 trig right
#define echoRight 9 // HC-SR04 echo right
#define trigLeft 10 // HC-SR04 trig left
#define echoLeft 11	// HC-SR04 echo left

const long interval = 14;

char i;
long timeRight, timeLeft;
int distanceRight, distanceLeft;
long currentInterval, previousInterval;
boolean readSensor=false;

void _stop();
void forward();
void back();
void turnRight();
void turnLeft();
void turnBackRight();
void turnBackLeft();

void _stop(){
	digitalWrite(back, LOW);
	digitalWrite(forward, LOW);
	digitalWrite(left, LOW);
	digitalWrite(right, LOW);
}
void forward(){
	digitalWrite(back, LOW);
	digitalWrite(forward, HIGH);
	digitalWrite(left, LOW);
	digitalWrite(right, LOW);
}
void back(){
	digitalWrite(back, HIGH);
	digitalWrite(forward, LOW);
	digitalWrite(left, LOW);
	digitalWrite(right, LOW);
}
void forwardRight(){
	digitalWrite(back, LOW);
	digitalWrite(forward, HIGH);
	digitalWrite(left, LOW);
	digitalWrite(right, HIGH);
}
void forwardLeft(){
	digitalWrite(back, LOW);
	digitalWrite(forward, HIGH);
	digitalWrite(left, HIGH);
	digitalWrite(right, LOW);
}
void backRight(){
	digitalWrite(back, HIGH);
	digitalWrite(forward, LOW);
	digitalWrite(left, LOW);
	digitalWrite(right, HIGH);
}
void backLeft(){
	digitalWrite(back, HIGH);
	digitalWrite(forward, LOW);
	digitalWrite(left, HIGH);
	digitalWrite(right, LOW);
}

void autonomous(int distanceRight, int distanceLeft){
	if(distanceRight >= 5 && distanceLeft >= 5){
		forward();
	}else if(distanceRight < 5 && distanceLeft >= 5){
		forwardLeft();
	}else if(distanceRight >= 5 && distanceLeft < 5){
		forwardRight();
	}else{
		// ?????
	}
}

void setup(){
    Serial.begin(9600);
 
    // HC-06 default serial speed is 9600
    mySerial.begin(9600);

    pinMode(back, OUTPUT);
    pinMode(forward, OUTPUT);
    pinMode(left, OUTPUT);
    pinMode(right, OUTPUT);
    pinMode(trigRight, OUTPUT);
    pinMode(echoRight, INPUT);
    pinMode(trigLeft, OUTPUT);
    pinMode(echoLeft, INPUT);
}

void loop(){
	// manual Bluetooth controls via Android app
	if(mySerial.available()>0){
		i = (char)mySerial.read();
    
		switch(i){
			case'1':{
				forward();
				break;
			}
			case'2':{
				back();
				break;
			}
			case'3':{
				forwardRight();
				break;
			}
			case'4':{
				forwardLeft();
				break;
			}
			case'5':{
				backRight();
				break;
			}
			case'6':{
				backLeft();
				break;
			}
			case'7':{
				_stop();
				break;
			}
		}
	}
	
	// ultrasonic range sensor
	currentInterval = micros();
	if(readSensor){
		if(currentInterval - previousInterval <= 2){
			digitalWrite(trigRight, LOW);
			digitalWrite(trigLeft, LOW);
		}else if(currentInterval - previousInterval <=12){
			digitalWrite(trigRight, HIGH);
			digitalWrite(trigLeft, HIGH);
		}else{
			timeRight = pulseIn(echoRight, HIGH);
			timeLeft = pulseIn(echoLeft, HIGH);
			distanceRight = timeRight*0.034/2;
			distanceLeft = timeLeft*0.034/2;
			readSensor = false;

			//autonomous(distanceRight, distanceLeft);
		}
	}else if(currentInterval-previousInterval >= interval){
		previousInterval = currentInterval;
		readSensor = true;
	}
}

