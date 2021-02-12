#pragma once
#include "depend.h"

//update Guage info or test it
void updateGuage(float val, bool test){
    
    //update values
    oldXPos = targetXPos;
    oldYPos = targetYPos;

    //Get point on screen
    targetXPos = (val/SVMAX) * SCREEN_WIDTH;
    targetYPos = pow(((0.08*(targetXPos)) - (64.0*0.08)),2);

    //Used for booting
    if (test){
      String displayText = String("sV/hr:" + String(val));
      DIS_RIGHT.clearBuffer();
      DIS_RIGHT.drawLine(64, 60, targetXPos, targetYPos);
      DIS_RIGHT.drawStr(25, 45, displayText.c_str());
      DIS_RIGHT.drawCircle(64, 60, 6);
      DIS_RIGHT.sendBuffer();
    }
}

//Smooth Needle
void smoothGuage(float t){
  String displayText = String("sV/hr:" + String(sv));
  DIS_RIGHT.clearBuffer();

  //targetYPos = pow(((0.08*(targetXPos)) - (64.0*0.08)),2);
  //Draw gauage
  float tmpPos = lerp(oldXPos, targetXPos, t);
  DIS_RIGHT.drawLine(64, 60, tmpPos, pow(((0.08*(tmpPos)) - (64.0*0.08)),2));
  DIS_RIGHT.setFont(u8g2_font_pressstart2p_8f);  
  DIS_RIGHT.drawStr(25, 45, displayText.c_str());
  DIS_RIGHT.drawCircle(64, 60, 6);
  DIS_RIGHT.drawLine(0, 0, 0, 20);
  DIS_RIGHT.drawLine(32, 0, 32, 15);
  DIS_RIGHT.drawLine(64, 0, 64, 10);
  DIS_RIGHT.drawLine(96, 0, 96, 15);
  DIS_RIGHT.drawLine(127, 0, 127, 20);
  DIS_RIGHT.setFont(u8g2_font_5x7_tn );  
  DIS_RIGHT.drawStr(0, 30, "0");
  DIS_RIGHT.drawStr(63, 20, "3");
  DIS_RIGHT.drawStr(123, 30, "6");
  DIS_RIGHT.sendBuffer();
}

void calcNewGuageInfo(unsigned long currentMillis){
    previousMillis = currentMillis;
    cpm = counts * multiplier;
    sv = cpm * TUBE_MOD;
    
    //update guage
    updateGuage(sv, true);

    //reset counts
    counts = 0;
}

void updateSeedString(){
    seedStr = "";
    for (int y = 0; y < 31; y++){
        if (seed[y] == seedEmpty){
            seedStr += '*';
        }
        else if (seed[y] == 0x01){
            seedStr += '1';
        }
        else{
            seedStr += '0';
        }
    }
}

void displaySeedProgress(){
    DIS_LEFT.clearBuffer();
    DIS_LEFT.drawStr(0, 43, seedStr.c_str()); 
    DIS_LEFT.sendBuffer(); 
}