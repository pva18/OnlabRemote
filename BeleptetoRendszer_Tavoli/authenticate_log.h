/**
 ***************************************************************************************************
 * @file authenticate_log.h
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Header file for the authentication and logging functionality.
 ***************************************************************************************************
 */

#ifndef AUTHENTICATE_LOG_H
#define AUTHENTICATE_LOG_H

#include <stdint.h>

void AUTHENTICATE_LOG_Init(void);

bool AUTHENTICATE_LOG_Authenticate(const uint8_t *uid, uint32_t timestamp);

void AUTHENTICATE_LOG_WriteLog(const uint8_t *uid, uint32_t timestamp);

void AUTHENTICATE_LOG_SetLastTimeUpdate(uint32_t timestamp);

#endif /* AUTHENTICATE_LOG_H */
