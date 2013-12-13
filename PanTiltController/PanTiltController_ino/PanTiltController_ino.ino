#include <Servo.h>

/*SETTINGS
tilt and pan pins based on arduino setup
motorMultiplier is how extreme
*/
int tiltPin = 9;
int panPin = 10;
double motorMultiplier = 0.3;
float power = 1;
boolean debugMode = true;

/*Type of step conversion to use, will be making various types:
Linear: Degree = m * PixelDistance
SquareRoot: Degree = m * PixelDistance^0.5
CustomPow: Degree = m * PixelDistance^(C)
*/
String stepConversionType = "Linear";
//End options

String input = ""; //Holds the Serial input
Servo tilt; //Create tilt servo
Servo pan; //Create Pan servo

//Store each individual Servo position.  They are locked between 0 and 180 degrees.
//Intialized with 90 as the center.
int tiltPos = 90;
int panPos = 90;


void setup(){
  tilt.attach(tiltPin);
  pan.attach(panPin);

  Serial.begin(9600); //Initialize Serial Port 9600, use 9600 settings
  input.reserve(200); //Reserves 200 bytes for a string which is much more than we need
}

void loop(){
  pan.write(panPos);
  delay(15); //Wait for servos to get to position
  tilt.write(tiltPos);
  delay(15); //Wait for servos to get to position
}

//In between loop calls, Serial Event checks the serial port and
//Uses the inputs to change pan and tilt respectively.
//Input is PANVALUE;TILTVALUE;
//Resets if not used
//";" can be replaced with any non numeric character
//ONLY INTEGERS CAN BE USED
void serialEvent() {
  boolean firstVal = true; //Uses this to understand whether the value goes into pan or tilt first
  input = "";
  while(Serial.available()){ //Takes in a string as a form like "100;200;"
    char in = (char)Serial.read();
    if(isInt(in)){ //If the character is part of a number (Not a space), then
      input += in;
    }

    else if(firstVal){ //Sets Pan when not a number
      firstVal = false;
      panPos += convertToSteps(input.toInt());
      panPos = constrain(panPos, 0, 180);
      if(debugMode){        
        Serial.println(input);
        Serial.print("Pan: ");
        Serial.println(panPos);
      }
      input = "";
    }

    else{ //Sets Pitch when pan is already set
      tiltPos += convertToSteps(input.toInt());
      tiltPos = constrain(tiltPos, 0, 180);
      if(debugMode){
        Serial.println(input);
        Serial.print("Tilt: ");
        Serial.println(tiltPos);
      }
      input = "";
      return;
    }
  }
}

//Returns stepper motor degree conversion.
//May not be linear because of how pictures work, use this to adjust
//  How much a single pixel distance is worth in terms of degrees.
int convertToSteps(int pixelFeedback){ 
  if(stepConversionType == "Linear")
    return (int)(pixelFeedback * motorMultiplier);
  if(stepConversionType == "SquareRoot")
    return (int)(motorMultiplier * pow(pixelFeedback, 0.5);
  return (int)(motorMultiplier * pow(pixelFeedback, power));
}

//Checks if a character is an integer, also includes the
// negative sign for usage.
//Returns True if inChar is in "-0123456789"
//Note: Does NOT include decimal points
boolean isInt(char inChar){
  boolean isNegative = inChar == 45;
  boolean isNumber = false;
  if(inChar > 47 && inChar < 58){
    isNumber = true;
  }
  return isNegative || isNumber;
}
