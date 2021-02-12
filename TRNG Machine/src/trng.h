#pragma once
#include "depend.h"

//INTERUPT CAUSED BY RADIATION
void tube_impulse(){
  if (overrideInterupt)
    return;

  //Check for dupes
  unsigned long currentTime = millis();
  if (currentTime != lastCoinTime){
    lastCoinTime = currentTime;
    counts++;
    //Get bit by millis
    int s = currentTime % 2;
    for (int y = 0; y < 31; y++){
      if (seed[y] == seedEmpty){
        if (s == 0){
          seed[y] = 0x00;
        }
        else{
          seed[y] = 0x01;
        }
        return;
      }
    }
  }
}

//Do we have a complete seed?
bool completeSeed(){
  for (int y = 0; y < 31; y++){
    if (seed[y] == seedEmpty){
        return false;
    }
  }
  return true;
}

//Set all seed vals to 0x00
void resetSeed(){
  for (int y = 0; y < 31; y++){
    seed[y] = seedEmpty;
  }
}

//Convert byte[] seed into a uint64_t
uint64_t b2d(){
  overrideInterupt = true;
  uint64_t val = seedt[0];

  for (int i = 1; i < 31; i += 1){
      if (seedt[i] == 0x01){
        //POW func that can handle a uint
        uint64_t thebigone = 2;
        for (int p = 1; p < i; p += 1){
          thebigone *= (uint64_t)2;
        }
        val += thebigone;
      }
  }
  
  overrideInterupt = false;
  return val;
}
