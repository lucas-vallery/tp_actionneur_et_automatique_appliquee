/**
 * @file 	hal_abstraction.h
 * @brief 	This file contains all the functions prototypes for
 * 			the hal_abstraction.c file
 *
 * This file contains all the abstraction functions needed to the shell
 */

#pragma once
#include <stdio.h>
#include <usart.h>

/**
 * @def SERIAL_PORT
 * Serial port linked to the computer
 */
#define SERIAL_PORT &huart2

/**
 * @brief Abstraction of HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
 */
int serial_transmit(const uint8_t *pData, uint16_t size, uint32_t timeout);

/**
 * @brief Abstraction of HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
 */
int serial_receive(uint8_t *pData, uint16_t size, uint32_t timeout);

