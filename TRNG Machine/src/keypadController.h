#pragma once
#include "depend.h"

//Handle Input From The Keypad
void keypadInput(KeypadEvent key){
   switch (keypad.getState()){
    case PRESSED:
      analogWrite(buzzerPin, 350);
      Serial.println(key);
    break;
    case RELEASED:
      //Serial.println(key);
    break;
    case HOLD:
      analogWrite(buzzerPin, 350);
      Serial.println(key);
    break;
  }
}