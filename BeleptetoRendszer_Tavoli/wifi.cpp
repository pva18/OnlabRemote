#include "wifi.h"

#include <Arduino.h>

#define WIFI_CONNECT_TIMEOUT_MS 10000
#define CLIENT_TIMEOUT_MS 5000

#define WIFI_CENTRAL_SSID "belepteto_rendszer"
#define WIFI_CENTRAL_PASS "0123456789abcdef"
#define WIFI_CENTRAL_IP "192.168.4.1"
#define WIFI_CENTRAL_PORT 80

static const char *ssid = WIFI_CENTRAL_SSID;
static const char *password = WIFI_CENTRAL_PASS;
const char *host = WIFI_CENTRAL_IP;
const uint16_t port = WIFI_CENTRAL_PORT;

bool WIFI_ClientWaitForResponse(WiFiClient &client, unsigned long timeout);

/**
 * @brief Connect to the WiFi network
 * @return True if the connection was successful, false otherwise
 */
bool WIFI_Connect(void)
{
    if (WiFi.isConnected())
    {
        return true;
    }

    // Connect to the WiFi network
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Wait for connection
    unsigned long startMillis = millis();
    while ((millis() - startMillis) < WIFI_CONNECT_TIMEOUT_MS)
    {
        delay(500);

        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief Wait for the client response
 * @param client The client
 * @param timeout The timeout in milliseconds
 * @return True if the response was received, false otherwise
 */
bool WIFI_ClientWaitForResponse(WiFiClient &client, unsigned long timeout)
{
    unsigned long startMillis = millis();
    while (client.available() == 0)
    {
        if ((millis() - startMillis) > timeout)
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief Request time from the central module.
 * @param client The client object
 * @param time The time
 * @return True if the time was received, false otherwise
 */
bool WIFI_ClientRequestTime(WiFiClient &client, uint32_t *time)
{
    if (!client.connect(host, port))
    {
        return false;
    }

    // Send the request symbol and the size
    client.print("T 4\n");

    if (!WIFI_ClientWaitForResponse(client, CLIENT_TIMEOUT_MS))
    {
        client.stop();
        return false;
    }

    String response = client.readStringUntil('\n');
    while (client.available())
    {
        client.read();
    }

    long time_parsed = response.toInt();
    if (time_parsed == 0)
    {
        client.stop();
        return false;
    }

    *time = (uint32_t)time_parsed;

    client.stop();
    return true;
}

/**
 * @brief Request new memory from the central module for the EEPROM.
 * @param client The client object
 * @param buffer Buffer to store the received memory
 * @param size The size of the memory
 * @return True if the memory was received, false otherwise
 */
bool WIFI_ClientRequestNewMemory(WiFiClient &client, uint8_t *buffer, uint16_t size)
{
    if (!client.connect(host, port))
    {
        return false;
    }

    // Send the request symbol and the size
    client.print("N ");
    client.print(size);
    client.print('\n');

    if (!WIFI_ClientWaitForResponse(client, CLIENT_TIMEOUT_MS))
    {
        client.stop();
        return false;
    }

    int i = 0;
    while (client.available())
    {
        uint8_t n_byte = (uint8_t)client.read();
        buffer[i] = n_byte;
        i++;
        if (i >= size)
        {
            i = 0;
            break;
        }
    }

    if (i != size)
    {
        client.stop();
        return false;
    }

    client.stop();
    return true;
}

/**
 * @brief Send the EEPROM memory to the central module.
 * @param client The client object
 * @param buffer The buffer that contains the memory
 * @param size The size of the buffer
 * @return True if the memory was sent, false otherwise
 */
bool WIFI_ClientSendMemory(WiFiClient &client, const uint8_t *buffer, uint16_t size)
{
    if (!client.connect(host, port))
    {
        return false;
    }

    // Send the request symbol and the size
    client.print("M ");
    client.print(size);
    client.print('\n');

    if (!WIFI_ClientWaitForResponse(client, CLIENT_TIMEOUT_MS))
    {
        client.stop();
        return false;
    }

    String response = client.readStringUntil('\n');
    while (client.available())
    {
        client.read();
    }

    if (response != "O")
    {
        client.stop();
        return false;
    }

    client.write(buffer, size);

    client.stop();
    return true;
}
