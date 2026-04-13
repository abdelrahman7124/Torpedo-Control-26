/*
Author: Marwan Aly
Project: Wireless Connection
Team: Torpedo ROV
Discrption: Handles wireless communication for ESP32 and send data by using the ESP32 as a WIFI access point
*/

#ifndef WIRELESS_H
#define WIRELESS_H

#include "config.h"

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

};






#endif