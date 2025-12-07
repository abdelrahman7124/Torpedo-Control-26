#include <SPI.h>
#include <EthernetENC.h>
#include <EthernetUdp.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFF, 0xED };
IPAddress ip(192, 168, 1, 30);
unsigned int localPort = 8888;
IPAddress targetIP(192, 168, 1, 2);
EthernetUDP Udp;
using namespace std;
char packetBuffer[24];
int ethernet_setup(IPAddress myIP,IPAddress sendingToIP,unsigned int localport,unsigned int cs_pin){
    ip=myIP;
    targetIP=sendingToIP;
    localPort=localport;
    Ethernet.init(cs_pin);
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        return 0;
    }
    Ethernet.begin(mac, ip);
    if (Ethernet.linkStatus() == LinkOFF) {
        return 1;
    }
    Udp.begin(localPort);
    return 2;
}
void sendByUdp(string string1){
    Ethernet.maintain();
    Udp.beginPacket(targetIP, localPort);
    Udp.write(string1.c_str());
    Udp.endPacket();
}
void changeTargetIP(IPAddress newIP){
    targetIP=newIP;
}
void changeMyIP(IPAddress newIP){
    ip=newIP;
    Ethernet.begin(mac, newIP);
    Udp.begin(localPort);
}
void changeMyPort(unsigned int localport1){
    localPort=localport1;
    Udp.begin(localport1);
}
void MaintainEthernet(){
    Ethernet.maintain();
}
string ReadDataAndConfirm(){
    int packetSize = Udp.parsePacket(); 
    if (packetSize) {
        memset(packetBuffer, 0, sizeof(packetBuffer));
        Udp.read(packetBuffer, 24);
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write("received the message");
        Udp.endPacket();
        return string(packetBuffer);
    } 
    return "";
}
void sendDataArrayFloat(float* dataArray,int arraySize){
    string finaData="";
    for(int i=0;i<arraySize;i++){
        finaData+=to_string(dataArray[i]);
        if(i<arraySize-1){
            finaData+=",";
        }
    }
    sendByUdp(finaData);
}