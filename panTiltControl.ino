/*
 Arduino Pins - Where it goes
2 - LCD D7
3 - LCD D6
4 - LCD D5
5 - LCD D4
6 - panServo
7 - tiltServo
8 - Rotary Encoder - SW
9 - Rotary Encoder - DT
10 - Rotary Enoder - CLK
11 - LCD E 
12 - LCD RS

Arduino Mini Pins
2 - panServo 
3 - tiltServo
4 - 
5 - LCD E
6 - LCD RS
7 - Rotary Clock
8 - Rotary DT
9 - Rotary SW /Push Button
10 - D4
11 - D5
12 - D6
13 - D7
 */ 

/* Libraries */
#include <LiquidCrystal.h>
#include <Servo.h>

/*Variables */
/* LCD Display */
LiquidCrystal lcd(6, 5, 10,11,12,13);
// RS, E, D4, D5, D6, D7

/* Rotary Encoder */
int clockPin = 7;
int dtPin = 8;
int pushButton = 9;

/* Servo Variables */
int panMinVal = -1;
int panMaxVal = -1;
int tiltMinVal = -1;
int tiltMaxVal = -1;
int pictureVal = 0;

Servo panServo;
int panServoPin = 2;
Servo tiltServo;
int tiltServoPin = 3;

int increment = 0;
boolean manyPicturesPerDegree;

/*Photo Settings*/
int typeOfShot = 0; //0 = stationary, 1 = vertical, 2 = horizontal, 3 = angle


int pickValue(int startValue, int minValue, long int maxValue, int multi, int lcdColumn, int lcdRow, int customDisplay = 0){
  int clockLastVal;
  int clockCurrentVal;
  boolean clockDirection;
  int encoderPosCount = 0;
  //customDisplay:
  //0 - just display a value
  //1 - display for selecting pan-tilt mode
  //2 - display for finding total quantity of photos

  while(digitalRead(pushButton) != LOW){
    lcd.setCursor(lcdColumn,lcdRow);
    if(customDisplay == 0){lcd.print(encoderPosCount);}
    if(customDisplay == 1){typeOfShotSelect(encoderPosCount);}
    if(customDisplay == 2){photoQuantityDisplay(encoderPosCount);}
    lcd.print("             ");
    clockCurrentVal = digitalRead(clockPin);
    if (clockCurrentVal != clockLastVal){
      if(digitalRead(dtPin) != clockCurrentVal){
        if(encoderPosCount < maxValue){encoderPosCount+= multi;}
      }
      else{
        if(encoderPosCount > minValue){encoderPosCount-= multi;}
      }
    }
    clockLastVal = clockCurrentVal;
  }
  if(customDisplay == 0){return encoderPosCount;} 
}

void typeOfShotSelect(int encoderPosCount){
  lcd.setCursor(0,1);
  if (encoderPosCount < 5){
    lcd.print("Stationary       ");
    typeOfShot = 0;
    }
  else if (encoderPosCount >= 5 && encoderPosCount<10){
    lcd.print("Vertical       ");
    typeOfShot = 1;
    }
  else if (encoderPosCount >= 10 && encoderPosCount <15){
    lcd.print("Horizontal      ");
    typeOfShot = 2;
    }
  else if (encoderPosCount >= 15 && encoderPosCount <=20){
    lcd.print("Angle        ");
    typeOfShot = 3;
    }
}

void photoQuantityDisplay(int inc){
  increment = inc;
  lcd.print(increment);
  int panPathAngle = abs(panMaxVal - panMinVal);
  int tiltPathAngle = abs(tiltMaxVal - tiltMinVal);
  int maxAngle = max(panPathAngle,tiltPathAngle);
  maxAngle = max(maxAngle,1);
  lcd.print(" ");
  lcd.print("(" + String(maxAngle*increment) + ")");
}


