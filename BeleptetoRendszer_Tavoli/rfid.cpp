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
#include <stdint.h>

#include <PN532_SWHSU.h>
#include <PN532.h>

#include "BeleptetoRendszer_Tavoli.h"

/**
 * @brief The buffer for the UID of the tag.
 */
static char rfidBuffer[64];

/**
 * @brief Serial object for the RFID reader.
 */
SoftwareSerial rfidSerial(RFID_RX_PIN, RFID_TX_PIN);
PN532_SWHSU pn532swhsu(rfidSerial);
PN532 nfc(pn532swhsu);

uint8_t uid[RFID_UID_SIZE];

/**
 * @brief Initializes the RFID reader module.
 * @return True if the initialization was successful, false otherwise.
 */
bool RFID_Init(void)
{
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata)
    {
        return false;
    }
    nfc.SAMConfig();

    return true;
}

/**
 * @brief Reads the tag from the RFID reader.
 * @return True if the tag was read successfully, false otherwise.
 */
bool RFID_ReadTag(void)
{
    bool success;

    uint8_t uid_read[RFID_UID_SIZE] = {0};
    uint8_t uidLength;
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid_read, &uidLength);
    if (!success)
    {
        return false;
    }

    // Copy the uid to the last bytes of the uid array
    for (uint8_t i = 0; i < uidLength; i++)
    {
        uid[RFID_UID_SIZE - 1 - i] = uid_read[uidLength - 1 - i];
    }
    // Fill the rest of the array with zeros
    for (uint8_t i = 0; i < RFID_UID_SIZE - uidLength; i++)
    {
        uid[i] = 0;
    }

    return true;
}

/**
 * @brief Gets the UID of the tag as a string.
 * @return The UID of the tag as a string.
 * @note Format: "ff(#RFID_UID_SIZE times)"
 */
const char *RFID_GetUidAsString(void)
{
    for (int i = 0; i < RFID_UID_SIZE; i++)
    {
        sprintf(rfidBuffer + i * 2, "%02x", uid[i]);
    }
    return rfidBuffer;
}

/**
 * @brief Gets the UID of the tag as a byte array with the size of #RFID_UID_SIZE.
 * @return The UID of the tag as a byte array.
 */
const uint8_t *RFID_GetUidAsByteArray(void)
{
    return uid;
}

/**
 * @brief Compares two UIDs.
 * @param uid1 The first UID.
 * @param uid2 The second UID.
 * @return True if the UIDs are equal, false otherwise.
 */
bool RFID_UidEquals(const uint8_t *uid1, const uint8_t *uid2)
{
    for (uint8_t i = 0; i < RFID_UID_SIZE; i++)
    {
        if (uid1[i] != uid2[i])
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Copies the UID from the source to the destination.
 * @param destination The destination of the copy.
 * @param source The source of the copy.
 */
void RFID_UidCopy(uint8_t *destination, const uint8_t *source)
{
    for (uint8_t i = 0; i < RFID_UID_SIZE; i++)
    {
        destination[i] = source[i];
    }
}
