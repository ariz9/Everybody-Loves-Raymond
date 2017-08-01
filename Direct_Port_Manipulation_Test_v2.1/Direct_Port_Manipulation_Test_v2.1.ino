unsigned long sensorLeft[3];
unsigned long sensorRight[3];
byte sensorDirection[2];

/*PINS
 * Pin2: frequency input from left sensor
 * Pin3: frequency input from right sensor
 * Pin4: S3 of both sensors
 * Pin5: S4 of both sensors
 */

void setup() {
  // put your setup code here, to run once:
  DDRD= 0b11110011;
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //unsigned long timer= micros();  //--for counting time
  pulseCount();
  determineDirection();
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

void pulseCount(){ 
  //count pulse period
  //lower period= higher freq= intense color
  unsigned long startMicros;
  byte state;
  byte k=0;

  for (byte i=0; i<2; i++){
    for (byte j=i; j<2; j++){
      startMicros= micros();
      PORTD= (i<<PD4 & j<<PD5);
      state=0b00001100;
      while (state == 0b00001100){
        state= (PIND &(1<<PD2) & (1<<PD3)) ;
      }
      sensorLeft[k]= micros()-startMicros;
      sensorRight[k]= micros() -startMicros;
      switch (state){
        case 0b00000100:
          while( state== 0b00000100){
            state= (PIND & (1<<PD2));
          }
          sensorLeft[k]= micros()-startMicros;
        break;
        case 0b00001000:
          while( state== 0b00000100){
            state= (PIND & (1<<PD3));
          }
          sensorRight[k]= micros()-startMicros;
        break;
        }
      k++;
    }
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

void moveForward(){
  
}

void moveLeft(){

}

void moveRight(){
  
}
void moveBack(){
  
}

