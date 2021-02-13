#pragma once
#include <Key.h>
#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Fingerprint.h>
#include <U8g2lib.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define LOG_PERIOD 15000.0  //Logging period in milliseconds, recommended value 15000-60000.
#define MAX_PERIOD 60000.0  //Maximum logging period without modifying this sketch
#define SVMAX 6.0 //Max SV estimated
float TUBE_MOD = 0.00812037; //Tube modifier

//Pins
#define buzzerPin 8
#define radPowerPin 40
#define keyPin A0
#define switchPin A1
#define fingerSerial Serial1 //Fingerprint serial port

unsigned long counts;     //variable for GM Tube events
unsigned long cpm;        //variable for CPM
unsigned int multiplier;  //variable for calculation CPM in this sketch
unsigned long previousMillis;  //variable for time measurement

//rng
bool giegerCounterActive = false;
int standAloneStart = 0;
int standAloneEnd = 1;
byte seed[31];
char cpmBuf[4];
unsigned long lastCompleteSeed = 0;
int standAloneC = 0;
String seedStr = "";
String rngStr = "NULL";
const byte seedEmpty = 0x02; 

//Keypad
const byte KEYROWS = 4;
const byte KEYCOLS = 4;
byte rowPins[KEYROWS] = {36, 34, 32, 30}; 
byte colPins[KEYCOLS] = {28, 26, 24, 22};
const char keypadArray[KEYROWS][KEYCOLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
//Create keypad
Keypad keypad = Keypad(makeKeymap(keypadArray), rowPins, colPins, KEYROWS, KEYCOLS); 

float sv = 0.0;
float targetXPos = 0.0;
float oldXPos = 0.0;
float targetYPos = 0.0;
float oldYPos = 0.0;

bool overrideInterupt = false;
unsigned long lastCoinTime = 0;

//Use MEGA Communcation pins
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

//Create displays
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI DIS_LEFT(U8G2_R0, /* clock=*/ 3, /* data=*/ 4, /* cs=*/ 7, /* dc=*/ 6, /* reset=*/ 5);
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI DIS_RIGHT(U8G2_R0, /* clock=*/ 9, /* data=*/ 10, /* cs=*/ 13, /* dc=*/ 12, /* reset=*/ 11);

//Simple lerp func
float lerp(float a, float b, float x)
{
  if (x > 1.0){
    x = 1.0;
  }
  else if (x < 0.0){
    x = 0.0;
  }
  return a + x * (b - a);
}