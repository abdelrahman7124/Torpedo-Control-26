#ifndef WIRELESS_H
#define WIRELESS_H

#include "config.h"

class wireless
{
    private:
        char* ssid;
        char* password;
        char* client_ip;
        
        int port;
        
        WiFiUDP udp;
        
        
    public:
        
        wireless();
        void set_ssid(char* word);
        void set_password(char* word);
        void set_clientIP(char* ip);
        void set_port(int num);

        char* get_ssid();
        char* get_password();
        char* get_clientIP();
        IPAddress get_serverIP();
        int get_port();
        
        void connect_init();
        void send_int(int msg);
        void send_string(char* msg);
        void send_float(float msg);
        wl_status_t check_connection();

};






#endif