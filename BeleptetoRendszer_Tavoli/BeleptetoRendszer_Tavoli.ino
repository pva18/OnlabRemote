/**
 ***************************************************************************************************
 * @file BeleptetoRendszer_Tavoli.ino
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Main file of the remote access control system.
 * @note The code is written for the ESP8266 platform, and it is compiled with the ESP8266 Arduino
 * Core library.
 ***************************************************************************************************
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
#include "WifiModemWakeupSleep.hpp"

#define DEBUG 0

#ifdef DEBUG
#define DEBUG_PRINT(str) Serial.print(str)
#else
#define DEBUG_PRINT(str)
#endif /* DEBUG */

#define ACTIVE_TIME_MS 15000
#define SLEEP_TIME_MS 1000
#define WIFI_ACTIVE_HOUR 1

void ioPinsInit(void);

bool checkActivity(void);
bool checkWiFiActivity(void);

void handleWiFi(void);
void handleRFID(void);

static uint8_t temporary_image[EEPROM_SIZE];

/**
 * @brief Arduino setup function.
 */
void setup(void)
{
#ifdef DEBUG
    Serial.begin(115200);
    delay(10);
    DEBUG_PRINT("\r\n");
    Serial.println("Belepteto rendszer tavoli");
    Serial.println();
#endif /* DEBUG */

    ioPinsInit();

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
}

/**
 * @brief Arduino loop function.
 */
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
 * @brief Check if the module is active.
 * @return True if the module is active, false otherwise.
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
 * @brief Handle the WiFi connection and communication.
 */
void handleWiFi(void)
{
    // Create a WifiModemWakeupSleep object to handle modem wakeup and sleep
    WifiModemWakeupSleep wifiModemController(WiFi);

    if (!checkWiFiActivity())
    {
        return;
    }

    // Wake up WiFi modem and communicate with the central module
    WiFi.forceSleepWake();

    if (!WIFI_Connect())
    {
        DEBUG_PRINT("WiFi connection failed\r\n");
        return;
    }

    WiFiClient client;

    if (!WIFI_ClientSendMemory(client, EEPROM_GetMemoryImage(), EEPROM_GetSize()))
    {
        DEBUG_PRINT("Sending memory failed\r\n");
        return;
    }

    if (!WIFI_ClientRequestNewMemory(client, temporary_image, sizeof(temporary_image)))
    {
        DEBUG_PRINT("Requesting new memory failed\r\n");
        return;
    }
    EEPROM_Write(0, temporary_image, sizeof(temporary_image));
    EEPROM_MemoryImage_Commit();

    uint32_t time = 0;
    if (!WIFI_ClientRequestTime(client, &time))
    {
        DEBUG_PRINT("Requesting time failed\r\n");
        return;
    }
    RTC_SetTime(time);
}

/**
 * @brief Check if it is time for WiFi activity.
 * @return True if it is time for WiFi activity, false otherwise.
 */
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

/**
 * @brief Handle the RFID authentication.
 */
void handleRFID(void)
{
    if (!RFID_ReadTag())
    {
        // No tag was read
        return;
    }

    DEBUG_PRINT(RFID_GetUidAsString());
    DEBUG_PRINT("\r\n");

    const uint8_t *uid = RFID_GetUidAsByteArray();
    unixtime_t time = RTC_GetTime();

    if (AUTHENTICATE_LOG_Authenticate(uid, time))
    {
        // If the user is authenticated, switch on the relay and the green LED
        digitalWrite(RELAY_SWITCH_PIN, RELAY_ON);
        digitalWrite(LED_GREEN_PIN, LED_ON);

        // Switch off the relay and the green LED after 5 seconds
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
        // If the user is not authenticated, switch on the red LED
        digitalWrite(LED_RED_PIN, LED_ON);

        // Switch off the red LED after 3 seconds
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

void ioPinsInit(void)
{
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
