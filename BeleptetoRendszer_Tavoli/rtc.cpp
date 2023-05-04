#include "rtc.h"

#include <RTClib.h>

RTC_PCF8523 rtc;

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

unixtime_t RTC_GetTime(void)
{
    return rtc.now().unixtime();
}

void RTC_SetTime(unixtime_t time)
{
    rtc.adjust(DateTime(time));
}
