

/*--- INPUTS INITIALIZATION ---*/
void buttons_init(){
  pinMode(leftAuto_pin, INPUT_PULLUP);
  pinMode(leftUp_pin, INPUT_PULLUP);
  pinMode(leftDown_pin, INPUT_PULLUP);
  pinMode(rightAuto_pin, INPUT_PULLUP);
  pinMode(rightUp_pin, INPUT_PULLUP);
  pinMode(rightDown_pin, INPUT_PULLUP);
  pinMode(ignition_pin, INPUT_PULLUP);
  pinMode(outsource_pin, INPUT_PULLUP);
}

/*--- OUTPUTS INITIALIZATION ---*/
void relays_init(){
  pinMode(light_pin, OUTPUT);
  pinMode(relayLeftUp, OUTPUT);
  pinMode(relayLeftDown, OUTPUT);
  pinMode(relayRightUp, OUTPUT);
  pinMode(relayRightDown, OUTPUT);
}


/*--- CHECK INPUTS IN LOOP ---*/
void checkInputs(){
  LeftAuto.poll(!digitalRead(leftAuto_pin));
  LeftUp.poll(!digitalRead(leftUp_pin));
  LeftDown.poll(!digitalRead(leftDown_pin));
  RightAuto.poll(!digitalRead(rightAuto_pin));
  RightUp.poll(!digitalRead(rightUp_pin));
  RightDown.poll(!digitalRead(rightDown_pin));
  ignSignal.poll(!digitalRead(ignition_pin));
  AutoCloseWind.poll(!digitalRead(outsource_pin));
}


      