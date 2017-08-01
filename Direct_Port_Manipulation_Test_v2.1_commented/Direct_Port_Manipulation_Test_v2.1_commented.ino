/*PINS
 * Pin2: frequency input from left sensor
 * Pin3: frequency input from right sensor
 * Pin4: S3 of both sensors
 * Pin5: S4 of both sensors
 * Pin6: OE of left sensors (enabled low)
 * Pin7: OE of right sensors (enabled low)
 */


#define leftPin 2
#define rightPin 3

//The following array contains RGB value of each sensors
unsigned long sensorLeft[3];    //[r,b,g]
unsigned long sensorRight[3];   //[r,b,g]

volatile unsigned long startMicros[2];  //contains the starting time for both sensors:  [startTime rightSensor, startTime leftSensor]

byte k; //for array indexing of sensorLeft/Right inside pulseCount() for loop

byte sensorDirection[2];  //Save which direction the robot should go

void setup() {
  // put your setup code here, to run once:
  DDRD= 0b11110011;   //Set pin 2 and 3 to input
  attachInterrupt(digitalPinToInterrupt(leftPin),recordTimeLeft,RISING);
  attachInterrupt(digitalPinToInterrupt(rightPin),recordTimeRight,RISING);
  
  Serial.begin(9600); //for debugging through the serial port, comment to deactivate debugging and before competition (Saves a lot of processing time)
}

void loop() {
  // put your main code here, to run repeatedly:
  //unsigned long timer= micros();  //--for counting time
  
  pulseCount();   //This reads the RGB values of BOTH sensors
  determineDirection();   //This processes the RGB Value into the decision making of the robot

  //This bit tells the robot to move in the appropriate direction. we can add more options (eg. move 45 degree) if wanted/ needed
  if ((sensorDirection[1]==3) && (sensorDirection[2]==3)){
    moveForward();
  //  Serial.println("^");
  }
  if ((sensorDirection[1]<3) && (sensorDirection[2]==3)){
    moveRight();
  //  Serial.println(">");
  }
  if ((sensorDirection[1]==3) && (sensorDirection[2]<3)){
    moveLeft();
  //  Serial.println("<");
  }
  if ((sensorDirection[1]<3) && (sensorDirection[2]<3)){
    moveBack();
  //  Serial.println("V");
  }
  
  //timer= micros() - timer; //--for counting time
  //Serial.println(timer); //--for counting time
 
  //for logging
  /*Serial.print(sensorLeft[0]);
  Serial.print(" ");
  Serial.print(sensorLeft[1]);
  Serial.print(" ");
  Serial.print(sensorLeft[2]);
  Serial.print("      ");
  Serial.print(sensorRight[0]);
  Serial.print(" ");
  Serial.print(sensorRight[1]);
  Serial.print(" ");
  Serial.println(sensorRight[2]); //*/
}


//This function reads both sensors RGB value
//This is done by reading the period of the square wave outputted by the sensor
//Lower period= higher freq= intense color
//This function also utilises the interrupt service routine (ISR) function, so dont change pin 2 and 3 (PD2 and PD3) as it is reserved for it
/*Pseudocode
 * Open pin 6 and 7 to allow output of both sensors
 * Select the RGB channel, by manipulating the output of pin 4 and 5
 * Wait for the signal to stabilises
 * Record the start of when the signal stabilises (using ISR)
 * Keep checking pin 2 and 3 until one of them returns low
 * Record both sensor period, as we dont know yet which pin actually turns low
 * Now check which pin turns low
 * Turn off the other pin OE (output high), to disable ISR for that pin
 * loop and keep checking that pin
 * Record the period of that pin
 * Do the same for all other color
 */

 /*notes:
  * -might add some frequency modulation for sample reading, so more consistent reading interval
  * -if frequency is added, might also move some of the calculations for the robots into here, so processing time isnt wasted
  */
void pulseCount(){
  byte state;
  k=0;  //zero the array index
  
  for (byte i=0; i<2; i++){     //Loop through RGB  (in RBG sequence) {(0,0), (0,1), (1,1)}
    for (byte j=i; j<2; j++){   
      PORTD= (i<<PD4 & j<<PD5 & 0<<PD6 & 0<<PD7);   //cycle through RGB channel (PD4 and PD5), and enable output of both sensors (low PD6 and PD7)
      //Keep refreshing pin 2 and 3 until both pin becomes high
      //This is to ensure that the signal stabilises before any measurement are taken
      //The timer for both sensors are started by the ISR, so no need to do micros() here
      do{
        state= (PIND & (1<<PD2) & (1<<PD3));    //read pin 2 and 3
      }while (state !=0b00001100);              //until both pins are high
      
      //Now while both pins are high, keep reading until one of the pin turns low
      do{
        state= (PIND & (1<<PD2) & (1<<PD3));    //read pin 2 and 3
      }while (state == 0b00001100);             //while both pins are high

      //At this point, one of the pin or both of them has return to low. Record both sensors timing, and check which one returns low afterwards
      sensorLeft[k]= micros()-startMicros[0];
      sensorRight[k]= micros()-startMicros[1];
      
      //Check which one returns to low. If there is still a sensor at high, keep reading that sensor
      switch (state){
        case 0b00000100:                //left sensor is still high
          PORTD= (1<<PD7);              //turns off right sensor, so it doesnt trigger the ISR again
          while( state== 0b00000100){   //keep reading while left sensor is still high
            state= (PIND & (1<<PD2));   //read pin 2, left sensor
          }
          //The sensor has now return low
          sensorLeft[k]= micros()-startMicros[0];  //Record the actual left sensor period
          PORTD= (1<<PD6);    //turns off left sensor
        break;
        //Do the same for the right sensor
        case 0b00001000:
          PORTD= (1<<PD6);
          while( state== 0b00000100){
            state= (PIND & (1<<PD3));
          }
          sensorRight[k]= micros()-startMicros[1];
          PORTD= (1<<PD7);
        break;
        //If both of them turns low at the same time, just need to deactivate OE of both pins
        case 0b00000000:
          PORTD= (1<<PD7 & 1<<PD6);
        break;
          
        }
      k++;
    }
  }
}

//Interrupt Service Routine function
//Function is only to record starting time, for higher precision start time
void recordTimeLeft(){
  startMicros[0]= micros();   //record start time for left sensor
}
void recordTimeRight(){
  startMicros[1]= micros();   //record start time for right sensor
}




void determineDirection(){
  //Reset previous sensor direction
  sensorDirection[1]= 0;
  sensorDirection[2]= 0;
  
  /* if both are black, then move forward (sensorDirection= {3,3})
   * if one of them is black, then move towards the black side slowly  (sensorDirection= {<3,3} or vice versa)
   * if none of them is black, then back away slowly till a black is found (hopefully) (sensorDirection= {<3,<3})
   */
   
   for (byte i=0; i<3; i++){
     if (sensorLeft[i] > 4){//NOT BLACK. change this value to change sensitivity
        sensorDirection[1]++;
     }
     if (sensorRight[i] > 4){//NOT BLACK. change this value to change sensitivity
        sensorDirection[2]++;
     }
   }
}

void moveForward(){
  
}

void moveLeft(){

}

void moveRight(){
  
}
void moveBack(){
  
}
