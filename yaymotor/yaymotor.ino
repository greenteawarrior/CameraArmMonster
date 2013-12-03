#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

int incomingByte;


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  myMotor->setSpeed(1000);  // 10 rpm   
}

void loop() {
  //Serial.println("Double coil steps");
  //myMotor->step(360, FORWARD, MICROSTEP); 
  //void release(void);
  
  //if(Serial.available() > 0){
    //incomingByte = Serial.read();
    //Serial.print("I got the byte: ");
    //Serial.println(incomingByte, DEC);
  //}
  
  while(!Serial.available()){
    delay(100);
  }

  int dx = 0;
  int dy = 0;
  
  Serial.println("Here");
  
  boolean negate = false;
  
  while(Serial.available() > 0 && incomingByte != 59){
    if(incomingByte == 45){
      negate = true;
    }
    if(incomingByte > 47 && incomingByte < 58){
      dx = dx * 10 + incomingByte - 48;    
    }
    if(Serial.available()){
      incomingByte = Serial.read();
      
    }
  }
  if(negate){
    dx = -1 * dx;
    negate = false;
  }
  incomingByte = Serial.read(); // Gets rid of the semicolon

  while(Serial.available() > 0 && incomingByte != 59){
    if(incomingByte == 45){
      negate = true;
    }
    if(incomingByte > 47 && incomingByte < 58){
      dy = dy * 10 + incomingByte - 48;    
    }
    if(Serial.available()){
      incomingByte = Serial.read();
      
    }
  }
  incomingByte = Serial.read(); // Gets rid of the semicolon
  if(negate){
    dy = -1 * dy;
    negate = false;
  }
  
  Serial.print("X Distance: ");
  Serial.print(dx, DEC);
  Serial.print(", Y Distance: ");
  Serial.println(dy, DEC);
  
  delay(100);
}
