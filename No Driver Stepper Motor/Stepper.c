const int StepPin1 = 2; //BLUE
const int StepPin2 = 3; //PINK
const int StepPin3 = 4; //YELLOW
const int StepPin4 = 5; //ORANGE
const int turnsteps = 1000; //AMOUNT OF STEPS TO TAKE
const int pulsedelay = 5; //DELAY BETWEEN PULSES
const int senddelay = 30; //DELAY BETWEEN COMMANDS

int incomingBIN;

void setup() {
  Serial.begin(9600);
  pinMode(StepPin1, OUTPUT);
  pinMode(StepPin2, OUTPUT); 
  pinMode(StepPin3, OUTPUT); 
  pinMode(StepPin4, OUTPUT);  
  Serial.println("Please Enter Your Binary Message: ");   
}

int SpinMeRightRound(int dir){

    if (dir == 1){
    Serial.println("FORWARD! "); //THIS WILL MAKE THE MOTOR STEP FORWARDS YOU NEED TO GO FROM STEP 1-4 TO MAKE A FULL STEP
      // STEP 1
  
    digitalWrite(StepPin1, HIGH);
    digitalWrite(StepPin2, LOW);
    digitalWrite(StepPin3, LOW);
    digitalWrite(StepPin4, HIGH);
    delay(pulsedelay);
    
    // STEP 2
  
    digitalWrite(StepPin1, HIGH);
    digitalWrite(StepPin2, HIGH);
    digitalWrite(StepPin3, LOW);
    digitalWrite(StepPin4, LOW);
    delay(pulsedelay);
  
    // STEP 3
  
    digitalWrite(StepPin1, LOW);
    digitalWrite(StepPin2, HIGH);
    digitalWrite(StepPin3, HIGH);
    digitalWrite(StepPin4, LOW);
    delay(pulsedelay);
  
    // STEP 4
  
    digitalWrite(StepPin1, LOW);
    digitalWrite(StepPin2, LOW);
    digitalWrite(StepPin3, HIGH);
    digitalWrite(StepPin4, HIGH);
    delay(pulsedelay);
    }
    
    else{
     
    Serial.println("BACKWARDS! ");  //THIS WILL MAKE THE MOTOR STEP BACKWARDS YOU NEED TO GO FROM STEP 1-4 TO MAKE A FULL STEP

    // STEP 1
  
    digitalWrite(StepPin1, LOW);
    digitalWrite(StepPin2, LOW);
    digitalWrite(StepPin3, HIGH);
    digitalWrite(StepPin4, HIGH);
    delay(pulsedelay);
    

    // STEP 2
  
    digitalWrite(StepPin1, HIGH);
    digitalWrite(StepPin2, HIGH);
    digitalWrite(StepPin3, LOW);
    digitalWrite(StepPin4, LOW);
    delay(pulsedelay);

     // STEP 3
  
    digitalWrite(StepPin1, LOW);
    digitalWrite(StepPin2, HIGH);
    digitalWrite(StepPin3, HIGH);
    digitalWrite(StepPin4, LOW);
    delay(pulsedelay);

    // STEP 4
  
    digitalWrite(StepPin1, HIGH);
    digitalWrite(StepPin2, HIGH);
    digitalWrite(StepPin3, LOW);
    digitalWrite(StepPin4, LOW);
    delay(pulsedelay);
    }

    return 0;
    
}

int WriteOne(){
  for(int i = 0; i < turnsteps; i = i + 1 ){
    SpinMeRightRound(1);
  }
  return 0;
}

int WriteZero(){
  for(int i = 0; i < turnsteps; i = i + 1 ){
    SpinMeRightRound(0);
  }
  return 0;
}

void loop() {
  if (Serial.available() > 0) {
                incomingBIN = Serial.read();
                Serial.println(incomingBIN);
                if (incomingBIN == 49){
                  Serial.println("U SENT A 1!");
                  WriteOne();
                }
                else if (incomingBIN == 48){
                  Serial.println("U SENT A 0!");
                  WriteZero();
                }
                else{ //CATCH BAD CHARS
                  Serial.println("[WARN] Detected Unknown Charcter");
                  Serial.println("!!! PLEASE ONLY ENTER A 1 OR 0 !!!");
                }
                delay(senddelay);
  }
}
