#ifndef HCSR04_h
#define HCSR04_h
 
#include "Arduino.h"
 
class HCSR04
{
  public:
    HCSR04(int echo, int trigger);
    long medirDistancia();
    long medirDistanciaPromedio(int numeroMedidas);
  private:
    int _echo;
    int _trigger;
    long _tiempo;
    long _distanciaPromedio;
};
 
#endif