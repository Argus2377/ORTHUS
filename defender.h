#ifndef _defender_H_
#define _defender_H_

#include <Servo.h>

#define TRIG1 12
#define ECHO1 11
#define TRIG2 10
#define ECHO2 9

#define LeftAhead 5
#define LeftBack 6
#define RightAhead 8
#define RightBack 7

Servo servo;
    unsigned int angle = 90;   
    unsigned int look1 = 3000; 
    unsigned int timeEcho1=3000;
    unsigned int look2 = 3000;  
    unsigned int timeEcho2=3000;

void checkDistance(){
  digitalWrite(TRIG1, HIGH);
  digitalWrite(TRIG2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG1, LOW);
  digitalWrite(TRIG2, LOW);
  timeEcho1 = pulseIn(ECHO1, HIGH);
  timeEcho2 = pulseIn(ECHO2, HIGH);
  look1 = timeEcho1/58;
  look2 = timeEcho2/58;
  }

void goAhead(){
  digitalWrite(LeftAhead, HIGH);
  digitalWrite(LeftBack, LOW);
  digitalWrite(RightAhead, HIGH);
  digitalWrite(RightBack, LOW);
}

void goRight(){
  digitalWrite(LeftAhead, HIGH);
  digitalWrite(LeftBack, LOW);
  digitalWrite(RightAhead, LOW);
  digitalWrite(RightBack, HIGH);
}

void goLeft(){
  digitalWrite(LeftAhead, LOW);
  digitalWrite(LeftBack, HIGH);
  digitalWrite(RightAhead, HIGH);
  digitalWrite(RightBack, LOW);
}

void goBack(){
  digitalWrite(LeftAhead, LOW);
  digitalWrite(LeftBack, HIGH);
  digitalWrite(RightAhead, LOW);
  digitalWrite(RightBack, HIGH);
}

void stopRobot(){
  digitalWrite(LeftAhead, LOW);
  digitalWrite(LeftBack, LOW);
  digitalWrite(RightAhead, LOW);
  digitalWrite(RightBack, LOW);
}

#endif
