#include "main.h"
   
void setup() 
{
    Serial.begin(9600);
    Wire.begin();
    pressure.init();
    connection.connect_init();
}

void loop() 
{
    depth = pressure.readDepth();
    pressure_value = pressure.readPressure();
    time_stamp = pressure.getTime();
    delay(1000);

    msg = time_stamp + "Depth: " + String(depth) + " m, Pressure: " + String(pressure_value) + " Pa";

    connection.send_data(msg);
}
