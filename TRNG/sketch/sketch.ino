#include <Key.h>
#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Fingerprint.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <U8g2lib.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define LOG_PERIOD 15000.0  //Logging period in milliseconds, recommended value 15000-60000.
#define MAX_PERIOD 60000.0  //Maximum logging period without modifying this sketch
float TUBE_MOD = 0.00812037; //Tube modifier
#define SVMAX 6.0 //Max SV estimated
#define fingerSerial Serial1

unsigned long counts;     //variable for GM Tube events
unsigned long cpm;        //variable for CPM
unsigned int multiplier;  //variable for calculation CPM in this sketch
unsigned long previousMillis;  //variable for time measurement

const byte seedEmpty = 0x02; 

byte seed[31];
char cpmBuf[4];

bool coin = false;

const byte KEYROWS = 4;
const byte KEYCOLS = 4;

char keypadArray[KEYROWS][KEYCOLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char cross[] = {
  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xE0, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x70, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 
  0xC0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x80, 0x03, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x1C, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 
  0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0E, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 
  0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x01, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 
  0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xC0, 0x00, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0xE0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 
  0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x70, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x80, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 
  0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0C, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 
  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x0C, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 
  0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x03, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 
  0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0C, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0xC0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xC0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 
  0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0xC0, 0x01, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 
  0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0E, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 
  0xC0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x80, 0x03, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x1C, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 
  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xE0, };

byte rowPins[KEYROWS] = {36, 34, 32, 30}; 
byte colPins[KEYCOLS] = {28, 26, 24, 22};

float sv = 0.0;
float targetXPos = 0.0;
float oldXPos = 0.0;
float targetYPos = 0.0;
float oldYPos = 0.0;

const int buzzerPin = 8;
unsigned long lastCoinTime = 0;

//Use MEGA Communcation pins
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

//Create keypad
Keypad keypad = Keypad(makeKeymap(keypadArray), rowPins, colPins, KEYROWS, KEYCOLS); 

//Create displays
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI DIS_LEFT(U8G2_R0, /* clock=*/ 3, /* data=*/ 4, /* cs=*/ 7, /* dc=*/ 6, /* reset=*/ 5);
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI DIS_RIGHT(U8G2_R0, /* clock=*/ 9, /* data=*/ 10, /* cs=*/ 13, /* dc=*/ 12, /* reset=*/ 11);

//INTERUPT CAUSED BY RADIATION
void tube_impulse(){

  unsigned long currentTime = millis();

  if (currentTime != lastCoinTime){
    Serial.println(String(currentTime) + ":L" + String(lastCoinTime)); 
    int s = millis() % 2;
    lastCoinTime = currentTime;
    counts++;
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

bool completeSeed(){
  for (int y = 0; y < 31; y++){
    if (seed[y] == seedEmpty){
        return false;
    }
  }

  return true;
}

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

void resetSeed(){
  for (int y = 0; y < 31; y++){
    seed[y] = seedEmpty;
  }

}

void updateGuage(float val, bool test){
    
    //update values
    oldXPos = targetXPos;
    oldYPos = targetYPos;

    //Get point on screen
    targetXPos = (val/SVMAX) * SCREEN_WIDTH;
    targetYPos = pow(((0.08*(targetXPos)) - (64.0*0.08)),2);

    if (test){
      String displayText = String("sV/hr:" + String(val));
      DIS_RIGHT.clearBuffer();
      DIS_RIGHT.drawLine(64, 60, targetXPos, targetYPos);
      DIS_RIGHT.drawStr(25, 45, displayText.c_str());
      DIS_RIGHT.drawCircle(64, 60, 6);
      DIS_RIGHT.sendBuffer();
    }
    
}

void gaugeTest(){
  
  for (float i = 0.0f; i < SVMAX; i += 0.1f){
    updateGuage(i, true);
  }
  updateGuage(0, true);
  DIS_RIGHT.clearBuffer();
  DIS_RIGHT.drawLine(0, 0, 128, 64);
  DIS_RIGHT.drawLine(0, 64, 128, 0);
  DIS_RIGHT.drawStr(32, 32, "STANDBY");  // write something to the internal memory
  DIS_RIGHT.sendBuffer();
  
}

void keypadInput(KeypadEvent key){
   switch (keypad.getState()){
    case PRESSED:
      analogWrite(buzzerPin, 350);
      Serial.println(key);
    break;
    case RELEASED:
      //Serial.println(key);
    break;
    case HOLD:
      analogWrite(buzzerPin, 350);
      Serial.println(key);
    break;

    delay(5);
  }
}

int matchPrint(){
  uint8_t f = finger.getImage();
  if (f != FINGERPRINT_OK)  {
    return -1;
  }

  f = finger.image2Tz();
  if (f != FINGERPRINT_OK)  return -1;

  f = finger.fingerSearch();
  
  if (f != FINGERPRINT_OK){
    if (f == FINGERPRINT_NOTFOUND){
      return -9999;    
    }
    return -1;
  }
  //We found the finger print!
  return finger.fingerID;
}

void setup() {
  Serial.begin(9600);

  //seed
  resetSeed();

  //finger print sensor
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getParameters();
  finger.getTemplateCount();

  //Displays
  DIS_RIGHT.begin();
  DIS_RIGHT.clearBuffer();
  DIS_RIGHT.setFont(u8g2_font_pressstart2p_8f);  
  DIS_RIGHT.drawXBM(0, 0, 128, 64, cross);
  DIS_RIGHT.sendBuffer();        

  DIS_LEFT.begin();
  DIS_LEFT.clearBuffer();
  DIS_LEFT.setFont(u8g2_font_micro_mn);
  DIS_LEFT.drawStr(32, 32,"STANDBY"); 
  DIS_LEFT.sendBuffer();

  //Keypad
  keypad.addEventListener(keypadInput); //add an event listener for this keypad

  //Geiger Counter
  multiplier = MAX_PERIOD / LOG_PERIOD;      //calculating multiplier, depend on your log period
  attachInterrupt(digitalPinToInterrupt(2), tube_impulse, FALLING); //define external interrupts 

  //Buzzer
  pinMode(buzzerPin, OUTPUT);  // sets the pin as output

  gaugeTest();

  Serial.println("Ready.");
}

void loop() {
  if (completeSeed()){
    Serial.println("Have complete seed!");
    resetSeed();
  }
  
  //Flip coin
  coin = !coin;
  
  //Get Deltatime
  unsigned long currentMillis = millis();
  float t = (currentMillis - previousMillis)/LOG_PERIOD;
  

  //Smooth Needle
  
  String displayText = String("sV/hr:" + String(sv));
  DIS_RIGHT.clearBuffer();
  DIS_RIGHT.drawLine(64, 60, lerp(oldXPos, targetXPos, t), lerp(oldYPos, targetYPos, t));
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
 
  // put your main code here, to run repeatedly:
  int f = matchPrint();
  if (f >= 0){
  Serial.println("GOT RET: ");
  Serial.println(f);
    analogWrite(buzzerPin, 150);
    delay(150);
    analogWrite(buzzerPin, 350);
    delay(150);
    analogWrite(buzzerPin, 550);
    delay(150);
    analogWrite(buzzerPin, 650);
    delay(150);
    analogWrite(buzzerPin, 1023);
  }
  else if (f == -9999){
  Serial.println("GOT RET: ");
  Serial.println(f);
    analogWrite(buzzerPin, 1023);
    delay(150);
    analogWrite(buzzerPin, 150);
  }

  //Update stats
  String seedStr = "";
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
  if(t >= 1.0){
    previousMillis = currentMillis;
    cpm = counts * multiplier;
    sv = cpm * TUBE_MOD;
    
    String cpmStr = "CPM: " + String(cpm);
    String svStr = "sV/hr: " + String(sv);
    

    DIS_LEFT.clearBuffer();
    DIS_LEFT.drawStr(25, 23, cpmStr.c_str());
    DIS_LEFT.drawStr(25, 33, svStr.c_str());
    DIS_LEFT.drawStr(0, 43, seedStr.c_str());
    DIS_LEFT.sendBuffer();

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
  
  char key = keypad.getKey();

  if (key) {
    Serial.println(key);
  }

  analogWrite(buzzerPin, 0);
  
}
