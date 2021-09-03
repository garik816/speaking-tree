#include <Arduino.h>
#include <TimerOne.h>
#include <NewPing.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Servo.h>

#define ECHO 2
#define TRIG 3
#define sensVCC 4
#define DFBusyPin 5 //digitalRead(DFBusyPin)=0 - busy;
#define feedback A0
#define mouthPin 12

static unsigned long timer = millis();
static unsigned long timerDF = millis();

bool DFplaying = false;
bool DFNotBusy = false;
int status = 0;
int phrase = 0;
int mouthPos = 0;

Servo mouthServo;
NewPing sonar(TRIG, ECHO, 400);
SoftwareSerial DFplayerSoftwareSerial(7, 8); // RX, TX
DFRobotDFPlayerMini voiceDFPlayer;

float dist_3[3] = {0.0, 0.0, 0.0};   // массив для хранения трёх последних измерений
float middle, dist, dist_filtered;
float k;
byte i, delta;
unsigned long dispIsrTimer, sensTimer;

float middle_of_3(float a, float b, float c) {
  if ((a <= b) && (a <= c)) {
    middle = (b <= c) ? b : c;
  }
  else {
    if ((b <= a) && (b <= c)) {
      middle = (a <= c) ? a : c;
    }
    else {
      middle = (a <= b) ? a : b;
    }
  }
  return middle;
}

void blink(void){
  if (millis() - timer > 1000) {
    timer = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    DFNotBusy = digitalRead(DFBusyPin);
  }
}

void distanceMeasure(void){
  if (millis() - sensTimer > 100) {                          // измерение и вывод каждые 100 мс
    if (i > 1) i = 0;
    else i++;

    dist_3[i] = (float)sonar.ping() / 57.5;                 // получить расстояние в текущую ячейку массива
    dist = middle_of_3(dist_3[0], dist_3[1], dist_3[2]);    // фильтровать медианным фильтром из 3ёх последних измерений

    delta = abs(dist_filtered - dist);                      // расчёт изменения с предыдущим
    if (delta > 1) k = 0.7;                                 // если большое - резкий коэффициент
    else k = 0.1;                                           // если маленькое - плавный коэффициент

    dist_filtered = dist * k + dist_filtered * (1 - k);     // фильтр "бегущее среднее"



    //Serial.println(dist_filtered);                          // вывести //debug
    sensTimer = millis();                                   // сбросить таймер
  }
}

void player(void){
  if (status == 1 && phrase==1 && DFNotBusy == true){
    voiceDFPlayer.play(phrase);
  }
  if (status == 1 && phrase==2 && DFNotBusy == true){
    voiceDFPlayer.play(phrase);
  }
  if (status == 3 && phrase==3 && DFNotBusy == true){
    voiceDFPlayer.play(phrase);
  }
}




void setup() {
  Serial.begin(9600);
  DFplayerSoftwareSerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(DFBusyPin, INPUT);
  pinMode(sensVCC, OUTPUT);
  digitalWrite(sensVCC, 1);

  mouthServo.attach(mouthPin);
  mouthServo.write(0);

  if (!voiceDFPlayer.begin(DFplayerSoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }

  voiceDFPlayer.volume(30);  //Set volume value. From 0 to 30
  // voiceDFPlayer.play(1); //debug
  //myDFPlayer.next(); //debug
  delay(250);
}

// the loop function runs over and over again forever
void loop() {

  blink();
  distanceMeasure();
  player();
  mouthPos = map(analogRead(feedback), 0, 512, 0, 180);
  mouthServo.write(mouthPos);

  if(dist_filtered > 10 && dist_filtered < 50){
    status = 1;
    phrase = 1;
  }
  else status = 0;
  if(dist_filtered > 50 && dist_filtered < 100){
    status = 3;
    phrase = 3;
  }




  // if (millis() - timerDF > 30000) {
  //   timerDF = millis();
  //   phrase++;
  //   DFplaying = false;
  //   if (phrase > 3){
  //     phrase=1;
  //   }
  // }
 //Serial.print(digitalRead(DFBusyPin));


 Serial.print("\t dist_filtered = ");
 Serial.print(dist_filtered);
 Serial.print("\t status = ");
 Serial.print(status);
 Serial.print("\t phrase = ");
 Serial.print(phrase);
 Serial.print("\t DFNotBusy = ");
 Serial.print(DFNotBusy);
 // Serial.print("\t feedback = ");
 // Serial.print(analogRead(feedback));
 // Serial.print("\t angle = ");
 // Serial.print(mouthPos);
 Serial.println();

}
