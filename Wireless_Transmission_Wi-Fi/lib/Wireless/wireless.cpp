#include "wireless.h"

wireless::wireless()
{
    ssid = "ESP";
    password = "12345678";
    client_ip = "192.168.4.2";
    port = 1234;
}

void wireless::set_ssid(char* word)
{
    ssid = word;
}

void wireless::set_password(char* word)
{
    if (strlen(word) < 8)
    {
        Serial.println("Password must be at least 8 charecters long!");
    }
    
    else
    {
        password = word;
    }
}

void wireless::set_clientIP(char* msg)
{
    client_ip = msg;
}

void wireless::set_port(int num)
{
    if((num > 1024) && (num < 65535))
    {
        port = num;
    }
    
    else
    {
        Serial.println("Port is not available!");
    }
}

char* wireless::get_ssid()
{
    return ssid;
}

char* wireless::get_password()
{
    return password;
}

char* wireless::get_clientIP()
{
    return client_ip;
}

IPAddress wireless::get_serverIP()
{
    return WiFi.softAPIP();
}

int wireless::get_port()
{
    return port;
}

void wireless::connect_init()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssid, password);
    udp.begin(port);
}

void wireless::send_int(int msg)
{
    udp.beginPacket(client_ip, port);
    udp.print(msg);
    udp.endPacket();
}

void wireless::send_string(char* msg)
{
    udp.beginPacket(client_ip, port);
    udp.print(msg);
    udp.endPacket();
}

void wireless::send_float(float msg)
{
    udp.beginPacket(client_ip, port);
    udp.print(msg);
    udp.endPacket();
}

void wireless::check_connection()
{
    Serial.println(WiFi.status());
}