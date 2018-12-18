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
String charcterlist[36] = {"12", "2111", "2121", "211", "1", "1121","221", "1111", "11", "1222", "212", "1211","22","21","222","1221","2212","121","111","2","112","1112","122","2112","2122","2211","22222","12222","11222","11122","11112","11111","21111","22111","22211","22221"}; //abcdef...0123456789 (1=DOT and 2=DASH)
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
      if (tmp2 == "1"){
        Serial.println("SHORT");
        quickflash();
      }
      if (tmp2 == "2"){
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

