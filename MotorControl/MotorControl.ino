#include <SoftwareSerial.h> 
SoftwareSerial mySerial(12, 13); // RX, TX

#define back 2	// L298N back (blue)
#define forward 3	// L298N forward (green)
#define left 4	// L298N left (yellow)
#define right 5	// L298N right (orange)

char i;

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

void setup(){
    Serial.begin(9600);
 
    // HC-06 default serial speed is 9600
    mySerial.begin(9600);

    pinMode(back, OUTPUT);
    pinMode(forward, OUTPUT);
    pinMode(left, OUTPUT);
    pinMode(right, OUTPUT);
}

void loop(){
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
	/*
	
	*/
}

