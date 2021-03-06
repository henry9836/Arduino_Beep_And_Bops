//Built by Henry Oliver

const int ledPin = 9;
const int lightPin = 3;
const String charcterlist[36] = {"12", "2111", "2121", "211", "1", "1121","221", "1111", "11", "1222", "212", "1211","22","21","222","1221","2212","121","111","2","112","1112","122","2112","2122","2211","22222","12222","11222","11122","11112","11111","21111","22111","22211","22221"}; //abcdef...0123456789 (1=DOT and 2=DASH)

int lighttime = 0;
int darktime = 0;
int brightness = 1023;
int lightsensor = 0;
int blinktime = 416;
int longtime = 1248;
int waittime = 1248;
int spacewaittime = 2912;
String tmp;
String tmp2;
String decodelist = "";
bool encodeMode = true;
bool chosenMode = false;
int invalue = 0;
char recievedChar;
bool wasspilt = true;

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

  if ((timeon > 20) && (timeon < 50)){
    decodelist += ".";
    wasspilt = false;
  }
  else if ((timeon > 80) && (timeon < 130)){
    decodelist += "-";a
    wasspilt = false;
  }
  else if ((timeon > 200) && (timeon < 3000)){
    decodelist += "/";
    wasspilt = true;
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
  digitalWrite(ledPin, 1);
  delay(blinktime);
  digitalWrite(ledPin, 0);
  wait();
}

void longflash() {
  digitalWrite(ledPin, 1);
  delay(longtime);
  digitalWrite(ledPin, 0);
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
          if (darktime > 150){
            Serial.println(darktime);
            if (wasspilt == false){
              decode(darktime);
            }
            if (darktime > 1200){
              Serial.println(decodelist); //PRINT OUT DECODED STRING
            }
          }
        }
    }
  }
}
