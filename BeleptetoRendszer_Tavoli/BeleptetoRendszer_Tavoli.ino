/**
 * @file BeleptetoRendszer_Tavoli.ino
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-04-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "BeleptetoRendszer_Tavoli.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <RTClib.h>

#include "eeprom.h"
#include "wifi.h"
#include "rfid.h"
#include "authenticate_log.h"
#include "timers.h"
#include "rtc.h"

#define DEBUG 0

#ifdef DEBUG
#define DEBUG_PRINT(str) Serial.print(str)
#else
#define DEBUG_PRINT(str)
#endif /* DEBUG */

#define ACTIVE_TIME_MS 15000
#define SLEEP_TIME_MS 1000
#define WIFI_ACTIVE_HOUR 1

bool checkActivity(void);
bool checkWiFiActivity(void);

void handleWiFi(void);
void handleRFID(void);

static uint8_t temporary_image[EEPROM_SIZE];

RTC_PCF8523 rtc;

void setup(void)
{
#ifdef DEBUG
    Serial.begin(115200);
    delay(10);
    DEBUG_PRINT("\r\n");
    Serial.println("Belepteto rendszer tavoli");
    Serial.println();
#endif /* DEBUG */

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    EEPROM_Init();

    if (!RTC_Init())
    {
        DEBUG_PRINT("RTC init failed.\r\n");
    }

    if (!RFID_Init())
    {
        DEBUG_PRINT("RFID init failed.\r\n");
    }

    pinMode(WAKEUP_PIN, INPUT);

    pinMode(RELAY_SWITCH_PIN, OUTPUT);
    digitalWrite(RELAY_SWITCH_PIN, RELAY_OFF);

    pinMode(LED_RED_PIN, OUTPUT);
    digitalWrite(LED_RED_PIN, LED_OFF);

    pinMode(LED_GREEN_PIN, OUTPUT);
    digitalWrite(LED_GREEN_PIN, LED_OFF);

    pinMode(RFID_IRQ_PIN, OUTPUT);
    digitalWrite(RFID_IRQ_PIN, LOW);
}

// Activity check
// RFID authentication
// WiFi connection
// EEPROM write
// Sleep

void loop(void)
{
    // Always handle the timers
    TIMERS_HandleEvents(millis());

    if (checkActivity())
    {
        // If there is user activity, handle the RFID authentication
        handleRFID();
    }
    else
    {
        // Only communicate with the central module if there is no user activity
        handleWiFi();
    }

    // Go to sleep
    delay(SLEEP_TIME_MS);
}

/**
 * @brief Check if the module is active
 * @return True if the module is active, false otherwise
 */
bool checkActivity(void)
{
    static unsigned long startMillis = 0;
    static bool isActive = false;

    // Check if the wakeup button is pressed
    if (digitalRead(WAKEUP_PIN) == WAKEUP_PRESSED)
    {
        // Put the module to active state and start the timer
        isActive = true;
        startMillis = millis();
    }
    else if (isActive && ((millis() - startMillis) > ACTIVE_TIME_MS))
    {
        // If the timer is over, put the module to inactive state
        isActive = false;
    }

    return isActive;
}

/**
 * @brief Handle the WiFi connection and communication
 */
void handleWiFi(void)
{
    if (!checkWiFiActivity())
    {
        return;
    }

    // Wake up WiFi modem and communicate with the central module
    WiFi.forceSleepWake();

    if (!WIFI_Connect())
    {
        DEBUG_PRINT("WiFi connection failed\r\n");
        WiFi.forceSleepBegin();
        return;
    }

    WiFiClient client;

    if (!WIFI_ClientSendMemory(client, EEPROM_GetMemoryImage(), EEPROM_GetSize()))
    {
        DEBUG_PRINT("Sending memory failed\r\n");
        WiFi.forceSleepBegin();
        return;
    }

    if (!WIFI_ClientRequestNewMemory(client, temporary_image, sizeof(temporary_image)))
    {
        DEBUG_PRINT("Requesting new memory failed\r\n");
        WiFi.forceSleepBegin();
        return;
    }
    EEPROM_Write(0, temporary_image, sizeof(temporary_image));
    EEPROM_MemoryImage_Commit();

    uint32_t time = 0;
    if (!WIFI_ClientRequestTime(client, &time))
    {
        DEBUG_PRINT("Requesting time failed\r\n");
        WiFi.forceSleepBegin();
        return;
    }
    RTC_SetTime(time);

    WiFi.forceSleepBegin();
}

bool checkWiFiActivity(void)
{
    static bool wifi_activated = false;

    // Get current time
    unixtime_t time = RTC_GetTime();
    unixtime_t hour_of_day = UNIXTIME_TO_HOUR_OF_DAY(time);

    if ((!wifi_activated) && (hour_of_day == WIFI_ACTIVE_HOUR))
    {
        // WiFi was not activated in this hour and it is time for communication with the central
        // module
        wifi_activated = true;
        return true;
    }
    else if ((wifi_activated) && (hour_of_day != WIFI_ACTIVE_HOUR))
    {
        // WiFi was activated in the last hour and it is no longer the time for WiFi activity
        wifi_activated = false;
    }

    return false;
}

void handleRFID(void)
{
    if (!RFID_ReadTag())
    {
        return;
    }

    DEBUG_PRINT(RFID_GetUidAsString());

    const uint8_t *uid = RFID_GetUidAsByteArray();
    uint32_t time = rtc.now().unixtime();

    if (AUTHENTICATE_LOG_Authenticate(uid, time))
    {
        digitalWrite(RELAY_SWITCH_PIN, RELAY_ON);
        digitalWrite(LED_GREEN_PIN, LED_ON);

        timer_event_t switch_off;
        switch_off.millis_start = millis();
        switch_off.millis_period = 5000;
        switch_off.handler = [](uint32_t __unused)
        {
            digitalWrite(LED_GREEN_PIN, LED_OFF);
            digitalWrite(RELAY_SWITCH_PIN, RELAY_OFF);
        };
        TIMERS_AddEvent(&switch_off);

        AUTHENTICATE_LOG_WriteLog(uid, time);
    }
    else
    {
        digitalWrite(LED_RED_PIN, LED_ON);

        timer_event_t red_led_off;
        red_led_off.millis_start = millis();
        red_led_off.millis_period = 3000;
        red_led_off.handler = [](uint32_t __unused)
        {
            digitalWrite(LED_RED_PIN, LED_OFF);
        };
        TIMERS_AddEvent(&red_led_off);
    }
}
