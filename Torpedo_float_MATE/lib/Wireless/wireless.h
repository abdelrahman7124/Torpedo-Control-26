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
        
        WiFiUDP udp;
        
        
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
        void send_int(int msg);
        void send_string(char* msg);
        void send_float(float msg);
        wl_status_t check_connection() const;

};






#endif