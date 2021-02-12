#pragma once
#include "depend.h"

//Checks fingerprint database
int matchPrint(){
  uint8_t f = finger.getImage();
  if (f != FINGERPRINT_OK)  {
    return -1;
  }

  f = finger.image2Tz();
  if (f != FINGERPRINT_OK)  return -1;

  f = finger.fingerSearch();
  
  if (f != FINGERPRINT_OK){
    if (f == FINGERPRINT_NOTFOUND){
      return -9999;    
    }
    return -1;
  }
  //We found the finger print!
  return finger.fingerID;
}
