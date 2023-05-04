/**
 ***************************************************************************************************
 * @file rfid.h
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Header file for the RFID functionality.
 ***************************************************************************************************
 */

#ifndef RFID_H
#define RFID_H

#include <MFRC522v2.h>

#include "MFRC522DriverUART.h"

/**
 * @brief Initializes the RFID reader.
 * @return True if the initialization was successful, false otherwise.
 */
bool RFID_Init(void);

/**
 * @brief Reads the tag from the RFID reader.
 * @return True if the tag was read successfully, false otherwise.
 */
bool RFID_ReadTag(void);

/**
 * @brief Gets the UID of the tag as a string.
 * @return The UID of the tag as a string.
 */
const char *RFID_GetUidAsString(void);

/**
 * @brief Gets the UID of the tag as a byte array.
 * @return The UID of the tag as a byte array.
 */
const uint8_t *RFID_GetUidAsByteArray(void);

#endif /* RFID_H */
