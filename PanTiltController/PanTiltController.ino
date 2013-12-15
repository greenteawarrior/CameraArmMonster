//creating servo object
#include <Servo.h>

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
  }
/*  boolean firstVal = true; //Uses this to understand whether the value goes into pan (true?) or tilt (false?) first
  input = "";
  while(Serial.available()){ //Takes in a string as a form like "100;200;"
    char in = (char)Serial.read();
    if(isInt(in)){ //If the character is part of a number (Not a space), then
      input += in;
    }
    
    

    else if(firstVal){ //Sets Pan when not a number
      firstVal = false;
      panPos += convertToSteps(input.toInt());
      panPos = constrain(panPos, 45, 135);
      if(debugMode){        
       // Serial.println(input);
        Serial.print("Pan: ");
        Serial.print(panPos);
      }
      input = "";
    }

    else{ //Sets Tilt when pan is already set
      tiltPos += convertToSteps(input.toInt());
      tiltPos = constrain(tiltPos, 45, 135);
      if(debugMode){
        //Serial.println(input);
        Serial.print(" Tilt: ");
        Serial.println(tiltPos);
      }
      input = "";
      return;
    }
  }*/
}

char nextChar(){
  return Serial.read();
}

int nextInt(){
  return nextArg().toInt();
}

String nextArg(){
  skipWhitespace();
  String arg = "";
  arg.reserve(200);
  char incomingByte = Serial.peek();
  while(incomingByte != ' ') //Space is 32 and Semicolon is 59
  {
    arg += Serial.read();
    incomingByte = Serial.peek();
  }
  return arg
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
    case 3: Serial.println("Now using Proportional Power feedback");
      type = 3;
    case 4: Serial.println("Now using type 4 Spatial Tracking");
      type = 4;
      return;
    default: Serial.print("Type is unchanged, no result found for type = ";
    Serial.println(arg);
  }
}

//Returns servo degree conversion. 
//May not be linear because of how pictures work, use this to adjust
//  How much a single pixel distance is worth in terms of degrees.
int convertToSteps(int pixelFeedback){
  switch(type)
  {
    case 1: return linear(pixelFeedback);
    case 2: return threshold(pixelFeedback);
  }
}

int linear(int pix){
  return (int)(motorMultiplier * pix);}

int threshold(int pix){
  return constrain(pix / 15, -1, 1);
}

//Checks if a character is an integer, also includes the
// negative sign for usage. (Is it a string of numbers?)
//Note: Does NOT include decimal points
bool isInt(char inChar){
  boolean isNegative = inChar == 45; //45 is the hyphen
  boolean isNumber = false;
  if(inChar >= 48 && inChar <= 57
  ){ //0 to 9 in ascii
    isNumber = true;
  }
  return isNegative || isNumber;
}


