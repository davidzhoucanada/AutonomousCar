#include <SoftwareSerial.h> 
SoftwareSerial mySerial(12, 13); // RX, TX

#define IN1 2	// L298N Back (blue)
#define IN2 3	// L298N Forward  (green)
#define IN3 4	// L298N Left (yellow)
#define IN4 5	// L298N Right (orange)

char i;

void _stop();
void forward();
void back();
void turnRight();
void turnLeft();
void turnBackRight();
void turnBackLeft();

void _stop(){
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);
}
void forward(){
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);
}
void back(){
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);
}
void forwardRight(){
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, HIGH);
}
void forwardLeft(){
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, HIGH);
	digitalWrite(IN4, LOW);
}
void backRight(){
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, HIGH);
}
void backLeft(){
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, HIGH);
	digitalWrite(IN4, LOW);
}

void setup(){
    Serial.begin(9600);
 
    // HC-06 default serial speed is 9600
    mySerial.begin(9600);  
}

void loop(){
	if(mySerial.available()>0){
		i=(char)mySerial.read();
    
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
}

