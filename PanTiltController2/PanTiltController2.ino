//creating servo object
#include <Servo.h>
#include <math.h>

/*SETTINGS
tilt and pan pins based on arduino setup
motorMultiplier is how extreme
*/
int tiltPin = 9;
int panPin = 10;
int type = 1;
/*
Type tells what feedback type to use:
1: Linear (0.04 * Pixel Offset)
2: Threshold (1 if off by more than 10 cm, otherwise 0)
3: 
4: Spatial Tracking (No webcam required)
*/

double motorMultiplier = 0.04;

boolean debugMode = true;



//End options

String input = ""; //Holds the Serial i
Servo tilt; //Create Tilt servo
Servo pan; //Create Pan servo

int tiltPos = 90; //Stores Servo Position.  0-?? degrees
//is it really 0-180 degrees? consider mechanical limitations of gimbal system
int panPos = 90; //Stores Pan Position. 0-?? degrees

double radToDeg = 57.2957795131;
double degToRad = 0.01745329251;
double r = 40.5;

double xPos = 0.0;
double yPos = 40.5;
double zPos = 0.0;

double xGoal = 0.0;
double yGoal = 0.0;
double zGoal = 0.0;

void setup(){
  tilt.attach(tiltPin);
  pan.attach(panPin);

  Serial.begin(9600); //Initialize Serial Port 9600, use 9600 settings
  input.reserve(200); //Reserves 200 bytes for a string which is much more than we need
}

void loop(){
  pan.write(panPos);
  delay(60); //Wait for servos to get to position
  //might want to play with the delay numbers for better performance


  tilt.write(tiltPos); //<
  delay(60); //Wait for servos to get to position
}

//In between loop calls, Serial Event checks the serial port and
//Uses the inputs to change pan and tilt respectively.
//Input is PANVALUE;TILTVALUE;
//Resets if not used
//";" can be replaced with any non numeric character
//ONLY INTEGERS CAN BE USED
void serialEvent() {
  while(Serial.available()){
    String command = nextArg();
    if(command == "t" || command == "T"){ //Requires 1 input
      changeType();}
    else if(command == "i" || command == "I"){ //Requires 2 inputs
    switch(type){
        case 1: linearMotion();
          break;
        case 2: thresholdMotion();
          break;
        case 3: Serial.println("Not programmed yet");
          break;
        case 4: spatialMovement();
          break;}}
    else if(command == "g" || command == "G"){ //Requires 3 inputs
      if(type != 4){
        Serial.println("Not setting goal, wrong type");}
      else{
        setGoal();}
    }
    else{
      Serial.print("Ignored argument: ");
      Serial.println(command);
    }
  }
}

char nextChar(){
  return Serial.read();
}

int nextInt(){
  return nextArg().toInt();
}

double nextDouble(){
  String in = nextArg();
  in += "0";
  char buf[in.length()];
  in.toCharArray(buf, in.length());
  return atof(buf);
}

String nextArg(){
  skipWhitespace();
  String arg = "";
  arg.reserve(200);
  int incomingByte = Serial.peek();
  while(incomingByte > 0 && incomingByte != 32) //Space is 32 and Semicolon is 59
  {
    arg += (char)Serial.read();
    incomingByte = Serial.peek();
  }
  return arg;
} 

void skipWhitespace(){
  while(Serial.peek() == ' '){
    Serial.read();
  }
}

void changeType(){
  int arg = nextInt();
  switch(arg){
    case 1: Serial.println("Now using type 1 Linear input");
      type = 1;
      return;
      
    case 2: Serial.println("Now using type 2 Threshold feedback");
      type = 2;
      return;
    case 3: Serial.println("Now using type 3 Proportional Power feedback");
      type = 3;
      return;
    case 4: Serial.println("Now using type 4 Spatial Tracking");
      type = 4;
      return;
    default: Serial.print("Type is unchanged, no result found for type = ");
    Serial.println(arg);
  }
}


/* Activated with Serial command: I (dx) (dy) when type = 2
Lineary proportional motion
*/
void linearMotion(){
  int panDelta = (int)(motorMultiplier * nextInt());
  panPos += panDelta;
  int tiltDelta = (int)(motorMultiplier * nextInt());
  tiltPos += tiltDelta;
  Serial.print("Pan moved ");
  Serial.print(panDelta);
  Serial.print(" degrees linearly to ");
  Serial.print(panPos);
  Serial.print(" Tilt moved ");
  Serial.print(tiltDelta);
  Serial.print(" degrees linearly to ");
  Serial.println(tiltPos); 
}


void thresholdMotion(){
  int panDelta = constrain(nextInt()/15, -1, 1);
  panPos += panDelta;
  int tiltDelta = constrain(nextInt()/15, -1, 1);
  tiltPos += tiltDelta;
  Serial.print("Pan moved ");
  Serial.print(panDelta);
  Serial.print(" degree(s) to ");
  Serial.print(panPos);
  Serial.print(" Tilt moved ");
  Serial.print(tiltDelta);
  Serial.print(" degree(s) to ");
  Serial.println(tiltDelta); 
}

void setGoal(){
  xGoal = nextDouble();
  yGoal = nextDouble();
  zGoal = nextDouble();
  Serial.print("New goal defined to be: X:");
  Serial.print(xGoal);
  Serial.print(" in Y: ");
  Serial.print(yGoal);
  Serial.print(" in Z: ");
  Serial.print(zGoal);
  Serial.println(" in");
}

void spatialMovement(){
  //Get input values
  double theta = nextDouble() * degToRad;
  double phi = nextDouble() * degToRad;
  
  //Determine new position
  xPos = r * cos(phi) * cos(theta);
  yPos = r * cos(phi) * sin(theta);
  zPos = r * sin(phi);
  
  //Determine XYZ Vector of where the camera needs to look
  double xAim = xGoal - xPos;
  double yAim = yGoal - yPos;
  double zAim = zGoal - zPos;
  
  //Determine Pan/Tilt degrees required
  double panGoal = atan(yAim / xAim) - theta;
  double tiltGoal = atan(zAim / sqrt(xAim * xAim + yAim * yAim));

  //Move to new positions while constrained between reasonable degrees
  panPos = (int)constrain(panGoal * radToDeg, 45, 135); 
  tiltPos = 90 - (int)constrain(tiltGoal * radToDeg, 60, 120);
  Serial.print("Now adjusting to match new coordinates");
}
