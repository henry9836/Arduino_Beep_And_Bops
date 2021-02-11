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
String seedStr = "";

const byte KEYROWS = 4;
const byte KEYCOLS = 4;

char keypadArray[KEYROWS][KEYCOLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char STARTUPLOGO[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0x01, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 
  0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0xE0, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0xFF, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 
  0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0xC0, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0x01, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 
  0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xFC, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x3F, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 
  0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x0C, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 
  0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xF9, 
  0x9F, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xFE, 0xFF, 0xF9, 0x8F, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xF1, 0x8F, 0xFF, 0x7F, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xE3, 
  0xC7, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xFE, 0xFF, 0xC3, 0xE3, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x07, 0xE0, 0xFF, 0x7F, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0x0F, 
  0xF0, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xFC, 0xFF, 0x0F, 0xF0, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0x07, 0xE0, 0xFF, 0x3F, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0x07, 
  0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xF8, 0xFF, 0x03, 0xC0, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0x03, 0xC0, 0xFF, 0x0F, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0x01, 
  0x80, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xE0, 0xFF, 0x00, 0x00, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0x00, 0x00, 0xFF, 0x03, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x7F, 0x00, 
  0x00, 0xFE, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0x7F, 0x00, 0x00, 0xFE, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0xFC, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 
  0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF3, 0x88, 0xF0, 0x80, 
  0x8F, 0x6F, 0x26, 0x3C, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 
  0x98, 0x98, 0x98, 0x81, 0x88, 0x41, 0x26, 0x06, 0x01, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xC0, 0x90, 0x98, 0x08, 0x80, 0x98, 0x41, 0x22, 0x06, 
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x98, 0xB8, 0x08, 0x80, 
  0x98, 0x41, 0x22, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 
  0xF0, 0xA8, 0x08, 0x80, 0x98, 0xC7, 0x22, 0x02, 0x0F, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xC0, 0x78, 0xA8, 0xC8, 0x81, 0x98, 0xC7, 0x23, 0x02, 
  0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x50, 0xC8, 0x08, 0x81, 
  0x98, 0x81, 0x23, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 
  0xD0, 0xC8, 0x88, 0x81, 0x98, 0x81, 0x21, 0x02, 0x01, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xC0, 0x98, 0xC8, 0x98, 0x81, 0x88, 0x81, 0x21, 0x06, 
  0x01, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0xC0, 0x90, 0x89, 0xF0, 0x80, 
  0x8F, 0x8F, 0x21, 0x3C, 0x0F, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x23, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00};

byte seedt[33] = 
{
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01,
};

byte rowPins[KEYROWS] = {36, 34, 32, 30}; 
byte colPins[KEYCOLS] = {28, 26, 24, 22};

float sv = 0.0;
float targetXPos = 0.0;
float oldXPos = 0.0;
float targetYPos = 0.0;
float oldYPos = 0.0;

bool overrideInterupt = false;

const int buzzerPin = 8;
const int testPin = A1;
unsigned long lastCoinTime = 0;

//Use MEGA Communcation pins
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

//Create keypad
Keypad keypad = Keypad(makeKeymap(keypadArray), rowPins, colPins, KEYROWS, KEYCOLS); 

//Create displays
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI DIS_LEFT(U8G2_R0, /* clock=*/ 3, /* data=*/ 4, /* cs=*/ 7, /* dc=*/ 6, /* reset=*/ 5);
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI DIS_RIGHT(U8G2_R0, /* clock=*/ 9, /* data=*/ 10, /* cs=*/ 13, /* dc=*/ 12, /* reset=*/ 11);

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
    analogWrite(buzzerPin, 550);
    delay(1000);
    analogWrite(buzzerPin, 0);
  }
  //delay(500);
}

//INTERUPT CAUSED BY RADIATION
void tube_impulse(){
  if (overrideInterupt)
    return;

  //Check for dupes
  unsigned long currentTime = millis();
  if (currentTime != lastCoinTime){
    //Serial.println(String(currentTime) + ":L" + String(lastCoinTime)); 
    lastCoinTime = currentTime;
    counts++;
    //Get bit by millis
    int s = currentTime % 2;
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

//Handle Input From The Keypad
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
  }
}

//Do we have a complete seed?
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

//Set all seed vals to 0x00
void resetSeed(){
  for (int y = 0; y < 31; y++){
    seed[y] = seedEmpty;
  }

}

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

//Checks fingerprint database
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

//Convert byte[] seed into a uint64_t
uint64_t b2d(){
  overrideInterupt = true;
  uint64_t val = seedt[0];

  for (int i = 1; i < 31; i += 1){
      if (seedt[i] == 0x01){
        //POW func that can handle a uint
        uint64_t thebigone = 2;
        for (int p = 1; p < i; p += 1){
          thebigone *= (uint64_t)2;
        }
        val += thebigone;
      }
  }
  
  overrideInterupt = false;
  return val;
}

void setup() {
  Serial.begin(9600);

  //Buzzer
  pinMode(buzzerPin, OUTPUT);  // sets the pin as output
  analogWrite(buzzerPin, 250);

  //Test Pin
  pinMode(testPin, INPUT);
  
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
  if (constrain(analogRead(testPin), 0, 1023) == 1023){
    Serial.println("ON");
  }
  else{
    Serial.println("OFF");
  }
  Serial.println(constrain(analogRead(testPin), 0, 1023));
  
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
  //Wrong
  else if (f == -9999){
  Serial.println("GOT RET: ");
  Serial.println(f);
    analogWrite(buzzerPin, 1023);
    delay(150);
    analogWrite(buzzerPin, 150);
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
