/*
 * serial_abstraction.h
 *
 *  Created on: Oct 31, 2022
 *      Author: lucas
 */

#pragma once
#include <stdio.h>
#include <usart.h>

#define USART_PORT &huart2

int serial_transmit(uint8_t *pData, uint16_t size, uint32_t timeout);

int serial_receive(uint8_t *pData, uint16_t size, uint32_t timeout);
