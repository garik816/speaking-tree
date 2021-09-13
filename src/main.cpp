#include <Arduino.h>
#include <Servo.h>

#define feedback A0
#define eyePin 11
#define mouthPin 12

static unsigned long timer = millis();
static unsigned long eyeTimer = millis();
static unsigned long mouthTimer = millis();
static unsigned long stopTimer = millis();

int eyePos = 0;
int mouthPos = 0;
bool flag = false;

Servo mouthServo;
Servo eyeServo;

void blink(void){
  if (millis() - timer > 1000) {
    timer = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}

void stopper(void){
  if (flag == false) {
    if (millis() - stopTimer > 5000) {
      stopTimer = millis();
      eyePos = 0;
      eyeServo.write(eyePos);
      mouthPos = 0;
      mouthServo.write(mouthPos);
      delay(1000);
    }
  }
}


void eyeBlinkAnimation(void){
  if (flag == true){
    if (millis() - eyeTimer > 10000) {
      eyeTimer = millis();
      eyePos = 0;
      eyeServo.write(eyePos);
      delay(500);
    }
    else {
      eyePos = 120;
      eyeServo.write(eyePos);
    }
  }
}

void mouthAnimation(void){
  if (millis() - mouthTimer > 200) {
    if (analogRead(feedback)>600 || analogRead(feedback)<400){
      flag = true;
      mouthTimer = millis();
      mouthPos = map(analogRead(feedback), 350, 700, 0, 180);
      mouthServo.write(mouthPos);
    }
    else {
      flag = false;
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  mouthServo.attach(mouthPin);
  mouthServo.write(0);
  eyeServo.attach(eyePin);
  eyeServo.write(0);
  delay(1000);
}

void loop() {
  blink();
  mouthAnimation();
  eyeBlinkAnimation();
  stopper();
}
