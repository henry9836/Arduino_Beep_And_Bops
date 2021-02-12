#pragma once
#include "audio.h"
#include "radiationEyeCandy.h"

//Debugging
enum DEBUGTYPE{
  OK,
  WARN,
  ERR
};

String lines[10];
int debugLineDis = 10;
int nextDebugElement = 0;

//Logs debug info onto screen
void logScreen(String text, DEBUGTYPE type){
  DIS_LEFT.clearBuffer();
  DIS_LEFT.setFont(u8g2_font_p01type_tf);
  //Append Text To Lines
  if (type == OK){
    text = "[OK] " + text;
  }
  else if (type == WARN){
    text = "[WARN] " + text;
  }
  else{
    text = "[ERR] " + text;
  }
  lines[nextDebugElement] = text;

  //Draw all lines
  for (int i = 0; i < 10; i++){
    if (lines[i].length() > 0){
      DIS_LEFT.drawStr(0, (i * debugLineDis) + 4, lines[i].c_str());
    }
    else{
      break;
    }
  }
  nextDebugElement++;
  if (nextDebugElement > 10){
    nextDebugElement = 0;
  }
  DIS_LEFT.sendBuffer();
  if (type == ERR){
    playSound(ERROR);
  }
}

//Guage Tester Function
void gaugeTest(){
  
  for (float i = 0.0f; i < SVMAX; i += 0.1f){
    updateGuage(i, true);
  }
  
  updateGuage(0, true);
  DIS_RIGHT.clearBuffer();
  DIS_RIGHT.drawXBM(0, 0, 128, 64, STARTUPLOGO);
  DIS_RIGHT.sendBuffer();
  
  //Debug
  logScreen("Gauge Range Test Completed", OK);
}
