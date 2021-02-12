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