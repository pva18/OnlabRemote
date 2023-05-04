/**
 ***************************************************************************************************
 * @file rtc.cpp
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Implementation of rtc.h.
 ***************************************************************************************************
 */

#include "rtc.h"

#include <RTClib.h>

/**
 * @brief The RTC object.
 */
RTC_PCF8523 rtc;

/**
 * @brief Initializes the RTC.
 * @return True if the initialization was successful, false otherwise.
 */
bool RTC_Init(void)
{
    if (!rtc.begin(&Wire))
    {
        return false;
    }

    if (!rtc.initialized() || rtc.lostPower())
    {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    rtc.start();

    return true;
}

/**
 * @brief Gets the current time from the RTC.
 * @return The current time.
 */
unixtime_t RTC_GetTime(void)
{
    return rtc.now().unixtime();
}

/**
 * @brief Sets the current time of the RTC.
 * @param time The time to set.
 */
void RTC_SetTime(unixtime_t time)
{
    rtc.adjust(DateTime(time));
}
