/**
 * @file EEPROM_24LC64.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-04-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "EEPROM_24LC64.h"

EEPROM_24LC64::EEPROM_24LC64(TwoWire &i2c, uint8_t device_address_lower_3_bits)
    : _i2c(i2c), _device_address(EEPROM_24LC64_DEVICE_BASE_ADDRESS | device_address_lower_3_bits)
{
}

EEPROM_24LC64::~EEPROM_24LC64()
{
}

void EEPROM_24LC64::writeByte(uint16_t address, uint8_t data)
{
    // Transmit Control Byte
    _i2c.beginTransmission(_device_address);
    // Transmit Address
    _i2c.write(address >> 8);
    _i2c.write(address & 0xFF);
    // Transmit Data
    _i2c.write(data);

    _i2c.endTransmission();
}

void EEPROM_24LC64::writePage(uint16_t address, uint8_t *data, uint8_t length)
{
    // Negative conditions for a valid write
    int is_greater_than_page_size = length > EEPROM_24LC64_PAGE_SIZE;
    int is_page_border_crossed = ((address + length) % EEPROM_24LC64_PAGE_SIZE) < (address % EEPROM_24LC64_PAGE_SIZE);

    // Check if the write is valid
    if (is_greater_than_page_size || is_page_border_crossed)
    {
        return;
    }

    // Transmit Control Byte
    _i2c.beginTransmission(_device_address);
    // Transmit Address
    _i2c.write(address >> 8);
    _i2c.write(address & 0xFF);
    // Transmit Data
    for (uint8_t i = 0; i < length; i++)
    {
        _i2c.write(data[i]);
    }

    _i2c.endTransmission();
}

uint8_t EEPROM_24LC64::readByte(uint16_t address)
{
    // Transmit Control Byte
    _i2c.beginTransmission(_device_address);
    // Transmit Address
    _i2c.write(address >> 8);
    _i2c.write(address & 0xFF);
    // Stop Transmission
    _i2c.endTransmission(false);

    _i2c.requestFrom(_device_address, 1);
    if (_i2c.available())
    {
        return _i2c.read();
    }
    else
    {
        return 0;
    }
}

uint16_t EEPROM_24LC64::readMultiBytes(uint16_t address, uint8_t *data, uint16_t length)
{
    // Transmit Control Byte
    _i2c.beginTransmission(_device_address);
    // Transmit Address
    _i2c.write(address >> 8);
    _i2c.write(address & 0xFF);
    // Stop Transmission
    _i2c.endTransmission(false);

    // Receive Data
    _i2c.requestFrom(_device_address, length);
    uint16_t index = 0;
    while (_i2c.available() && (index < length))
    {
        data[index] = _i2c.read();
        index++;
    }

    return index;
}

uint8_t EEPROM_24LC64::readNextByte()
{
    // Transmit Control Byte and receive data
    _i2c.requestFrom(_device_address, 1);
    if (_i2c.available())
    {
        return _i2c.read();
    }
    else
    {
        return 0;
    }
}

uint16_t EEPROM_24LC64::readNextMultiBytes(uint8_t *data, uint16_t length)
{
    // Transmit Control Byte and receive data
    _i2c.requestFrom(_device_address, length);
    uint16_t index = 0;
    while (_i2c.available() && (index < length))
    {
        data[index] = _i2c.read();
        index++;
    }

    return index;
}
