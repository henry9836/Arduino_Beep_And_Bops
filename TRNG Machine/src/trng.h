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

//Used in standalone mode
int standalongRNG(){

  //Use arduino's libary for rng using the seed we generated from uranium
  randomSeed(lastCompleteSeed);
  Serial.println(lastCompleteSeed);

  if (standAloneEnd > standAloneStart){
    return random(standAloneStart, standAloneEnd);
  }
  else{
    return random(standAloneEnd, standAloneStart);
  }
}

//Convert byte[] seed into a unsigned long, 
//used to be uint_64 but arduino doesn't seem to like large 64 bit numbers
unsigned long b2d(){
  overrideInterupt = true;
  unsigned long val = 0;

  if (!completeSeed()){
    return val;
  }

  for (size_t i = 0; i < 31; i++)
  {
    val = (val << 1) + seed[i];
  }
 
  overrideInterupt = false;
  lastCompleteSeed = val;
  return val;
}