void setBounds(boolean panMinPrompt, boolean panMaxPrompt, boolean tiltMinPrompt, boolean tiltMaxPrompt){
  //pickValue(int startValue, int minValue, int maxValue, int multi, int lcdColumn, int lcdRow, int customMode)
  if(panMinPrompt == true){
    lcd.clear();
    lcd.print("Pan Start Value:");
    panMinVal = pickValue(90,0,180,5,0,1);  
    Serial.print("Pan min is: ");
    Serial.println(panMinVal);
    panServo.write(panMinVal);
    delay(250);
  }

  if(panMaxPrompt == true){
    lcd.clear();
    lcd.print("Pan End Value:");
    panMaxVal = pickValue(90,0,180,5,0,1); 
    Serial.print("Pan max is: ");
    Serial.println(panMaxVal);
    panServo.write(panMaxVal);
    delay(250);
  }

  if(tiltMinPrompt == true){
    lcd.clear();
    lcd.print("Tilt Start Value:");
    tiltMinVal = pickValue(90,0,180,5,0,1);   
    Serial.print("Tilt min is: ");
    Serial.println(tiltMinVal);
    tiltServo.write(tiltMinVal);
    delay(250);
  }

  if (tiltMaxPrompt == true){
    lcd.clear();
    lcd.print("Tilt End Value:");
    tiltMaxVal = pickValue(90,0,180,5,0,1); 
    Serial.print("Tilt max is: ");
    Serial.println(tiltMaxVal);
    tiltServo.write(tiltMaxVal);
    delay(250);
  }
  if (panMinVal == -1){panMinVal = panMaxVal;}
  if (panMaxVal == -1){panMaxVal = panMinVal;}
  if (tiltMinVal == -1){tiltMinVal = tiltMaxVal;}
  if (tiltMaxVal == -1){tiltMaxVal = tiltMinVal;}
  Serial.println("panMinVal: " + String(panMinVal) + " panMaxVal: " + String(panMaxVal) +  " tiltMinVal: " + String(tiltMinVal) +  " tiltMaxVal: " + String(tiltMaxVal));
}

void selectMode(){
  //setBounds(panMin, panMax, tiltMin, tiltMax)
  if (typeOfShot == 0){ //Stationary
    setBounds(true, false, true, false);
    panMaxVal = panMinVal;
    tiltMaxVal = tiltMinVal;
  }
  else if (typeOfShot == 1){ //Vertical
    setBounds(true, false, true, true);
    panMaxVal = panMinVal;
  }
  else if (typeOfShot == 2){ //Horizontal
    setBounds(true, true, true, false);
    tiltMaxVal = tiltMinVal;
  }
  else if (typeOfShot == 3){//Angle
    setBounds(true, true, false, false);
    //Code to pick This
  }
  
}


void setup() {
  Serial.begin(9600);
  
  /*Liquid Crystal*/
  lcd.begin(16,2);

  /* Rotary Encoder */
  pinMode(clockPin,INPUT);
  pinMode(dtPin,INPUT);

  /* Servo */
  tiltServo.attach(tiltServoPin);
  panServo.attach(panServoPin);
}




void loop() {
  /*lcd.clear();
  lcd.print("Pan & Tilt by");
  lcd.setCursor(0,1);
  lcd.print("Travis Bumgarner");
  delay(2000);*/

  //pickValue(int startValue, int minValue, int maxValue, int multi, int lcdColumn, int lcdRow, int customMode)
   
  lcd.clear();
  lcd.print("Shooting Mode:");
  pickValue(0,0,15,1,0,1,1);//Function for picking type of shot
  delay(250);

    
  if(typeOfShot == 0){setBounds(true,false,true,false);} //Stationary 
  else if(typeOfShot == 1){setBounds(true,false,true,true);} //Vertical
  else if(typeOfShot == 2){setBounds(true,true,true,false);} //Horizontal
  else if(typeOfShot == 3){setBounds(true,true,true,true);} //Angle  
  delay(250);

  lcd.clear();
  lcd.print("Increment: ");
  if(typeOfShot == 0){ 
    pickValue(0,0,99999,5,0,1,2);
  }
  else{
    pickValue(0,0,1000,1,0,1,2);
  }
  
  
}
