//creating servo object
#include <Servo.h>
#include <math.h>

/////////////////
//BEGIN OPTIONS//
/////////////////

/* SETTINGS
 * tilt and pan pins based on arduino setup
 * motorMultiplier is how extreme the motor moves */
int tiltPin = 9;
int panPin = 10;
int type = 1;
/* Type tells what feedback type to use, Changeable in Serial:
 * 1: Linear (0.04 * Pixel Offset)
 * 2: Threshold (1 if off by more than 10 cm, otherwise 0)
 * 3: Power-Based 
 * 4: Spatial Tracking (No webcam required) */

double motorMultiplier = 0.04;
boolean debugMode = true;
///////////////
//End options//
///////////////

Servo tilt; //Create Tilt servo
Servo pan; //Create Pan servo

int tiltPos = 90; //Stores Servo Position.  45-135 degrees
int panPos = 90; //Stores Pan Position. 60-120 degrees

//Physical constants used to calculate point tracking
double radToDeg = 57.2957795131;
double degToRad = 0.01745329251;
double r = 40.5;

//Point tracking variables
double xPos = 0.0;
double yPos = 40.5;
double zPos = 0.0;

double xGoal = 0.0;
double yGoal = 0.0;
double zGoal = 0.0;


void setup(){
  //Attach servos to pins
  tilt.attach(tiltPin);
  pan.attach(panPin);

  //Get Serial port going
  Serial.begin(9600);}

////////////////////
//MAIN UPDATE LOOP//
////////////////////
void loop(){
  pan.write(panPos);
  delay(60); //Wait for servos to get to position

  tilt.write(tiltPos);
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
    //Get command
    String command = nextArg();

    //Determine if the command means something

    //Change Type
    if(command == "t" || command == "T"){ //Requires 1 input
      changeType();}

    //Input Motion
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
    //Change Goal
    else if(command == "g" || command == "G"){ //Requires 3 inputs
      if(type != 4){
        Serial.println("Not setting goal, wrong type");}
      else{
        setGoal();}}

    //A nonsensical command
    else{
      Serial.print("Ignored argument: ");
      Serial.println(command);}
  }
}

//Eliminates whitespace elements
void skipWhitespace(){
  while(Serial.peek() == ' '){
    Serial.read();}}

/* Serial Parser
 * Obtains the next argument.  All arguments are separated by spaces
 * Will continue adding to a string until a space or Serial cannot read
 * anymore values. */
String nextArg(){
  skipWhitespace();
  String arg = "";
  arg.reserve(200);
  int incomingByte = Serial.peek();
  while(incomingByte > 0 && incomingByte != 32){ //Space is 32 and Semicolon is 59
    arg += (char)Serial.read();
    incomingByte = Serial.peek();}
  return arg;}

//Gets the next argument as an integer
int nextInt(){
  return nextArg().toInt();}

//Gets the next argument as a double
double nextDouble(){
  String in = nextArg();
  in += "0";
  char buf[in.length()];
  in.toCharArray(buf, in.length());
  return atof(buf);}

/* Activated using Serial Command: T (Type Number)
 * 1: Linear
 * 2: Threshold
 * 3: Power
 * 4: Point Tracking */
void changeType(){
  int arg = nextInt();
  switch(arg){
    case 1: Serial.println("Now using type 1 Linear input");
      type = 1; return;
    case 2: Serial.println("Now using type 2 Threshold feedback");
      type = 2; return;
    case 3: Serial.println("Now using type 3 Proportional Power feedback");
      type = 3; return;
    case 4: Serial.println("Now using type 4 Spatial Tracking");
      type = 4; return;
    default: Serial.print("Type is unchanged, no result found for type = ");
      Serial.println(arg);}}


/* Activated with Serial command: I (dx) (dy) when type = 2
 * Lineary proportional motion to the number of degrees off */
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
  Serial.println(tiltPos);}

/* Thresholded motion.  If pixel distance is off by 15,
 * It will output 1.
 * Otherwise it will output 0. */
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
  Serial.println(tiltDelta);}

/* Useable only when type = 4
 * Otherwise is never called
 * Resets the goal using the next three arguments */
void setGoal(){
  xGoal = nextDouble();
  yGoal = nextDouble();
  zGoal = nextDouble();
  Serial.print("New goal defined to be: X:");
  Serial.print(xGoal);
  Serial.print("in Y:");
  Serial.print(yGoal);
  Serial.print("in Z:");
  Serial.print(zGoal);
  Serial.println("in");}

/* Point Tracking motion update
 * This updates the current position with two arguments
 *    New Pan New Tilt
 * This function calculates the new required pan and tilt degrees
 * And updates them
 * Note: This is not automatic, requires Serial Input from CAM */
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
  Serial.print("Now adjusting to match new coordinates");}