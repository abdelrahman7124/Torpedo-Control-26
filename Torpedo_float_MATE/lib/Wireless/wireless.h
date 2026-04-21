/*
Author: Marwan Aly
Project: Wireless Connection
Team: Torpedo ROV
Description: Handles wireless communication for ESP8266 and send data by using the ESP8266 as a WIFI access point
*/

#ifndef WIRELESS_H
#define WIRELESS_H

#include <ESP8266WiFi.h>
#include <Arduino.h>

#include "wireless_config.h"

class wireless
{
    private:
        char ssid[64];
        char password[32];
        IPAddress client_ip;
        
        int port;
        
        WiFiServer server;
        WiFiClient client;
        
        void update();
        
    public:
        
        wireless();
        void set_ssid(const char* word);
        void set_password(const char* word);
        void set_clientIP(IPAddress ip);
        void set_port(int num);

        const char* get_ssid() const;
        const char* get_password() const;
        const IPAddress get_clientIP() const;
        const IPAddress get_serverIP() const;
        int get_port() const;
        void connect_init();
        void send_data(const String msg);
        wl_status_t check_connection() const;
        String receive_data();
        void close();
};

#endif