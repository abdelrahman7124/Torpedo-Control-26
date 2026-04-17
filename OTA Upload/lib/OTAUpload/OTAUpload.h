#pragma once
#include <WiFi.h>
#include <ArduinoOTA.h>

class OTAManager {
public:
    OTAManager(const char* ap_ssid, const char* ap_pass, const char* ota_pass = nullptr);
    void begin();
    void handle();
    bool isConnected();

private:
    const char* _ap_ssid;
    const char* _ap_pass;
    const char* _ota_pass;
};