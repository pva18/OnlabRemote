/**
 ***************************************************************************************************
 * @file MFRC522DriverUART.h
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Header file for the MFRC522 driver for UART communication.
 ***************************************************************************************************
 */

#pragma once

#include <SoftwareSerial.h>
#include <MFRC522Driver.h>
#include <MFRC522DriverPin.h>

class MFRC522DriverUART : public MFRC522Driver
{
public:
    /////////////////////////////////////////////////////////////////////////////////////
    // Functions for setting up the Arduino.
    /////////////////////////////////////////////////////////////////////////////////////

    bool init() override;

    /////////////////////////////////////////////////////////////////////////////////////
    // Basic interface functions for communicating with the MFRC522.
    /////////////////////////////////////////////////////////////////////////////////////
    
    void PCD_WriteRegister(const PCD_Register reg, const byte value) override;
    void PCD_WriteRegister(const PCD_Register reg, const byte count, byte *const values) override;
    byte PCD_ReadRegister(const PCD_Register reg) override;
    void PCD_ReadRegister(const PCD_Register reg, const byte count, byte *const values, const byte rxAlign = 0) override;

    // Advanced init with custom chip select method, e.g. with i2c port expander or multiplexer.
    MFRC522DriverUART(SoftwareSerial &uartClass, uint8_t rxPin, uint8_t txPin)
        : MFRC522Driver(), _uartClass(uartClass), _rxPin(rxPin), _txPin(txPin){};

protected:
    // UART communication.
    SoftwareSerial &_uartClass; // UART class.
    uint8_t _txPin;             // TX pin.
    uint8_t _rxPin;             // RX pin.
};
