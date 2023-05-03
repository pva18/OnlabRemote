#ifndef RFID_H
#define RFID_H

#include <MFRC522v2.h>

#include "MFRC522DriverUART.h"

bool RFID_Init(void);

bool RFID_ReadTag(void);

const char *RFID_GetUidAsString(void);

const uint8_t *RFID_GetUidAsByteArray(void);

#endif /* RFID_H */
