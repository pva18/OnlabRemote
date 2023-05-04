#ifndef RTC_H
#define RTC_H

#include <stdint.h>

#define UNIXTIME_TO_HOUR_OF_DAY(TIME) ((TIME / 3600) % 24)

#define UNIXTIME_TO_MINUTE_OF_HOUR(TIME) ((TIME / 60) % 60)

#define UNIXTIME_TO_SECOND_OF_MINUTE(TIME) (TIME % 60)

typedef uint32_t unixtime_t;

bool RTC_Init(void);

unixtime_t RTC_GetTime(void);

void RTC_SetTime(unixtime_t time);

#endif /* RTC_H */