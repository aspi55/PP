/*
adc singals from hall sensors. 
It's need to control current on window lifter
When window is close the current seek to the maximum. 
Hall sensor has detected this and sets status flag.
*/


void adcLeft() {
  curLeft = analogRead(left_hall);
// Serial.print("LEFT ADC A6 ");
// Serial.println(curLeft);
    if((curLeft < 250)||(curLeft > 750)){
                //    Serial.println("LEFT OVER CURRENT!");
      currentmillis = millis();
      if(currentmillis - timerADC >= timeAfterMax){
      timerADC = currentmillis;
            //      Serial.println("LEFT LOCK END");
      stateLeft = 0;
      }
    }
}

 void adcRight (){
  curRight = analogRead(right_hall);
// Serial.print("RIGHT ADC A7 ");
// Serial.println(curRight);
 
    if((curRight < 250)||(curRight > 750)){
      currentmillis = millis();
    //          Serial.println("RIGHT OVER CURRENT!");
      if(currentmillis - timerADC >= timeAfterMax){
        timerADC=currentmillis;
      //        Serial.println("RIGHT LOCK END");
        stateRight = 0;
      }
    }
}
