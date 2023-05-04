/**
 ***************************************************************************************************
 * @file wifi.h
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Header file for the WiFi functionality.
 ***************************************************************************************************
 */

#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>

bool WIFI_Connect(void);

bool WIFI_ClientRequestTime(WiFiClient &client, uint32_t *time);

bool WIFI_ClientRequestNewMemory(WiFiClient &client, uint8_t *buffer, uint16_t size);

bool WIFI_ClientSendMemory(WiFiClient &client, const uint8_t *buffer, uint16_t size);

#endif /* WIFI_H */
