/**
 ***************************************************************************************************
 * @file WifiModemWakeupSleep.hpp
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief This class handles the WiFi modem wakeup and sleep using the RAII technique.
 ***************************************************************************************************
 */

#include "WiFi.h"

/**
 * @brief This class handles the WiFi modem wakeup and sleep using the RAII technique.
 */
class WifiModemWakeupSleep
{
private:
    ESP8266WiFiClass &wifi;

public:
    /**
     * @brief Constructor that wakes up the WiFi modem.
     * @param wifi The WiFi object to use.
     */
    WifiModemWakeupSleep(ESP8266WiFiClass &wifi)
        : wifi(wifi)
    {
        wifi.forceSleepWake();
    }

    /**
     * @brief Destructor that puts the WiFi modem to sleep.
     */
    ~WifiModemWakeupSleep()
    {
        wifi.forceSleepBegin();
    }
};
