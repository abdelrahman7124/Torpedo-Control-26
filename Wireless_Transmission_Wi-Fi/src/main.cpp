#include <Arduino.h>
#include "wireless.h"

wireless connection;

void setup() 
{
  Serial.begin(115200);
  connection.connect_init();
}

void loop()
{
  connection.send_float(20.0);
}

