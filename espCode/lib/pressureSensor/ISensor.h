#ifndef __ISENSOR_H
#define __ISENSOR_H
#include "Config.h"
class ISensor{
public:
  virtual void init()=0;
  virtual void update()=0; 
  virtual void display()=0;
};
#endif