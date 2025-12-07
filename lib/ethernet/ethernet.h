#ifndef ETHERNET_H
#define ETHERNET_H
#include <SPI.h>
#include <EthernetENC.h>
#include <EthernetUdp.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#define UDP_PACKET_MAX_SIZE 24
int ethernet_setup(IPAddress myIP, IPAddress sendingToIP, unsigned int localport, unsigned int cs_pin);
void MaintainEthernet();
void sendByUdp(std::string data);
void sendDataArrayFloat(float* dataArray, int arraySize);
std::string ReadDataAndConfirm();
void changeTargetIP(IPAddress newIP);
void changeMyIP(IPAddress newIP);
void changeMyPort(unsigned int localport1);

#endif