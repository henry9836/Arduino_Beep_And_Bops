#pragma once
#include "depend.h"

enum SOUNDS{
    ERROR,
    DENIED,
    ACCESS
};

void playSound(SOUNDS sound){
  switch (sound)
  {
  case ACCESS:
    analogWrite(buzzerPin, 150);
    delay(150);
    analogWrite(buzzerPin, 350);
    delay(150);
    analogWrite(buzzerPin, 550);
    delay(150);
    analogWrite(buzzerPin, 650);
    delay(150);
    break;
  case DENIED:
    analogWrite(buzzerPin, 1023);
    delay(150);
    analogWrite(buzzerPin, 350);
    delay(150);
    break;
  case ERROR:
    analogWrite(buzzerPin, 550);
    delay(1000);
    analogWrite(buzzerPin, 0);
    break;
  default:
    break;
  }
  analogWrite(buzzerPin, 0);
}