#include <math.h>
#include<stdio.h>
#include<stdlib.h>
#include <LiquidCrystal.h>

//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LiquidCrystal lcd(11, 10, 9, 7, 4, 3);

enum request{INVALID, POWER, GETSTATS, CHANGETEMPTARGET, CHANGESENSORTARGET, CHANGETHRESHOLD};

const int screenContrastPin = A0;
const int relayPin = 13;
const int tempPin1 = A2; //output, airflow temp
const int tempPin2 = A4; //input, room temp
const int switchPin = 2; //power switch

unsigned long beforeTime;

int targetSensor = tempPin2;
int targetTemp = 21;
int contrast = 0;
int threshold = 1;
bool relayOverride = false;
bool currentRelayMode = false;

double readThermal(int pin){
  double currentTemp;
  currentTemp = log(((10240000/analogRead(pin)) - 10000));
  currentTemp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * currentTemp * currentTemp ))* currentTemp );
  currentTemp = currentTemp - 273.15; // Convert Kelvin to Celcius
  return currentTemp;
}

void clearSerial(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}

void sendStats(){
  Serial.print("OVERRIDE:");
  Serial.println(relayOverride);
  Serial.print("TARGET TEMPERATURE:");
  Serial.println(targetTemp);
  Serial.print("ROOM TEMPERATURE:");
  Serial.println(readThermal(tempPin2));
  Serial.print("AIRFLOW TEMPERATURE:");
  Serial.println(readThermal(tempPin1));
  Serial.print("TARGET SENSOR:");
  if (targetSensor == tempPin2){
    Serial.println("ROOM");
  }
  else{
    Serial.println("AIRFLOW");
  }
  Serial.print("THRESHOLD:");
  Serial.println(threshold);
  Serial.println("&");
}

//0123456789012345
//#1:1############
//#2:#############
//COMMAND STRUCTURE "#CMDNUMBER:DATA#" must be 16 long
void processCmd(){
  char cmd[15]; //buffer for a command

  //Read serial into cmd array
  int i = 0;
  while(Serial.available() > 0) {
    cmd[i] = Serial.read();
    i++;
    if (i > 15){
      break;
    }
  }
  //Sanity
  if ((cmd[0] == '#' && cmd[15] == '#')){
    Serial.println("Valid");
    
    //Convert number to number
    char cmdId[] = {cmd[1]};
    char cmdData[] = {cmd[3], cmd[4], cmd[5], cmd[6], cmd[7], cmd[8], cmd[9], cmd[10], cmd[11], cmd[12], cmd[13], cmd[14], cmd[15]};
    int id = atoi(cmdId);
    int data = atoi(cmdData);
    
    //Power request
    //Used to override relay switch to the off position otherwise automatic mode applies
    if (id == POWER){
      //Extract bool
      relayOverride = data;
      sendStats();
    }
    //Displays all the info
    else if (id == GETSTATS){
      sendStats();
    }
    //Change target temp
    else if (id == CHANGETEMPTARGET){
      targetTemp = data;
      sendStats();
    }
    //Change which sensor is used for getting the temperture
    else if (id == CHANGESENSORTARGET){
      if (targetSensor == tempPin2){
        targetSensor = tempPin1;
      }
      else{
        targetSensor = tempPin2;
      }
      sendStats();
    }
    else if (id == CHANGETHRESHOLD){
      threshold = data;
      sendStats();
    }
    //Error
    else{
      Serial.println("Request Not Found!");
      Serial.println("&");
    }
  }
  else{
    Serial.println("Malformed Request");
    Serial.println(cmd);
    Serial.println("&");
  }
  

  //Clear anything leftover
  clearSerial();
}

void setup(){
  Serial.begin(9600);
  analogWrite(screenContrastPin, 70);
  pinMode(switchPin, INPUT);

  //Setup Relay
  currentRelayMode = false;
  digitalWrite(relayPin, LOW);

  //Setup LCD
  lcd.begin(16, 2);
  lcd.print("hi");
  lcd.setCursor(0,1);
  lcd.print("it works!");
}

void loop(){
  
  delay(1000); 

  //Command Pending
  if (Serial.available() != 0){
    processCmd();
  }

  if (!relayOverride){
    //If the room is hotter than the target make the fan spin
    if ((targetTemp < readThermal(targetSensor)) && !currentRelayMode){
      digitalWrite(relayPin, HIGH);
      currentRelayMode = true;
    }
    //Once we are below our target + threshold then stop the fan
    else if ((targetTemp - threshold > readThermal(targetSensor)) && (currentRelayMode)){
      digitalWrite(relayPin, LOW);
      currentRelayMode = false;
    }
  }
  else{
    if (currentRelayMode){
      digitalWrite(relayPin, LOW);
      currentRelayMode = false;
    }
  }
}