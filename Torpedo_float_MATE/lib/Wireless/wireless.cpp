#include "wireless.h"

wireless::wireless()
{
    this->ssid = DEFAULT_SSID;
    this->password = DEFAULT_PASSWORD;
    this->client_ip = DEFAULT_CLIENT_IP;
    this->port = DEFAULT_PORT;
}

void wireless::set_ssid(char* word)
{
    this->ssid = word;
    LOG_INFO("SSID set to: %s", this->ssid);
}

void wireless::set_password(char* word)
{
    if (strlen(word) < PASSWORD_MIN_LENGTH)
    {
        LOG_ERROR("%s", "Password must be at least 8 characters long!");
    }
    
    else
    {
        this->password = word;
        LOG_INFO("Password set to: %s", this->password);
    }
}

void wireless::set_clientIP(char* msg)
{
    this->client_ip = msg;
    LOG_INFO("Client IP set to: %s", this->client_ip);
}

void wireless::set_port(int num)
{
    if((num > MIN_PORT) && (num < MAX_PORT))
    {
        this->port = num;
        LOG_INFO("Port set to: %d", this->port);
    }
    
    else
    {
        LOG_ERROR("%s", "Port is not available!");
    }
}

char* wireless::get_ssid()
{
    return this->ssid;
}

char* wireless::get_password()
{
    return this->password;
}

char* wireless::get_clientIP()
{
    return this->client_ip;
}

IPAddress wireless::get_serverIP()
{
    return WiFi.softAPIP();
}

int wireless::get_port()
{
    return this->port;
}

void wireless::connect_init()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(this->ssid, this->password);
    
    if (!udp.begin(this->port))
    {
        LOG_ERROR("%s", "Failed to start UDP server!");
    }
    else
    {
        LOG_INFO("%s", "UDP server started successfully!");
    }
}

void wireless::send_int(int msg)
{
    udp.beginPacket(this->client_ip, this->port);
    udp.print(msg);
    udp.endPacket();
}

void wireless::send_string(char* msg)
{
    udp.beginPacket(this->client_ip, this->port);
    udp.print(msg);
    udp.endPacket();
}

void wireless::send_float(float msg)
{
    udp.beginPacket(this->client_ip, this->port);
    udp.print(msg);
    udp.endPacket();
}

wl_status_t wireless::check_connection()
{
    return (WiFi.status());
}