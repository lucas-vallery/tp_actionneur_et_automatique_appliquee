/*
 * serial_abstraction.c
 *
 *  Created on: Oct 31, 2022
 *      Author: lucas
 */

#include <hal_abstraction.h>

int serial_transmit(const uint8_t *pData, uint16_t size, uint32_t timeout){
	HAL_UART_Transmit(SERIAL_PORT, pData, size, timeout);

	return 0;
}

int serial_receive(uint8_t *pData, uint16_t size, uint32_t timeout){
	HAL_UART_Receive(SERIAL_PORT, pData, size, timeout);

	return 0;
}
