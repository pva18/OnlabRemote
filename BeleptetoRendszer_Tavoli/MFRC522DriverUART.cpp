/**
 ***************************************************************************************************
 * @file MFRC522DriverUART.cpp
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Implementation of MFRC522DriverUART.h.
 ***************************************************************************************************
 */

#include "MFRC522DriverUART.h"

/////////////////////////////////////////////////////////////////////////////////////
// Basic interface functions for communicating with the MFRC522DriverUART
/////////////////////////////////////////////////////////////////////////////////////

bool MFRC522DriverUART::init()
{
    _uartClass.begin(9600, EspSoftwareSerial::Config::SWSERIAL_8N1, _rxPin, _txPin);

    return true;
}

/**
 * Writes a byte to the specified register.
 * @param reg The register to write to. One of the PCD_Register enums.
 * @param value The value to write.
 */
void MFRC522DriverUART::PCD_WriteRegister(const PCD_Register reg, const byte value)
{
    _uartClass.write((byte)reg);
    _uartClass.write(value);
} // End PCD_WriteRegister()

/**
 * Writes a number of bytes to the specified register.
 * @param reg The register to write to. One of the PCD_Register enums.
 * @param count The number of bytes to write to the register
 * @param values The values to write. Byte array.
 */
void MFRC522DriverUART::PCD_WriteRegister(const PCD_Register reg, const byte count, byte *const values)
{
    for (int i = 0; i < count; i++)
    {
        _uartClass.write((byte)reg);
        _uartClass.write(values[i]);
    }
} // End PCD_WriteRegister()

/**
 * Reads a byte from the specified register in the MFRC522DriverSPI chip.
 * @param reg The register to read from. One of the PCD_Register enums.
 */
byte MFRC522DriverUART::PCD_ReadRegister(const PCD_Register reg)
{
    byte value;

    _uartClass.write(reg | 0x80);
    value = (byte)_uartClass.read();

    return value;
} // End PCD_ReadRegister()

/**
 * Reads a number of bytes from the specified register in the MFRC522DriverSPI chip.
 * @param reg The register to read from. One of the PCD_Register enums.
 * @param count The number of bytes to read
 * @param values Byte array to store the values in.
 * @param rxAlign Only bit positions rxAlign..7 in values[0] are updated.
 */
void MFRC522DriverUART::PCD_ReadRegister(const PCD_Register reg, const byte count,
                                         byte *const values, const byte rxAlign)
{
    byte address = reg | 0x80;
    byte value;
    byte mask = (byte)(0xFF << rxAlign) & 0xFF;

    if (count == 0)
    {
        return;
    }

    if (rxAlign)
    {
        _uartClass.write(address);
        value = (byte)_uartClass.read();
        values[0] = (values[0] & ~mask) | (value & mask);
    }

    for (int i = 1; i < count; i++)
    {
        _uartClass.write(address);
        value = (byte)_uartClass.read();
        values[i] = value;
    }
} // End PCD_ReadRegister()
