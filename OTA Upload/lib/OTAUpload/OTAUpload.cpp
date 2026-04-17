#include "OTAUpload.h"

OTAManager::OTAManager(const char* ap_ssid, const char* ap_pass, const char* ota_pass)
    : _ap_ssid(ap_ssid), _ap_pass(ap_pass), _ota_pass(ota_pass) {}

void OTAManager::begin() {
    WiFi.softAP(_ap_ssid, _ap_pass);
    Serial.print("[OTA] AP started. IP: ");
    Serial.println(WiFi.softAPIP());

    if (_ota_pass) ArduinoOTA.setPassword(_ota_pass);

    ArduinoOTA.onStart([]() {
        Serial.println("[OTA] Upload starting...");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("[OTA] Done. Rebooting.");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("[OTA] %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("[OTA] Error[%u]\n", error);
    });

    ArduinoOTA.begin();
}

void OTAManager::handle() {
    ArduinoOTA.handle();
}

bool OTAManager::isConnected() {
    return WiFi.softAPgetStationNum() > 0;
}