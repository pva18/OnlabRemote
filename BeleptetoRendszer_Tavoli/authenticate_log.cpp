#include "authenticate_log.h"

#include <cstring>

#include "eeprom.h"

#define UID_SIZE 10

#define AUTHENTICATE_SIZE (UID_SIZE + 4)

#define LOG_SIZE (UID_SIZE + 6)

/**
 * @defgroup eeprom_header_addresses EEPROM header addresses
 * @{
 */
#define AUTHENTICATE_HEADER_SIZE_ADDRESS 0
#define AUTHENTICATE_LENGTH_ADDRESS 2
#define AUTHENTICATE_BASE_ADDRESS_ADDRESS 4
#define LOG_LENGTH_ADDRESS 6
#define LOG_BASE_ADDRESS_ADDRESS 8
#define LAST_TIME_UPDATE_ADDRESS 10
/** @} */

typedef struct _eeprom_header_t
{
    uint16_t headerSize;
    uint16_t authenticationLength;
    uint16_t authenticationBaseAddress;
    uint16_t logLength;
    uint16_t logBaseAddress;
    uint32_t lastTimeUpdate;
} eeprom_header_t;

typedef struct _authenticate_t
{
    uint8_t uid[UID_SIZE];
    uint8_t beginHour;
    uint8_t beginMinute;
    uint8_t endHour;
    uint8_t endMinute;
} authenticate_t;

typedef struct _log_t
{
    uint16_t logNumber;
    uint8_t uid[UID_SIZE];
    uint32_t timestamp;
} log_t;

eeprom_header_t eepromHeader;

void AUTHENTICATE_LOG_Init(void)
{
    // Read header from EEPROM
    EEPROM_Read(AUTHENTICATE_HEADER_SIZE_ADDRESS,
                (uint8_t *)(&(eepromHeader.headerSize)),
                2);
    EEPROM_Read(AUTHENTICATE_LENGTH_ADDRESS,
                (uint8_t *)(&(eepromHeader.authenticationLength)),
                2);
    EEPROM_Read(AUTHENTICATE_BASE_ADDRESS_ADDRESS,
                (uint8_t *)(&(eepromHeader.authenticationBaseAddress)),
                2);
    EEPROM_Read(LOG_LENGTH_ADDRESS,
                (uint8_t *)(&(eepromHeader.logLength)),
                2);
    EEPROM_Read(LOG_BASE_ADDRESS_ADDRESS,
                (uint8_t *)(&(eepromHeader.logBaseAddress)),
                2);
    EEPROM_Read(LAST_TIME_UPDATE_ADDRESS,
                (uint8_t *)(&(eepromHeader.lastTimeUpdate)),
                4);
}

bool AUTHENTICATE_LOG_Authenticate(const uint8_t *uid, uint32_t timestamp)
{
    // Iterate through the authenticate structures
    for (uint16_t i = 0; i < eepromHeader.authenticationLength; i += AUTHENTICATE_SIZE)
    {
        authenticate_t authenticate;
        uint16_t read_address = eepromHeader.authenticationBaseAddress + i;

        // Read authenticate structure from EEPROM
        EEPROM_Read(read_address, authenticate.uid, UID_SIZE);
        read_address += UID_SIZE;
        EEPROM_Read(read_address, &(authenticate.beginHour), 1);
        read_address += 1;
        EEPROM_Read(read_address, &(authenticate.beginMinute), 1);
        read_address += 1;
        EEPROM_Read(read_address, &(authenticate.endHour), 1);
        read_address += 1;
        EEPROM_Read(read_address, &(authenticate.endMinute), 1);

        // Check if the uid is the same and the timestamp is in the interval
        uint32_t interval_begin = (uint32_t)(authenticate.beginHour) * 3600 +
                                  (uint32_t)(authenticate.beginMinute) * 60;
        uint32_t interval_end = (uint32_t)(authenticate.endHour) * 3600 +
                                (uint32_t)(authenticate.endMinute) * 60;
        bool is_time_greater_interval_begin = (timestamp % 86400 >= interval_begin);
        bool is_time_smaller_interval_end = (timestamp % 86400 <= interval_end);

        if ((memcmp(uid, authenticate.uid, UID_SIZE) == 0) &&
            is_time_greater_interval_begin && is_time_smaller_interval_end)
        {
            // Return true if the authetication is successful
            return true;
        }
    }

    // Return false if the authetication is unsuccessful
    return false;
}

void AUTHENTICATE_LOG_WriteLog(const uint8_t *uid, uint32_t timestamp)
{
    uint8_t log[LOG_SIZE];

    // Get the next log number and address
    uint16_t log_next_number = eepromHeader.logLength / LOG_SIZE + 1;
    uint16_t log_next_address = eepromHeader.logBaseAddress + log_next_number * LOG_SIZE;

    // Build the log from the log number, the uid and the timestamp
    memcpy(log, &log_next_number, 2);
    memcpy(log + 2, uid, UID_SIZE);
    memcpy(log + 2 + UID_SIZE, &timestamp, 4);

    // Write the log to the EEPROM
    EEPROM_Write(log_next_address, log, LOG_SIZE);

    // Update the log length
    eepromHeader.logLength += LOG_SIZE;
    EEPROM_Write(LOG_LENGTH_ADDRESS, (uint8_t *)(&(eepromHeader.logLength)), 2);

    // Commit the changes
    EEPROM_MemoryImage_Commit();
}

void AUTHENTICATE_LOG_SetLastTimeUpdate(uint32_t timestamp)
{
    // Update the last time update
    eepromHeader.lastTimeUpdate = timestamp;
    EEPROM_Write(LAST_TIME_UPDATE_ADDRESS, (uint8_t *)(&(eepromHeader.lastTimeUpdate)), 4);

    // Commit the changes
    EEPROM_MemoryImage_Commit();
}
