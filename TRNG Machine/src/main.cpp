#include "depend.h"
#include "frames.h"
#include "trng.h"
#include "audio.h"
#include "keypadController.h"
#include "fingerprint.h"
#include "radiationEyeCandy.h"
#include "debugger.h"
#include "auth.h"
#include "menu.h"

void setup() {
  Serial.begin(9600);

  //Pins
  pinMode(buzzerPin, OUTPUT);  // sets the pin as output
  //Bop
  analogWrite(buzzerPin, 250);
  pinMode(keyPin, INPUT);
  pinMode(switchPin, INPUT);
  pinMode(radPowerPin, OUTPUT);
  
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
  
  delay(1500);

  //top notch security lamo
  authenticate();
}

void loop() {
  //Toggle the gieger counter according to the switch state
  checkSwitch();
  digitalWrite(radPowerPin, giegerCounterActive);
  
  //Render screens/menus
  renderLoop();

  if (giegerCounterActive){
    if (completeSeed()){
      updateSeedString();
      Serial.println("SEED:"+seedStr);
      b2d();
      resetSeed();
    }
    
    //Get Deltatime
    unsigned long currentMillis = millis();
    float deltaTime = (currentMillis - previousMillis)/LOG_PERIOD;

    //Smooth Needle
    smoothGuage(deltaTime);

    //Update seed string
    updateSeedString();
    
    //Set new info and calc new needle pos
    if(deltaTime >= 1.0){
      calcNewGuageInfo(currentMillis);
    }
    //Draw seed string on left screen
    else{
      //displaySeedProgress();
    }
  }

  //Reset buzzer
  analogWrite(buzzerPin, 0);

  delay(100);
}