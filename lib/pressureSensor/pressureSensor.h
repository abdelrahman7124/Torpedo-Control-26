#ifndef __PRESSURE_H
#define __PRESSURE_H
#include "ISensor.h"

class Pressure:public ISensor{
  int pressure;
  int temp;
  int oldPressure=1013;
  SPIClass * hspi;
  void resetSensor();
public:
  Pressure();
  void init()override;
  void update()override;
  void display()override;
  int getTemprature();
  int getPressure();
  void depthInitialization(); 
  float getDepth();
};




#endif