#include <SoftwareSerial.h> 

//lef2 rig3 for0 back1 stop4
#define IN1 2        // L298N Right (blue)
#define IN2 3        // L298N Left  (green)
#define IN3 4        // L298N Forward (yellow)
#define IN4 5        // L298N Backward (orange)
#define ENA 10
#define ENB 11
// #define MAX_V 255
// #define MIN_V 
char *command;
int direc = 0;
int speed = 0;
// float angle = 0;

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
void turnRight(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void turnLeft(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void forward(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void back(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnBackRight(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnBackLeft(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void setup() {
  Serial.begin(9600); // set up Serial library at 9600 bps
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void loop(){
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
    if(command[1]=='0'){//00 forward
      analogWrite(ENA,speed);
      forward();
      // break;
    }
    else if(command[1]=='2'){//02 forward left
      analogWrite(ENA,speed);
      turnLeft();
      // break;
    }
    else if(command[1]=='3'){//03 forward right
      analogWrite(ENA,speed);
      turnRight();
      // break;
    }
  }
  else if(command[0]=='1'){
    if(command[1]=='1'){//11 backward
      analogWrite(ENA,speed);
      back();
      // break;
    }
    else if(command[1]=='2'){//12 backleft
      analogWrite(ENA,speed);
      turnBackLeft();
      // break;
    }
    else if(command[1]=='3'){//13 backright
      analogWrite(ENA,speed);
      turnBackRight();
      // break;
    }  
  }
}





