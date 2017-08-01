/*
  V3 of the code
  
  utilises ISR more for (almost) parallel processing
  sensor reading are now regulated with timing
  ignore the first couple millisecond of sensor reading to allow noises to settle
  calculate and move the robot while wwaiting for this couple milisecond
  frequency of this reading can be adjusted

  Now (theoretically) should also support the robot to know specifically which RGB color are actually on, with minimum memory loss

  also frees up 2 (OE pins) pins, as OE are no longer needed to be controlled
 */

unsigned long sensorLeft[3];
unsigned long sensorRight[3];

volatile unsigned long sensorISRData[4];

unsigned long sensorDirection[2];


/*
  following is used by sensorDirection() to determine color using just 8 numbers
  sensorDirection array element will have the following value if
  R only= 1     RB= 4     RBG (white)= 9
  B only= 3     RG= 6     No color (black)= 0
  G only= 5     BG= 8
 */
#define red 1
#define blue 3
#define green 5
#define RB 4
#define RG 6
#define BG 8
#define RGB 9
#define white 9
#define noColor 0
#define black 0

//For movement
#define forward 1
#define left 2
#define hardLeft 21
#define reallyHardLeft 22
#define right 3
#define hardRight 31
#define reallyHardRight 32
#define backward 0

byte robot;

#define deadEnd 1
#define isLost 2
#define isOK 0

void setup() {
  // put your setup code here, to run once:
  DDRD= 0b00001100;
  Serial.begin(9600);
}

void loop() {
  //Every color should run every 6 ms
  byte color=0;
  for (byte i=1; i<2; i++){
    for (byte j=i; j<2; j++){
      color++;
      PORTD= (i<<PD4 & j<<PD5);
      unsigned long startMillis=millis();
      do{
        //calculation here
        decideDirection();
        robotMove();
        //logMove();
      }while (millis()-startMillis<4);
    
      EIFR= (1<<INT0 & 1<<INT1);  //clear ISR registry
      attachInterrupt(digitalPinToInterrupt(2),risingLeft,RISING);
      attachInterrupt(digitalPinToInterrupt(2),fallingLeft,FALLING);
      attachInterrupt(digitalPinToInterrupt(2),risingRight,RISING);
      attachInterrupt(digitalPinToInterrupt(2),fallingRight,FALLING);
    
      do{
        sensorLeft[color]= sensorISRData[2]-sensorISRData[0];
        sensorRight[color]= sensorISRData[3]-sensorISRData[1];
      }while (millis()-startMillis<6);
    } 
  }
}
  

void risingLeft(){
  sensorISRData[0]= micros();
}

void fallingLeft(){
  sensorISRData[2]= micros();
  detachInterrupt(digitalPinToInterrupt(2));
}

void risingRight(){
  sensorISRData[3]= micros();
}

void fallingRight(){
  sensorISRData[1]= micros();
  detachInterrupt(digitalPinToInterrupt(3));
}


void decideDirection(){
  //Try to follow a colored line
  sensorDirection[0]= 9;
  sensorDirection[1]= 9;
  for (byte i=1; i<4; i++){
    if (sensorLeft[i] >100 && sensorLeft[i] <200){   //change this range for no color
        sensorDirection[0]= sensorDirection[0]-(2*i)-1;   //-1 if no red, -3 if no blue, -5 if no green, see under for more detail
    }
    if (sensorRight[i] >100 && sensorLeft[i] <200){   //change this range for no color
        sensorDirection[1]= sensorDirection[1]-(2*i)-1;
    }
  }
}
/*
  sensorDirection array element will have the following value if
  R only= 1     RB= 4     RBG (white)= 9
  B only= 3     RG= 6     No color (black)= 0
  G only= 5     BG= 8
 */


void robotMove(){
  switch (robot){
    case isLost:
      
    break;
    case deadEnd:
      
    break;
    default:




      // check at the end if robot is lost or meet dead end
      robot = isOK;
    break;
  }
  

  /*
  if (robotIsLost = 0){
    if (robotIsDeadEnd =0){
      if (sensorDirection[0] != black || sensorDirection[0] !=white){
        if (sensorDirection[1] == black || sensorDirection[1] == white){
          //move forward
        }
        else if (sensorDirection[1]<=BG && sensorDirection[1] >=red){
          //turn left
        }
      }

      else if(sensorDirection[0] == black || sensorDirection[0] == white){
        if (sensorDirection[1] == black || sensorDirection[1] == white){
          //move forward
        }
        else if (sensorDirection[1]<=BG && sensorDirection[1] >=red){
          //turn right
        }
      }
    }
    else{
      //follow trough the log file
    }
  }
  else {
      //keep backing away slowly
  }*/
}

 

