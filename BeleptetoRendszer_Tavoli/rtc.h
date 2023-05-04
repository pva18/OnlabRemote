/**
 ***************************************************************************************************
 * @file rtc.h
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Header file for the RTC functionality.
 ***************************************************************************************************
 */

#ifndef RTC_H
#define RTC_H

#include <stdint.h>

/**
 * @defgroup time_conversions Time conversions
 * @brief Macros for converting between time units.
 * @{
 */
/**
 * @brief Converts UNIX time to the hour of the day.
 * @param TIME The UNIX time to convert.
 */
#define UNIXTIME_TO_HOUR_OF_DAY(TIME) ((TIME / 3600) % 24)
/**
 * @brief Converts UNIX time to the minute of the hour.
 * @param TIME The UNIX time to convert.
 */
#define UNIXTIME_TO_MINUTE_OF_HOUR(TIME) ((TIME / 60) % 60)
/**
 * @brief Converts UNIX time to the second of the minute.
 * @param TIME The UNIX time to convert.
 */
#define UNIXTIME_TO_SECOND_OF_MINUTE(TIME) (TIME % 60)
/** @} */

/**
 * @brief Type for storing the time in seconds since 1970-01-01 00:00:00, aka. UNIX time.
 */
typedef uint32_t unixtime_t;

bool RTC_Init(void);

unixtime_t RTC_GetTime(void);

void RTC_SetTime(unixtime_t time);

#endif /* RTC_H */
