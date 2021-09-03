#include <Arduino.h>
#include <TimerOne.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Servo.h>

#define button1 4
#define button2 2
#define button3 5
#define button4 3
#define DFBusyPin 6 //digitalRead(DFBusyPin)=0 - busy;
#define feedback A0
#define eyePin 11
#define mouthPin 12

static unsigned long timer = millis();
static unsigned long eyeTimer = millis();
static unsigned long commandTimer = millis();

bool DFplaying = false;
bool DFNotBusy = false;
int status = 0;
int phrase = 0;
int eyePos = 0;
int mouthPos = 0;

byte command = 0b00000000;

bool butt1 = 0;
bool butt2 = 0;
bool butt3 = 0;
bool butt4 = 0;

Servo mouthServo;
Servo eyeServo;
SoftwareSerial DFplayerSoftwareSerial(7, 8); // RX, TX
DFRobotDFPlayerMini voiceDFPlayer;

void blink(void){
  if (millis() - timer > 1000) {
    timer = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    DFNotBusy = digitalRead(DFBusyPin);
  }
}

void commandErase(void){
  if (millis() - commandTimer > 5000) {
    commandTimer = millis();
    phrase = command;
    command = 0;
  }
}


void eyeBlinkAnimation(void){
  if (millis() - eyeTimer > 10000) {
    eyeTimer = millis();
    eyePos = 0;
    eyeServo.write(eyePos);
    delay(500);
  }
  else eyePos = 120;
  eyeServo.write(eyePos);
}

void mouthAnimation(void){
  mouthPos = map(analogRead(feedback), 0, 512, 0, 180);
  mouthServo.write(mouthPos);
}


void voice(void){
  if (phrase!=0){
    voiceDFPlayer.play(phrase);
    phrase=false;
  }
}

void pultRead(void){
  if (digitalRead(button1) == true){command |= bit(0);}
  if (digitalRead(button2) == true){command |= bit(1);}
  if (digitalRead(button3) == true){command |= bit(2);}
  if (digitalRead(button4) == true){command |= bit(3);}
}



void setup() {
  Serial.begin(9600);
  DFplayerSoftwareSerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  mouthServo.attach(mouthPin);
  mouthServo.write(0);
  eyeServo.attach(eyePin);
  eyeServo.write(0);

  if (!voiceDFPlayer.begin(DFplayerSoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }

  if (digitalRead(DFBusyPin)==0){voiceDFPlayer.stop();}
  voiceDFPlayer.volume(30);  //Set volume value. From 0 to 30
  delay(250);
}

// the loop function runs over and over again forever
void loop() {
  if(digitalRead(DFBusyPin)==false){
    eyeBlinkAnimation();
    mouthAnimation();
  }
  else {
    mouthServo.write(0);
    eyeServo.write(0);
  }

  blink();
  voice();
  pultRead();
  commandErase();
  // // mouthAnimation();
  // // eyeBlinkAnimation();
  // Serial.print("\t command = ");
  // Serial.print(command, BIN);
  // Serial.print("\t phrase = ");
  // Serial.print(phrase);
  // Serial.print("\t status = ");
  // Serial.print(status);
  // Serial.println();
}
