#ifndef ETHERNET_H
#define ETHERNET_H

#include <SPI.h>
#include <EthernetENC.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#define UDP_PACKET_MAX_SIZE 24

extern uint8_t mac[6];   // Fix: expose mac to all translation units

int ethernet_setup(IPAddress myIP, IPAddress sendingToIP, unsigned int localport, unsigned int cs_pin);
void MaintainEthernet();
void sendByUdp(std::string data);
void sendDataArrayFloat(float* dataArray, int arraySize);
std::string ReadDataAndConfirm();
void changeTargetIP(IPAddress newIP);
void changeMyIP(IPAddress newIP);
void changeMyPort(unsigned int localport1);
char* checkIncomingUDP();
void recoverNetwork();
bool checkNetworkHealth();
void resetUDP();

#endif