#ifndef ETHERNET_OTA_H
#define ETHERNET_OTA_H
#include <SPI.h>
#include <EthernetENC.h>
#include <Update.h>
void sendIndexPage(EthernetClient& client);
void handleOTA(EthernetClient& client, String header);
void handleEthernetOTA(EthernetServer& server);
void setValues(float kp, float ki, float kd);
void sendValues(float kp, float ki, float kd);
#endif