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

#include <stdint.h>

/** @brief The size of the UID of the tag. */
#define RFID_UID_SIZE 10

bool RFID_Init(void);

bool RFID_ReadTag(void);

const char *RFID_GetUidAsString(void);

const uint8_t *RFID_GetUidAsByteArray(void);

bool RFID_UidEquals(const uint8_t *uid1, const uint8_t *uid2);

void RFID_UidCopy(uint8_t *destination, const uint8_t *source);

#endif /* RFID_H */
