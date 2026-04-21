#include "ethernet.h"


using namespace std;

uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 44);
unsigned int localPort = 8881;
IPAddress targetIP(192, 168, 1, 2);
EthernetUDP Udp;


char packetBuffer[24];
char packetBuffer2[256];

int ethernet_setup(IPAddress myIP, IPAddress sendingToIP, unsigned int localport, unsigned int cs_pin) {
    ip = myIP;
    targetIP = sendingToIP;
    localPort = localport;
    Ethernet.init(cs_pin);

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        return 0;
    }

    Ethernet.begin(mac, ip);
    Udp.begin(localPort);

    if (Ethernet.linkStatus() == LinkOFF) {
        return 1;
    }

    return 2;
}

void sendByUdp(string string1) {
    Ethernet.maintain();
    Udp.beginPacket(targetIP, 8888);
    Udp.write(string1.c_str());
    Udp.endPacket();
}

void changeTargetIP(IPAddress newIP) {
    targetIP = newIP;
}

void changeMyIP(IPAddress newIP) {
    ip = newIP;
    Ethernet.begin(mac, newIP);
    Udp.begin(localPort);
}

void changeMyPort(unsigned int localport1) {
    localPort = localport1;
    Udp.begin(localport1);
}

void MaintainEthernet() {
    Ethernet.maintain();
}

string ReadDataAndConfirm() {
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

void sendDataArrayFloat(float* dataArray, int arraySize) {
    string finalData = "";
    for (int i = 0; i < arraySize; i++) {
        finalData += to_string(dataArray[i]);
        if (i < arraySize - 1) {
            finalData += ",";
        }
    }
    sendByUdp(finalData);
}

char* checkIncomingUDP() {
    int packetSize = Udp.parsePacket();

    if (packetSize > 0) {
        int len = Udp.read(packetBuffer2, 255);
        if (len > 0) packetBuffer2[len] = 0;
        while (Udp.parsePacket() > 0) {
            Udp.read(packetBuffer2, 255);
        }
        return packetBuffer2;
    }
    return NULL;
}

bool checkNetworkHealth() {
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Error: Ethernet Chip not found/responding.");
        return false;
    }

    if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Error: Ethernet Cable disconnected.");
        return false;
    }

    return true;
}

void recoverNetwork() {
    Serial.println(">> RECOVERING ETHERNET...");
    Udp.stop();
    Ethernet.init(5);  
    Ethernet.begin(mac, ip);
    Udp.begin(localPort);
    Serial.println(">> RECOVERY COMPLETE.");
}

void resetUDP() {
    Udp.stop();
    Udp.begin(localPort);
}