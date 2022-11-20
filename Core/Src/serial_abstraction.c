/*
 * serial_abstraction.c
 *
 *  Created on: Oct 31, 2022
 *      Author: lucas
 */

#include "serial_abstraction.h"

int serial_transmit(uint8_t *pData, uint16_t size, uint32_t timeout){
	HAL_UART_Transmit(USART_PORT, pData, size, timeout);

	return 0;
}

int serial_receive(uint8_t *pData, uint16_t size, uint32_t timeout){
	HAL_UART_Receive(USART_PORT, pData, size, timeout);

	return 0;
}
