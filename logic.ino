
  /*
  state = 0  - all off;
  state = 1 - up Press;
  state = 2 - down Press;
  state = 3 - autoUp activated;
  state = 4 - autoDn activated;  
  */

void left (bool up, bool down, bool autoLeft){
  if (!up && !down && (stateLeft == 1 || stateLeft == 2)) stateLeft = 0;      

 // if(autoLeft) Serial.println("autoleft");
  if (up && (stateLeft != 3)){
    stateLeft =  1;
    }
  if (down && (stateLeft != 4)) {
    stateLeft =  2;
    }
  if (up && autoLeft){
  //  Serial.println("!Left UP Start!");
    timeLeft = millis();
    stateLeft = 3;
  }
  if (down && autoLeft){
  //  Serial.println("!Left down Start!");
    timeLeft = millis();
    stateLeft =  4;
  }
}

void right (bool up, bool down, bool autoRight){
  if (!up && !down && (stateRight == 1 ||stateRight == 2)) stateRight = 0;

  if (up && (stateRight != 3)){
    stateRight = 1;
    }
  if (down && (stateRight != 4)) {
    stateRight = 2;
    }
  if (up && autoRight){
  //  Serial.println("!Right UP! Start!");
    timeRight = millis();
    stateRight = 3;
  }
  if (down && autoRight){
//    Serial.println("!Right down Start!");
    timeRight = millis();
    stateRight = 4;
  }
 /* 
  }*/
}



void work (){

  if(AutoCloseWind.click()){      //auto close windows after alarm signal
    leftStart = millis();
    left(true, false, true);      //left first
    autoClose = 1;                //flag close from alarm
   // Serial.println("alarm");
  }

  if(autoClose == 1 && stateLeft == 0){
    rightStart = millis();
    right(true, false, true);     //right second after left completely close
    autoClose = 0;
  }
    /*
  state = 0  - all off;
  state = 1 - up Press;
  state = 2 - down Press;
  state = 3 - autoUp activated;
  state = 4 - autoDn activated;  
  */


   switch(stateLeft){
    case 0:
        digitalWrite(relayLeftUp, LOW);
        digitalWrite(relayLeftDown, LOW);  
      break;
    case 1:
        digitalWrite(relayLeftDown, LOW);
        digitalWrite(relayLeftUp, HIGH);
    //    Serial.println("LEFT UP!");
      break;
    case 2:
        digitalWrite(relayLeftUp, LOW);
        digitalWrite(relayLeftDown, HIGH);
  //      Serial.println("LEFT DOWN!");
      break;
    case 3:
  //      Serial.println("auto LEFT UP!");
        digitalWrite(relayLeftUp, HIGH);
      adcLeft();        
   if (millis() - timeLeft > timeAuto){
        digitalWrite(relayLeftUp, LOW);
        stateLeft = 0;
        }
      break;     
    case 4:
//      Serial.println("auto DOWN!");
      digitalWrite(relayLeftDown, HIGH);
      adcLeft();  
    if (millis() - timeLeft > timeAuto){
        digitalWrite(relayLeftDown, LOW);
//        Serial.println("!left down END!");
        stateLeft = 0;
      }
      break;
   }
   switch(stateRight){
    case 0:
        digitalWrite(relayRightUp, LOW);
        digitalWrite(relayRightDown, LOW);  
      break;
    case 1:
        digitalWrite(relayRightDown, LOW);
        digitalWrite(relayRightUp, HIGH);
      break;
    case 2:
        digitalWrite(relayRightUp, LOW);
        digitalWrite(relayRightDown, HIGH);
      break;
    case 3:
      adcRight();
      digitalWrite(relayRightUp, HIGH);
//      Serial.println("!right UP AUTO!");  
      if (millis() - timeRight > timeAuto){
        digitalWrite(relayRightUp, LOW);
 //       Serial.println("!right UP END!");        
        stateRight = 0;
      }
      break;     
    case 4:
      adcRight();
      digitalWrite(relayRightDown, HIGH);
 //     Serial.println("!right DOWN AUTO!");  
      if (millis() - timeRight > timeAuto){
        digitalWrite(relayRightDown, LOW);
        stateRight = 0;
 //       Serial.println("!right down END!");
      }
      break;
   }
}


