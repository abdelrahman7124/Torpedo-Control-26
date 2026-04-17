#include "OTAUpload.h"
// your other includes...

OTAManager ota("ESP32-OTA", "ap_password", "ota_password");
// YourSystem mySystem;

int led = 2;
bool ledState = false;
unsigned long lastBlink = 0;

void setup() {
    Serial.begin(115200);
    ota.begin();
    pinMode(led, OUTPUT);
    
    // mySystem.begin();
}

void loop() {
    ota.handle();    // always first
    // mySystem.update();
    // Example: Blink an LED to show the system is alive
    if (millis() - lastBlink >= 2000) {
        ledState = !ledState;
        digitalWrite(led, ledState);
        lastBlink = millis();
    }
    
}