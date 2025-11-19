#include <SPI.h>
#include <EthernetENC.h>
#include <EthernetUdp.h>
#include <Arduino.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
unsigned int localPort = 8888;

EthernetUDP Udp;
char packetBuffer[24];

void setup() {
  Serial.begin(9600);
  Ethernet.init(5);
  pinMode(22,OUTPUT);
  Serial.println("beginning module");
  Ethernet.begin(mac, ip);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet module not found");
    while (true) {
      delay(1);
    }
  }

  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  Udp.begin(localPort);
}

void loop() {
  Ethernet.maintain();

  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Udp.read(packetBuffer, 24);
    Serial.println("Data sent:");
    if(packetBuffer[0]=='1'){
      digitalWrite(22,HIGH);
      Serial.println("led high");
    }else{
      digitalWrite(22,LOW);
      Serial.println("led low");
    }
    Serial.println(packetBuffer);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("received the message");
    Udp.endPacket();
  }
}