#include <Wire.h>
#define ledR 6
#define ledG 7
#define ledB 8

int red[] = { 255, 0, 0 };

int yellow[] = { 255, 128, 0 };

int green[] = { 0, 255, 0 };

void setup() {

  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);

  Wire.begin(1);
  Wire.onReceive(setColor);
  Serial.begin(9600);
}

void loop() {

  // put your main code here, to run repeatedly:
}



void setColor() {



  if (Wire.available()) {


   int temp = Wire.read();


   if (temp < 35) {
       turnRgbOn(green[0], green[1], green[2]);
    }
  else if (temp > 35 && temp < 50) {

    turnRgbOn(yellow[0], yellow[1], yellow[2]);
  } else if (temp > 50) {

    turnRgbOn(red[0], red[1], red[2]);
  }


  Serial.println(temp);

}


}



void turnRgbOn(int r, int g, int b) {
  analogWrite(ledR, r);
  analogWrite(ledG, g);
  analogWrite(ledB, b);
}

