#pragma once
#include "depend.h"
#include "audio.h"
#include "fingerprint.h"
#include "frames.h"

void authenticate(){

    if (DEBUGMODE){
        return;
    }

    //Key
    //Display
    DIS_LEFT.clearBuffer();
    DIS_LEFT.drawXBM(0, 0, 128, 64, KEY);
    DIS_LEFT.sendBuffer();
    while (true)
    {
        //Key turned
        if (constrain(analogRead(keyPin), 0, 1023) == 1023){
            break;
        }
        delay(50);
    }
 
    DIS_LEFT.setFont(u8g2_font_7x14B_tf);
    //Fingerprint
    while (true)
    {
        //Try and read a fingerprint
        int f = matchPrint();

        //Matches
        if (f >= 0){
            //Display
            DIS_LEFT.clearBuffer();
            DIS_LEFT.drawStr(26, 33, "ACCESS");
            DIS_LEFT.drawStr(23, 48, "GRANTED");
            DIS_LEFT.drawXBM(87, 4, 41, 56, FINGERPRINT);
            DIS_LEFT.sendBuffer();
            playSound(ACCESS);
            break;
        }
        //Doesn't Match
        else if (f == -9999){
            //Display
            DIS_LEFT.clearBuffer();
            DIS_LEFT.drawStr(25, 33, "ACCESS");
            DIS_LEFT.drawStr(25, 48, "DENIED");
            DIS_LEFT.drawXBM(87, 4, 41, 56, FINGERPRINT);
            DIS_LEFT.sendBuffer();
            playSound(DENIED);
        }
        //No Finger
        else{
            //Display
            DIS_LEFT.clearBuffer();
            DIS_LEFT.drawStr(20, 33, "AWAITING");
            DIS_LEFT.drawStr(10, 48, "FINGERPRINT");
            DIS_LEFT.drawXBM(87, 4, 41, 56, FINGERPRINT);
            DIS_LEFT.sendBuffer();
        }
        
        delay(500);
    }
}