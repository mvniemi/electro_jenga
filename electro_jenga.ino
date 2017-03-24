/*
Electo-Jenga
Michael Niemi 2017
MIT License
*/
#include <ADCTouch.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
//#include "pitches.h"
#define I2C_ADDR 0x27 // Define I2C Address where the SainSmart LCD is
#define BACKLIGHT_PIN 3

//Defines for our Inputs/Outputs:

#define BEGIN 0
#define COUNTDOWN 1
#define ACTIVE 2

#define TEST 11
#define TIMERUP 6
#define TIMERDOWN 5
#define SENSUP 8
#define SENSDOWN 7
#define ZERO 9
#define TURN 12
#define SPEAKER_PIN 4
#define MODE_PIN 10
#define RELAY1 0
#define RELAY2 1
#define NORMAL 0
#define DEATH 1
#define CAP_READ A0


//LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
LiquidCrystal_I2C lcd(I2C_ADDR,2,1,0,4,5,6,7);

byte state = BEGIN;
byte player = 0;
int ref;
int timer = 3;
int threshold = 15;

byte mode = NORMAL;
unsigned long inputStateMillis = 0;
const long inputStateInterval = 150;

void setup() {

    pinMode(RELAY1,OUTPUT);
    pinMode(RELAY2,OUTPUT);
    digitalWrite(RELAY1,HIGH);
    digitalWrite(RELAY2,HIGH);

    for (int i = 5; i < 13; i++){
        pinMode(i , INPUT_PULLUP);
    }
    
    lcd.begin (20, 4);
// Switch on the backlight
    lcd.setBacklightPin(3,POSITIVE);
    lcd.setBacklight(HIGH);
    lcd.home();
    lcd.clear();

    Serial.begin(9600);

    ref = ADCTouch.read(A0, 1000) - 2;    //create reference values to
    lcd.setCursor(0,0);
    lcd.print("Touch to begin");



}

void loop() {

    

    int value = ADCTouch.read(A0);   //no second parameter
    value -= ref;       //remove offset
      Serial.print(value);    //send (boolean) pressed or not pressed
      Serial.println("\t"); 

    unsigned long currentMillis = millis();

    if (currentMillis - inputStateMillis >= inputStateInterval) {
        inputStateMillis = currentMillis;
        //Check our timer, sensitivity, and player turn button
        if (digitalRead( TIMERUP ) == 0){timer++;;}
        if (digitalRead( TIMERDOWN ) == 0){timer--;}
        if (digitalRead( SENSUP) == 0){ threshold += 2;}
        if (digitalRead( SENSDOWN) == 0){ threshold -= 2;}
        if (digitalRead( ZERO) == 0){ ref = ADCTouch.read(CAP_READ); }

        if (digitalRead( TURN ) == 1 && player == 0) {
          player = 1;
          state = BEGIN;
          allOff();
        }

        if (digitalRead( TURN ) == 0 && player == 1) {
          player = 0;
          state = BEGIN;
          allOff();
        }

        if (digitalRead( MODE_PIN ) == 0){
          if (mode == NORMAL){ mode = DEATH; }
          else (mode = NORMAL);
        }
  
      //Update LCD w/State info
//      clearLine(3,10,20);
      lcd.setCursor(0,3);
      lcd.print("T:");
      lcd.print(timer);
      lcd.print(" S:");
      lcd.print(threshold);
      lcd.print(" C=");
      lcd.print(value);
      lcd.print("   ");
      lcd.setCursor(0,2);
      if (mode == NORMAL){ lcd.print("Normal Mode    ");}
      else {lcd.print("Sudden Death    ");}

    }



//Print out state info
    if ( state == BEGIN ) {
      lcd.setCursor(0,0);
      lcd.print("Touch to begin");
      lcd.setCursor(0,1);
      lcd.print("Player ");
      lcd.print(player+1);    

      if ( value > threshold ){
        state = COUNTDOWN;
      }
        
    }

    if ( state == COUNTDOWN ) {
      clearLine(0,0,20);
      for (int i = 0; i < timer; i++){
        lcd.setCursor(0,0);
        lcd.print(timer-i);
        lcd.print("   ");
        tone(SPEAKER_PIN , 440, 100);
        delay(1000);
      }
      tone(SPEAKER_PIN , 880 , 400);
      lcd.setCursor(0,0);
      lcd.print("Shock Active!");
      state = ACTIVE;      
    }

    if ( state == ACTIVE ) {
      if (mode == NORMAL){
        if (value > threshold){
          if (player == 0) {
            digitalWrite(RELAY1 , LOW);
          }
          else {
            digitalWrite(RELAY2 , LOW);
          }
        }
        else {
          digitalWrite(RELAY1 , HIGH);
          digitalWrite(RELAY2 , HIGH);
        }
        delay(100);
      }
     else {
          if (player == 0) {
            digitalWrite(RELAY1 , LOW);
          }
          else {
            digitalWrite(RELAY2 , LOW);
         }      
     }
   }
}


void clearLine(int line, int beg, int stp){
  lcd.setCursor(beg,line);
  for (int i=beg; i < stp ; i++){
    lcd.print(" ");
  }
}

void allOff(){
          digitalWrite(RELAY1 , HIGH);
          digitalWrite(RELAY2 , HIGH);
}

