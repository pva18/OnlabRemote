/**
 ***************************************************************************************************
 * @file BeleptetoRendszer_Tavoli.h
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Header file of the remote access control system, containing the hardware configuration of
 * the module.
 ***************************************************************************************************
 */

#ifndef BELEPTETORENDSZER_TAVOLI_H
#define BELEPTETORENDSZER_TAVOLI_H

/**
 * @defgroup PINOUT Pinout
 * @brief Pinout of the module.
 * @{
 */
#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 14

#define WAKEUP_PIN 16
#define WAKEUP_PRESSED LOW

#define RELAY_SWITCH_PIN 15
#define RELAY_OFF LOW
#define RELAY_ON HIGH

#define RFID_RX_PIN 5
#define RFID_TX_PIN 4
#define RFID_IRQ_PIN 0

#define LED_RED_PIN 12
#define LED_GREEN_PIN 13
#define LED_ON LOW
#define LED_OFF HIGH
/** @} */

#endif /* BELEPTETORENDSZER_TAVOLI_H */
