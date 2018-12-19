//Built by Henry Oliver

int ledPin = 9;
int lightPin = 3;
int lighttime = 0;
int darktime = 0;
int brightness = 1023;
int lightsensor = 0;
int blinktime = 416;
int longtime = 1248;
int waittime = 1248;
int spacewaittime = 2912;
String charcterlist[36] = {"01", "1000", "1010", "100", "0", "0010","110", "0000", "00", "0111", "101", "0100","11","10","111","0110","1101","010","000","1","001","0001","011","1001","1011","1100","11111","01111","00111","00011","00001","00000","10000","11000","11100","11110"}; //abcdef...0013416789 (0=DOT and 1=DASH)
String tmp;
String tmp2;
String decodelist = "";
bool encodeMode = true;
bool chosenMode = false;
int invalue = 0;
char recievedChar;

void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("-.-- Morse Encoder/Decoder --.-");
  Serial.println("Built by Henry Oliver");
  Serial.println();
  Serial.print("Decode or Encode Mode? [E/d]: ");
}

int decode(int timeon){

  if ((timeon > 100) && (timeon < 500)){
    decodelist += ".";
  }
  else if ((timeon > 1000) && (timeon < 1300)){
    decodelist += "-";
  }
  else if ((timeon > 2500) && (timeon < 3000)){
    decodelist += "/";
  }
  
  return 0;
}

int encode(int character){
  tmp = charcterlist[character];
  for (int i=0; i <= (charcterlist[character]).length(); i++){
      tmp2 = tmp[i];
      if (tmp2 == "0"){
        Serial.println("SHORT");
        quickflash();
      }
      if (tmp2 == "1"){
        Serial.println("LONG");
        longflash();
      }
   }
  
  return 0;
}

void quickflash() {
  analogWrite(ledPin, brightness);
  delay(blinktime);
  analogWrite(ledPin, 0);
  wait();
}

void longflash() {
  analogWrite(ledPin, brightness);
  delay(longtime);
  analogWrite(ledPin, 0);
  wait();
}

int wait(){
  delay(waittime);
  return 0;
}

void loop() {
  if (chosenMode == false){
    if (Serial.available() > 0) {
      recievedChar = Serial.read();
      Serial.println(recievedChar);
      if (recievedChar == 100){
        encodeMode = false;
        Serial.println("encodeMode is false");
      }
      chosenMode = true;
    }
  }
  else{
    if (encodeMode){
      if (Serial.available() > 0) {
        recievedChar = Serial.read();
        Serial.println(recievedChar);
        if (recievedChar != 32){
          invalue = recievedChar - 65;
          if (invalue < 0){invalue+=43;}
          Serial.println("Encoding...");
          encode(invalue);
          wait();
        }
       else{
          Serial.println("END WORD DELAY");
          delay(spacewaittime);
       }
      }
    }
    else{
        lightsensor = digitalRead(lightPin);
        if (lightsensor == 1){
          Serial.println("LIGHT");
          lighttime += 1;
          darktime = 0;
          Serial.println(lighttime);
        }
        else{
          Serial.println("DARK");
          decode(lighttime);
          lighttime = 0;
          darktime += 1;
          if (darktime > 100){
            Serial.println(decodelist);
          }
          if (darktime > 2501){
            decode(darktime);
          }
        }
    }
  }
}

