#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

#define EEPROM_SIZE 8192

/**
 * @brief Initialize the EEPROM.
 */
void EEPROM_Init(void);

/**
 * @brief Get the size of the EEPROM.
 * @return The size of the EEPROM in bytes.
 */
uint16_t EEPROM_GetSize(void);

/**
 * @brief Get the memory image of the EEPROM.
 * @return Pointer to the memory image of the EEPROM.
 */
const uint8_t *EEPROM_GetMemoryImage(void);

/**
 * @brief Write data to the EEPROM.
 * @note The data is written to the memory image, not to the EEPROM. Commit the memory image to write the data to the EEPROM.
 * @param address The address to write to.
 * @param data The data to write.
 * @param length The length of the data.
 */
void EEPROM_Write(uint16_t address, uint8_t *data, uint16_t length);

/**
 * @brief Read data from the EEPROM.
 * @note The data is read from the memory image, not from the EEPROM. Update the memory image to read the data actually stored in the EEPROM.
 * @param address The address to read from.
 * @param data The data to read.
 * @param length The length of the data.
 */
void EEPROM_Read(uint16_t address, uint8_t *data, uint16_t length);

/**
 * @brief Update the EEPROM memory image.
 */
void EEPROM_MemoryImage_Update(void);
/**
 * @brief Commit the EEPROM memory image.
 */
void EEPROM_MemoryImage_Commit(void);

#endif /* EEPROM_H */
