/**
 * @file 	shell.h
 * @brief 	Header of the shell.c file
 *
 * This file contains all the define and typedef needed to the shell
 */


#pragma once

#include <stdlib.h>
#include <stdio.h>


#define UART_TX_BUFFER_SIZE 64
#define UART_RX_BUFFER_SIZE 1
#define CMD_BUFFER_SIZE 64
#define MAX_ARGS 9

/**
 * @def ASCII_LF
 * ASCII code of the lign feed
 */
#define ASCII_LF 0x0A
/**
 * @def ASCII_CR
 * ASCII code of the carriage return
 */
#define ASCII_CR 0x0D
/**
 * @def ASCII_DEL
 * ASCII code of delete
 */
#define ASCII_DEL 0x7F

typedef int (* serial_transmit_t)(const uint8_t *pData, uint16_t size, uint32_t timeout);
typedef int (* serial_receive_t)(uint8_t *pData, uint16_t size, uint32_t timeout);

/**
 * @brief Structure to handle the UART abstraction layer
 *
 * @warning Need to be initialized in the main with the corresponding functions
 */
typedef struct h_serial_struct {
	serial_transmit_t 	transmit;
	serial_receive_t 	receive;
}h_serial_t;


/**
 * @brief Shell structure
 */
typedef struct h_shell_struct {
	h_serial_t serial;

	uint32_t uartRxReceived;						/*!< UART receive flag*/
	uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
	uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];
}h_shell_t;

