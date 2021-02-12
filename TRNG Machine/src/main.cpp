#include "depend.h"
#include "frames.h"
#include "trng.h"
#include "audio.h"
#include "keypadController.h"
#include "fingerprint.h"
#include "radiationEyeCandy.h"
#include "debugger.h"

void setup() {
  Serial.begin(9600);

  //Buzzer
  pinMode(buzzerPin, OUTPUT);  // sets the pin as output
  analogWrite(buzzerPin, 250);

  //Test Pin
  pinMode(keyPin, INPUT);
  
  //seed
  resetSeed();
  
  //Displays
  DIS_RIGHT.begin();
  DIS_RIGHT.clearBuffer();
  DIS_RIGHT.setFont(u8g2_font_pressstart2p_8f);  
  DIS_RIGHT.drawXBM(0, 0, 128, 64, STARTUPLOGO);
  DIS_RIGHT.sendBuffer();        
  
  DIS_LEFT.begin();
  DIS_LEFT.clearBuffer();
  DIS_LEFT.setFont(u8g2_font_p01type_tf);
  
  //Boot screen
  logScreen("System Starting", OK);
  logScreen("Screens Initialized", OK);
  analogWrite(buzzerPin, 0);
  
  //Keypad
  keypad.addEventListener(keypadInput); //add an event listener for this keypad

  //Geiger Counter
  multiplier = MAX_PERIOD / LOG_PERIOD;      //calculating multiplier, depend on your log period
  attachInterrupt(digitalPinToInterrupt(2), tube_impulse, FALLING); //define external interrupts 

  //Do a range test
  gaugeTest();
  
  //Fingerprint Sensor
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    logScreen("Fingerprint Check", OK);
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    logScreen("Fingerprint Check Failed", ERR);
  }
  analogWrite(buzzerPin, 0);
  
  finger.getParameters();
  finger.getTemplateCount();

  //Finish Setup
  logScreen("Ready.", OK);
  Serial.println("Ready.");
  
  //Reset screen left to small font
  DIS_LEFT.setFont(u8g2_font_micro_mn);
  delay(1500);
}


void loop() {
  if (constrain(analogRead(keyPin), 0, 1023) == 1023){
    Serial.println("ON");
  }
  else{
    Serial.println("OFF");
  }
  Serial.println(constrain(analogRead(keyPin), 0, 1023));
  
  if (completeSeed()){
    Serial.println("Have complete seed!");
    Serial.println(seedStr);
    Serial.println("--------------------");
    resetSeed();
  }
  
  //Get Deltatime
  unsigned long currentMillis = millis();
  float t = (currentMillis - previousMillis)/LOG_PERIOD;
  
  //Smooth Needle
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

  //Check fingerprint
  int f = matchPrint();
  //Correct
  if (f >= 0){
    Serial.println("GOT RET: ");
    Serial.println(f);
    playSound(ACCESS);
  }
  //Wrong
  else if (f == -9999){
    Serial.println("GOT RET: ");
    Serial.println(f);
    playSound(DENIED);
  }

  //Show seed
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

  //Set new info and calc new needle pos
  if(t >= 1.0){
    previousMillis = currentMillis;
    cpm = counts * multiplier;
    sv = cpm * TUBE_MOD;
    
    //update guage
    updateGuage(sv, true);

    //reset counts
    counts = 0;
  }
  else{
    DIS_LEFT.clearBuffer();
    DIS_LEFT.drawStr(0, 43, seedStr.c_str()); 
    DIS_LEFT.sendBuffer(); 
  }

  //Keypad
  char key = keypad.getKey();

  if (key) {
    Serial.println(key);
  }

  //Reset buzzer
  analogWrite(buzzerPin, 0);
}