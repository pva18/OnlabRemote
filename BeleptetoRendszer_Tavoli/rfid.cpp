/**
 ***************************************************************************************************
 * @file rfid.cpp
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Implementation of rfid.h.
 ***************************************************************************************************
 */

#include "rfid.h"

#include <stdio.h>

#include "BeleptetoRendszer_Tavoli.h"

/**
 * @brief The buffer for the UID of the tag.
 */
static char rfidBuffer[64];

/**
 * @brief Serial object for the RFID reader.
 */
SoftwareSerial rfidSerial;
/**
 * @brief Instance of the MFRC522 driver for the RFID reader.
 */
MFRC522DriverUART rfidDriver(rfidSerial, RFID_RX_PIN, RFID_TX_PIN);
/**
 * @brief Instance of the MFRC522 class for the RFID reader.
 */
MFRC522 mfrc522(rfidDriver);

bool RFID_Init(void)
{
    return mfrc522.PCD_Init();
}

bool RFID_ReadTag(void)
{
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return false;
    }

    // Select one of the cards.
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return false;
    }

    return true;
}

const char *RFID_GetUidAsString(void)
{
    for (int i = 0; i < mfrc522.uid.size; i++)
    {
        sprintf(rfidBuffer + i * 2, "%02X", mfrc522.uid.uidByte[i]);
    }
    return rfidBuffer;
}

const uint8_t *RFID_GetUidAsByteArray(void)
{
    return mfrc522.uid.uidByte;
}
