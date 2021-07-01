#pragma once
#include "depend.h"
#include "radiationEyeCandy.h"
#include "auth.h"
#include "frames.h"
#include "trng.h"

enum SCREENS{
    MAINMENU,
    SERIALGEN,
    STANDALONE,
    STANDALONESTART,
    STANDALONEEND,
    STANDALONERNG
};

SCREENS currentScreen = MAINMENU;

void renderLoop(){
    switch (currentScreen)
    {
    case MAINMENU:{
        DIS_LEFT.clearBuffer();
        DIS_LEFT.setFont(u8g2_font_amstrad_cpc_extended_8u);
        DIS_LEFT.drawStr(13, 10, "[ MAIN MENU ]");
        DIS_LEFT.drawStr(10, 20, "A - SERIAL");
        DIS_LEFT.drawStr(10, 30, "B - STANDALONE");
        DIS_LEFT.drawStr(10, 40, "C - LOGOUT");
        DIS_LEFT.sendBuffer();

        //Handle input
        char key = keypad.getKey();
        if (key) {
            if (key == 'A'){
                currentScreen = SERIALGEN;
            }
            else if (key == 'B'){
                currentScreen = STANDALONE;
            }
            else if (key == 'C'){
                authenticate();
            }
        }
        break;
    }
    case SERIALGEN:{
        //Handle input
        char key = keypad.getKey();
        if (key) {
            if (key == 'A' || key == 'B' || key == 'C' || key == 'D'){
                currentScreen = MAINMENU;
            }
        }
        if (giegerCounterActive){
            displaySeedProgress();
        }
        else{
            DIS_LEFT.clearBuffer();
            DIS_LEFT.setFont(u8g2_font_amstrad_cpc_extended_8u);
            DIS_LEFT.drawStr(30, 35, "[ ERROR ]");
            DIS_LEFT.drawStr(35, 45, "NO POWER");
            DIS_LEFT.sendBuffer();
        }
        break;
    }
    case STANDALONESTART:
    case STANDALONEEND:
    case STANDALONE:{
        //Handle input
        char key = keypad.getKey();
        
        if (giegerCounterActive){
            if (currentScreen == STANDALONE){

                String range = String(standAloneStart, DEC) + " - " + String(standAloneEnd, DEC);

                DIS_LEFT.clearBuffer();
                DIS_LEFT.setFont(u8g2_font_amstrad_cpc_extended_8u);
                DIS_LEFT.drawStr(0, 10, "TRNG RANGE");
                DIS_LEFT.drawStr(0, 20, range.c_str());
                DIS_LEFT.drawStr(0, 30, "A - START");
                DIS_LEFT.drawStr(0, 40, "B - STARTNUM");
                DIS_LEFT.drawStr(0, 50, "C - ENDNUM");
                DIS_LEFT.drawStr(0, 60, "D - BACK");
                DIS_LEFT.sendBuffer();

                if (key == 'A'){
                    currentScreen = STANDALONERNG;
                }
                else if (key == 'B'){
                    currentScreen = STANDALONESTART;
                }
                else if (key == 'C'){
                    currentScreen = STANDALONEEND;
                }
                else if (key == 'D'){
                    currentScreen = MAINMENU;
                }
            }
            else if (currentScreen == STANDALONEEND || currentScreen == STANDALONESTART){
                String tmp = "";
                if (currentScreen == STANDALONESTART){
                    tmp = String(standAloneStart, DEC);
                }
                else{
                    tmp = String(standAloneEnd, DEC);
                }

                DIS_LEFT.clearBuffer();
                DIS_LEFT.setFont(u8g2_font_amstrad_cpc_extended_8u);
                DIS_LEFT.drawStr(0, 10, "SET RANGE NUM");
                DIS_LEFT.drawStr(0, 20, "A - CONFIRM");
                DIS_LEFT.drawStr(0, 30, "B - DELETE");
                DIS_LEFT.drawStr(0, 50, tmp.c_str());
                DIS_LEFT.sendBuffer();

                if (key == 'A'){
                    currentScreen = STANDALONE;
                }
                else if (key == 'B'){
                    if (currentScreen == STANDALONESTART){
                        standAloneStart = 0;
                    }
                    else{
                        standAloneEnd = 0;
                    }
                }
                else if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == '0'){
                    tmp += key;
                    if (currentScreen == STANDALONESTART){
                        standAloneStart = tmp.toInt();
                    }
                    else{
                        standAloneEnd = tmp.toInt();
                    }

                }

            }
        }
        else{
            if (key) {
                if (key == 'A' || key == 'B' || key == 'C' || key == 'D'){
                    currentScreen = MAINMENU;
                }
            }
            DIS_LEFT.clearBuffer();
            DIS_LEFT.setFont(u8g2_font_amstrad_cpc_extended_8u);
            DIS_LEFT.drawStr(30, 35, "[ ERROR ]");
            DIS_LEFT.drawStr(35, 45, "NO POWER");
            DIS_LEFT.sendBuffer();
        }
        break;
    }
    case STANDALONERNG:{

        char key = keypad.getKey();
        if (key == 'A'){
            rngStr = String(standalongRNG(), DEC);
        }
        else if (key == 'D'){
            currentScreen = STANDALONE;
        }

        DIS_LEFT.clearBuffer();
        DIS_LEFT.setFont(u8g2_font_amstrad_cpc_extended_8u);
        DIS_LEFT.drawStr(0, 10, "NEW NUMBER");
        DIS_LEFT.drawStr(0, 20, rngStr.c_str());
        DIS_LEFT.drawStr(0, 30, "A - NEW NUM");
        DIS_LEFT.drawStr(0, 40, "D - BACK");
        DIS_LEFT.sendBuffer();
        break;
    }
    default:{
        Serial.println("defaulted");
        break;
    }
    }
}