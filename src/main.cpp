#include <Wire.h>
#include <SFE_BMP180.h>
SFE_BMP180 pressure;

void setup(){
    Serial.begin(115200);
    if (pressure.begin()){
        Serial.println("its working");
    }
    else{
        Serial.println("Not working\n");
        while (1);
    }
}

void loop(){
    char status;
    double T, P;
    status = pressure.startTemperature();

    if (status != 0){
        delay(status);
        status = pressure.getTemperature(T);
        if (status != 0){
            Serial.print("Temperature: ");
            Serial.print(T, 2);
            Serial.println(" °C");
            status = pressure.startPressure(3);
            if (status != 0){
                delay(status);
                status = pressure.getPressure(P, T);
                if (status != 0){
                    Serial.print("Absolute Pressure: ");
                    Serial.print(P, 2);
                    Serial.print(" mb / ");
                    Serial.print(P * 0.02953, 2);
                    Serial.println(" inHg");
                }
            }
        }
    }
    delay(2000);
}
