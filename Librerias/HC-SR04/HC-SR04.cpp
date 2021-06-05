#include "Arduino.h"
#include "HC-SR04.h"

HCSR04::HCSR04(int echo, int trigger){
  _echo = echo;
  _trigger = trigger;
  _tiempo = 0;
  _distanciaPromedio = 0;
  pinMode(_trigger, OUTPUT);
  pinMode(_echo, INPUT);
}

long HCSR04::medirDistancia(){
  digitalWrite(_trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigger, LOW);
  _tiempo = pulseIn(_echo, HIGH);
  return _tiempo / 58.2; //Distancia en cm
}

long HCSR04::medirDistanciaPromedio(int numeroMedidas){
  _distanciaPromedio = 0;
  for (int i = 0; i < numeroMedidas; i++){
    _distanciaPromedio += medirDistancia();
    delay(50);
  }
  return _distanciaPromedio / numeroMedidas;
}