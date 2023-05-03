#include "rfid.h"

#include <stdio.h>

#include "BeleptetoRendszer_Tavoli.h"

static char rfidBuffer[64];

SoftwareSerial rfidSerial;
MFRC522DriverUART rfidDriver(rfidSerial, RFID_RX_PIN, RFID_TX_PIN);
MFRC522 mfrc522(rfidDriver);

bool RFID_Init(void)
{
    return mfrc522.PCD_Init();
}

bool RFID_ReadTag(void)
{
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
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