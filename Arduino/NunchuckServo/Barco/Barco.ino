// NUNCHUCK
//GND - GND
//VCC - 3V3
//SDA - Pin A4
//SCL - Pin A5


//SERVO
//Naranja - D9
//Marron - GND
//Rojo - 5V

#include "Wiichuck.h"
#include <Wire.h>
#include <Servo.h>

#define pinServo 9
int posicionMotor = 90;

Wiichuck wii;
Servo Motor;

void setup() {
  Serial.begin(9600);
  wii.init(); 
  wii.calibrate();
  Motor.attach(pinServo);
}
void loop() {
  if (wii.poll()) {
    Serial.print("joy:");
    Serial.print(wii.joyX());
    Serial.print(", ");
    Serial.print(wii.joyY());
    Serial.println("  \t");
    
    // Serial.print("accle:");
    // Serial.print(wii.accelX());
    // Serial.print(", ");
    // Serial.print(wii.accelY());
    // Serial.print(", ");
    // Serial.print(wii.accelZ());
    // Serial.print("  \t");
    
    // Serial.print("button:");
    // Serial.print(wii.buttonC());
    // Serial.print(", ");
    // Serial.print(wii.buttonZ());
    // Serial.println("");
    posicionMotor = map(wii.joyX(),0,255,0,180);
    Motor.write(posicionMotor);
  }
  
}