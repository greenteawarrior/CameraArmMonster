//creating servo object
#include <Servo.h>

/*SETTINGS
tilt and pan pins based on arduino setup
motorMultiplier is how extreme
*/
int tiltPin = 9;
int panPin = 10;

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
  boolean firstVal = true; //Uses this to understand whether the value goes into pan (true?) or tilt (false?) first
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
  }
}

//Returns servo degree conversion. 
//May not be linear because of how pictures work, use this to adjust
//  How much a single pixel distance is worth in terms of degrees.
int convertToSteps(int pixelFeedback){ 
  if(pixelFeedback < 15 && pixelFeedback > -15){
    return 0;
  }
  if(pixelFeedback >= 15){
    return 1;
  }
    return -1;
/*
  boolean flag = pixelFeedback < 0;
  
  if(flag){
    pixelFeedback = -1 * pixelFeedback;
  }
  //int outvalue = (int)(pow(pixelFeedback/ 50.0, 0.5) * 4);
  int outvalue = (int)(.001 * pow(pixelFeedback,3) + .3*pixelFeedback)
  if(flag){
    outvalue = -1 * outvalue;
  }
  return outvalue;*/
  //return (int)(pixelFeedback * motorMultiplier);
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


