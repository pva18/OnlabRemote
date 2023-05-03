#include "eeprom.h"

#include "EEPROM_24LC64.h"

#define EEPROM_ADDRESS_LOWER_3_BITS 0b111

EEPROM_24LC64 eeprom(Wire, EEPROM_ADDRESS_LOWER_3_BITS);

static uint8_t memoryImage[EEPROM_24LC64_SIZE];

static bool updatedPage[EEPROM_24LC64_SIZE_IN_PAGES];

void EEPROM_Init()
{
    EEPROM_MemoryImage_Update();
}

uint16_t EEPROM_GetSize(void)
{
    return EEPROM_24LC64_SIZE;
}

const uint8_t *EEPROM_GetMemoryImage(void)
{
    return memoryImage;
}

void EEPROM_Write(uint16_t address, uint8_t *data, uint16_t length)
{
    if (address + length > EEPROM_24LC64_SIZE)
    {
        // Trying to write outside of the EEPROM
        return;
    }

    for (uint16_t i = 0; i < length; i++)
    {
        memoryImage[address + i] = data[i];
        updatedPage[(address + i) / EEPROM_24LC64_PAGE_SIZE] = true;
    }
}

void EEPROM_Read(uint16_t address, uint8_t *data, uint16_t length)
{
    if (address + length > EEPROM_24LC64_SIZE)
    {
        // Trying to read outside of the EEPROM
        return;
    }
    
    for (uint16_t i = 0; i < length; i++)
    {
        data[i] = memoryImage[address + i];
    }
}

void EEPROM_MemoryImage_Update(void)
{
    eeprom.readMultiBytes(0, memoryImage, EEPROM_24LC64_SIZE);
}

void EEPROM_MemoryImage_Commit(void)
{
    // Note: At each write operation, the EEPROM updates the whole page that contains the write
    // address. Therefore, it is most efficient to write data in page size chunks.

    // Make sure the EEPROM is not busy
    delay(EEPROM_24LC64_WRITE_DELAY_MS);

    // Write all updated pages to the EEPROM
    for (uint16_t i = 0; i < EEPROM_24LC64_SIZE_IN_PAGES; i++)
    {
        if (!updatedPage[i])
        {
            // Only write pages that have been updated
            continue;
        }
        eeprom.writePage(i * EEPROM_24LC64_PAGE_SIZE, &memoryImage[i * EEPROM_24LC64_PAGE_SIZE], EEPROM_24LC64_PAGE_SIZE);
        delay(EEPROM_24LC64_WRITE_DELAY_MS);
    }
}
