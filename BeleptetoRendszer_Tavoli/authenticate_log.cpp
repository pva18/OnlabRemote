/**
 ***************************************************************************************************
 * @file authenticate_log.cpp
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Implementation of authenticate_log.h.
 ***************************************************************************************************
 */

#include "authenticate_log.h"

#include <cstring>

#include "eeprom.h"

/**
 * @defgroup authlog_sizes Authlog sizes
 * @brief The sizes of the authentication and log structures.
 * @{
 */
#define UID_SIZE 10
#define NAME_SIZE 16
#define AUTHENTICATE_SIZE (UID_SIZE + NAME_SIZE + 4)
#define LOG_SIZE (UID_SIZE + 4)
/** @} */

/**
 * @defgroup eeprom_header_addresses EEPROM header addresses
 * @brief The addresses of the header fields in the EEPROM.
 * @{
 */
#define AUTHENTICATE_HEADER_SIZE_ADDRESS 0
#define AUTHENTICATE_LENGTH_ADDRESS 2
#define AUTHENTICATE_BASE_ADDRESS_ADDRESS 4
#define LOG_LENGTH_ADDRESS 6
#define LOG_BASE_ADDRESS_ADDRESS 8
#define LAST_TIME_UPDATE_ADDRESS 10
/** @} */

/**
 * @brief The header structure in the EEPROM.
 */
typedef struct _eeprom_header_t
{
    uint16_t headerSize;
    uint16_t authenticationLength;
    uint16_t authenticationBaseAddress;
    uint16_t logLength;
    uint16_t logBaseAddress;
    uint32_t lastTimeUpdate;
} eeprom_header_t;

/**
 * @brief The authenticate structure.
 */
typedef struct _authenticate_t
{
    uint8_t uid[UID_SIZE];
    char name[NAME_SIZE + 1];
    uint8_t beginHour;
    uint8_t beginMinute;
    uint8_t endHour;
    uint8_t endMinute;
} authenticate_t;

/**
 * @brief The log structure.
 */
typedef struct _log_t
{
    uint8_t uid[UID_SIZE];
    uint32_t timestamp;
} log_t;

/**
 * @brief The header structure in the EEPROM.
 */
eeprom_header_t eepromHeader;

/**
 * @brief Initializes the authenticate log module.
 */
void AUTHENTICATE_LOG_Init(void)
{
    // Read header from EEPROM

    uint8_t buffer[4];
    EEPROM_Read(AUTHENTICATE_HEADER_SIZE_ADDRESS, buffer, 2);
    eepromHeader.headerSize = ((uint16_t)(buffer[0]) << 8) | buffer[1];

    EEPROM_Read(AUTHENTICATE_LENGTH_ADDRESS, buffer, 2);
    eepromHeader.authenticationLength = ((uint16_t)(buffer[0]) << 8) | buffer[1];

    EEPROM_Read(AUTHENTICATE_BASE_ADDRESS_ADDRESS, buffer, 2);
    eepromHeader.authenticationBaseAddress = ((uint16_t)(buffer[0]) << 8) | buffer[1];

    EEPROM_Read(LOG_LENGTH_ADDRESS, buffer, 2);
    eepromHeader.logLength = ((uint16_t)(buffer[0]) << 8) | buffer[1];

    EEPROM_Read(LOG_BASE_ADDRESS_ADDRESS, buffer, 2);
    eepromHeader.logBaseAddress = ((uint16_t)(buffer[0]) << 8) | buffer[1];

    EEPROM_Read(LAST_TIME_UPDATE_ADDRESS, buffer, 4);
    eepromHeader.lastTimeUpdate = ((uint32_t)(buffer[0]) << 24) |
                                  ((uint32_t)(buffer[1]) << 16) |
                                  ((uint32_t)(buffer[2]) << 8) |
                                  buffer[3];
}

/**
 * @brief Authenticates the given uid.
 * @param uid The uid to authenticate.
 * @param timestamp The timestamp of the authentication.
 * @return True if the authentication was successful, false otherwise.
 */
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
        EEPROM_Read(read_address, (uint8_t *)authenticate.name, NAME_SIZE);
        read_address += NAME_SIZE;
        EEPROM_Read(read_address, &(authenticate.beginHour), 1);
        read_address += 1;
        EEPROM_Read(read_address, &(authenticate.beginMinute), 1);
        read_address += 1;
        EEPROM_Read(read_address, &(authenticate.endHour), 1);
        read_address += 1;
        EEPROM_Read(read_address, &(authenticate.endMinute), 1);

        // Check if the uid is the same and the timestamp is in the interval
        uint32_t interval_begin = (uint32_t)(authenticate.beginHour) * (60 * 60) +
                                  (uint32_t)(authenticate.beginMinute) * 60;
        uint32_t interval_end = (uint32_t)(authenticate.endHour) * (60 * 60) +
                                (uint32_t)(authenticate.endMinute) * 60;

        if ((memcmp(uid, authenticate.uid, UID_SIZE) == 0) &&
            (timestamp % (60 * 60 * 24) >= interval_begin) &&
            (timestamp % (60 * 60 * 24) <= interval_end))
        {
            // Return true if the authetication is successful
            return true;
        }
    }

    // Return false if the authetication is unsuccessful
    return false;
}

/**
 * @brief Writes a log to the EEPROM.
 * @param uid The uid to write.
 * @param timestamp The timestamp to write.
 */
void AUTHENTICATE_LOG_WriteLog(const uint8_t *uid, uint32_t timestamp)
{
    uint8_t log[LOG_SIZE];

    // Get the next address
    uint16_t log_next_address = eepromHeader.logBaseAddress + eepromHeader.logLength;

    uint8_t buffer[4];
    buffer[0] = (uint8_t)(timestamp >> 24);
    buffer[1] = (uint8_t)((timestamp >> 16) & 0xFF);
    buffer[2] = (uint8_t)((timestamp >> 8) & 0xFF);
    buffer[3] = (uint8_t)(timestamp & 0xFF);

    // Build the log from the log number, the uid and the timestamp
    memcpy(log, uid, UID_SIZE);
    memcpy(log + UID_SIZE, buffer, 4);

    // Write the log to the EEPROM
    EEPROM_Write(log_next_address, log, LOG_SIZE);

    // Update the log length
    eepromHeader.logLength += LOG_SIZE;
    buffer[0] = (uint8_t)(eepromHeader.logLength >> 8);
    buffer[1] = (uint8_t)(eepromHeader.logLength & 0xFF);
    EEPROM_Write(LOG_LENGTH_ADDRESS, buffer, 2);

    // Commit the changes
    EEPROM_MemoryImage_Commit();
}

/**
 * @brief Set the last time update.
 * @param timestamp The timestamp to set.
 */
void AUTHENTICATE_LOG_SetLastTimeUpdate(uint32_t timestamp)
{
    // Update the last time update
    eepromHeader.lastTimeUpdate = timestamp;
    uint8_t buffer[4];
    buffer[0] = (uint8_t)(timestamp >> 24);
    buffer[1] = (uint8_t)((timestamp >> 16) & 0xFF);
    buffer[2] = (uint8_t)((timestamp >> 8) & 0xFF);
    buffer[3] = (uint8_t)(timestamp & 0xFF);
    EEPROM_Write(LAST_TIME_UPDATE_ADDRESS, buffer, 4);

    // Commit the changes
    EEPROM_MemoryImage_Commit();
}
