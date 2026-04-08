#include "wireless.h"

wireless::wireless()
{
    strncpy(this->ssid ,DEFAULT_SSID, sizeof(this->ssid));
    this->ssid[sizeof(this->ssid) - 1] = '\0';
    
    strncpy(this->password ,DEFAULT_PASSWORD,sizeof(this->password));
    this->password[sizeof(this->password) - 1] = '\0';

    this->client_ip = DEFAULT_CLIENT_IP;
    this->port = DEFAULT_PORT;
}

void wireless::set_ssid(const char* word)
{
    strncpy(this->ssid ,word, sizeof(this->ssid));
    this->ssid[sizeof(this->ssid) - 1] = '\0';
    LOG_INFO("SSID set to: %s", this->ssid);
}

void wireless::set_password(const char* word)
{
    if (strlen(word) < PASSWORD_MIN_LENGTH)
    {
        LOG_ERROR("%s", "Password must be at least 8 characters long!");
        return;
    }
    
    else
    {
        if(strlen(word) >= sizeof(this->password))
        {
            LOG_WARN("%s", "Password is too long! Maximum length is 31 characters.");
        }

        strncpy(this->password ,word, sizeof(this->password));
        this->password[sizeof(this->password) - 1] = '\0';
        LOG_INFO("Password set to: %s", this->password);
    }
}

void wireless::set_clientIP(IPAddress msg)
{
    this->client_ip = msg;
    LOG_INFO("Client IP set to: %s", this->client_ip.toString().c_str());
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

const char* wireless::get_ssid() const
{
    return this->ssid;
}

const char* wireless::get_password() const
{
    return this->password;
}

const IPAddress wireless::get_clientIP() const
{
    return this->client_ip;
}
    
const IPAddress wireless::get_serverIP() const
{
    return WiFi.softAPIP();
}

int wireless::get_port() const
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

wl_status_t wireless::check_connection() const
{
    return (WiFi.status());
}