unsigned long sensorLeft[3];    //Save the RGB value of the left sensor
unsigned long sensorRight[3];   //Save the RGB value of the right sensor
byte sensorDirection[2];        //Save which direction the robot should go

/*PINS
 * Pin2: frequency input from left sensor
 * Pin3: frequency input from right sensor
 * Pin4: S3 of both sensors
 * Pin5: S4 of both sensors
 */

void setup() {
  // put your setup code here, to run once:
  DDRD= 0b11110011;   //Set pin 2 and 3 to input
  Serial.begin(9600);   //for debugging through the serial port, comment to deactivate debugging and before competition (Saves a lot of processing time)
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
 
  /*//for logging
  Serial.print(sensorLeft[0]);
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

//The function that actually detects the RGB value
void pulseCount(){    
  //count pulse period of each RGB values
  //lower period= higher freq= intense color
  
  //read red (0,0)
  unsigned long startMicros= micros();    //Get the processor time in microsecond, as a starting point of the pulse period
  PORTD= (0<<PD4 & 0<<PD5);   //Set S3 to 0 and S4 to 0, this selects the Red channel
  byte state=0b00001100;    //Initial start for the state of the pin for the output of both sensors, trigger the while function
  while (state==0b00001100){
    state= (PIND & (1<<PD2) & (1<<PD3));    //Check the state of Pin 2 and 3, the output of both sensors
  }
  //At this stage, either one of the sensor, or both sensors has ended their pulse, but we dont know which one, so record the period of both for now
  sensorLeft[0]= micros()-startMicros;      
  sensorRight[0]= micros() -startMicros;
  //Now check which pin is still active
  switch (state){
    case 0b00000100:    //Pin 2(sensor left) is still active
      while( state== 0b00000100){
        state= (PIND & (1<<PD2));
      }
      sensorLeft[0]= micros()-startMicros;  //Record the actual period
    break;
    case 0b00001000:   //Pin 3(sensor right) is still active
      while( state== 0b00000100){
        state= (PIND & (1<<PD3));
      }
      sensorRight[0]= micros()-startMicros;
    break;
    
  }

  //Do the same for green and blue
  //read green (1,1)
  startMicros= micros();
  PORTD= (1<<PD4 & 1<<PD5);
  state=0b00001100;
  while (state==0b00001100){
    state= (PIND & (1<<PD2) & (1<<PD3)); 
  }
  sensorLeft[1]= micros()-startMicros;
  sensorRight[1]= micros() -startMicros-4;
  switch (state){
    case 0b00000100:
      while( state== 0b00000100){
        state= (PIND & (1<<PD2));
      }
      sensorLeft[1]= micros()-startMicros;
    break;
    case 0b00001000:
      while( state== 0b00000100){
        state= (PIND & (1<<PD3));
      }
      sensorRight[1]= micros()-startMicros;
    break;
    
  }

  //read blue (1,1)
  startMicros= micros();
  PORTD= (0<<PD4 & 1<<PD5);
  state=0b00001100;
  while (state==0b00001100){
    state= (PIND & (1<<PD2) & (1<<PD3)); 
  }
  sensorLeft[2]= micros()-startMicros;
  sensorRight[2]= micros() -startMicros-4;
  switch (state){
    case 0b00000100:
      while( state== 0b00000100){
        state= (PIND & (1<<PD2));
      }
      sensorLeft[2]= micros()-startMicros;
    break;
    case 0b00001000:
      while( state== 0b00000100){
        state= (PIND & (1<<PD3));
      }
      sensorRight[2]= micros()-startMicros;
    break;
    
  }

  

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


//The following functions are the one that actually controls the motor and H bridge
void moveForward(){
  
}

void moveLeft(){

}

void moveRight(){
  
}
void moveBack(){
  
}

