#include <SoftwareSerial.h> 
SoftwareSerial mySerial(12, 13); // RX, TX

#define IN1 2 // L298N back (blue)
#define IN2 3 // L298N forward (green)
#define IN3 4 // L298N left (yellow)
#define IN4 5 // L298N right (orange)
#define trigRight 6 // HC-SR04 trig right
#define echoRight 7 // HC-SR04 echo right
#define trigLeft 8  // HC-SR04 trig left
#define echoLeft 9  // HC-SR04 echo left
#define ENA 10 // PWM control for forward/backward
#define ENB 11 // PWM control for right/left;
// #define MAX_V 255
// #define MIN_V

char *command;
int direc = 0;
int speed = 0;
// float angle = 0;
char i;
long timeRight, timeLeft;
int distanceRight, distanceLeft;

void _stop();
void forward();
void back();
void forwardRight();
void forwardLeft();
void backRight();
void backLeft();

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

void autonomous(int distanceRight, int distanceLeft){
	if(distanceRight >= 5 && distanceLeft >= 5) forward();
	else if(distanceRight < 5 && distanceLeft >= 5) forwardLeft();
	else if(distanceRight >= 5 && distanceLeft < 5) forwardRight();
	//else
}

void autonomousTest(int distanceRight){
	if(distanceRight >= 10){
		Serial.println("going forward");
		//analogWrite(ENA,60);
		forward();
	}
	else{
		Serial.println("else");
		//analogWrite(ENA,60);
		back();
	}
}

void setup(){
    Serial.begin(9600);
 
    // HC-06 default serial speed is 9600
    mySerial.begin(9600);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(trigRight, OUTPUT);
    pinMode(echoRight, INPUT);
    pinMode(trigLeft, OUTPUT);
    pinMode(echoLeft, INPUT);
}

void loop(){
	// manual Bluetooth controls via Android app if input is available, else autonomous
	int index=0;
	while(Serial.available()>0){
		command[index++]=Serial.read();
	}
	speed = atoi(&command[2])*100+atoi(&command[3])*10+atoi(&command[4])*1;
	if(strlen(command)==1){
		_stop();
		// break;
	}
	else if(command[0]=='0'){
		if(command[1]=='0'){ //00 forward
			analogWrite(ENA,speed);
			forward();
			// break;
		}
		else if(command[1]=='2'){ //02 forward left
			analogWrite(ENA,speed);
			turnLeft();
			// break;
		}
		else if(command[1]=='3'){ //03 forward right
			analogWrite(ENA,speed);
			turnRight();
			// break;
		}
	}
	else if(command[0]=='1'){
		if(command[1]=='1'){ //11 backward
			analogWrite(ENA,speed);
			back();
			// break;
		}
		else if(command[1]=='2'){ //12 backleft
			analogWrite(ENA,speed);
			turnBackLeft();
			// break;
		}
		else if(command[1]=='3'){ //13 backright
			analogWrite(ENA,speed);
			turnBackRight();
			// break;
		}
	}
	/*
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
	}else{*/
		// ultrasonic range sensor
		digitalWrite(trigRight, LOW);
		//digitalWrite(trigLeft, LOW);
		delayMicroseconds(2);
	
		digitalWrite(trigRight, HIGH);
		//digitalWrite(trigLeft, HIGH);
		delayMicroseconds(10);
		digitalWrite(trigRight, LOW);
		//digitalWrite(trigLeft, LOW);
	
		timeRight = pulseIn(echoRight, HIGH);
		//timeLeft = pulseIn(echoLeft, HIGH);
		distanceRight = timeRight*0.034/2;
		//distanceLeft = timeLeft*0.034/2;
	
		Serial.println("Distance right, distance left ");
		Serial.println(distanceRight);
		//Serial.println(distanceLeft);
		autonomousTest(distanceRight);
		//autonomous(distanceRight, distanceLeft);
	//}
}

