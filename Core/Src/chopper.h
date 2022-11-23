/*
 * chooper.h
 *
 *  Created on: Nov 22, 2022
 *      Author: lucas
 */

#pragma once
#include "main.h"
#include "tim.h"

#define MAX_SPEED 	1500
#define NO_SPEED 	875

/**
 * @brief Chopper's start sequence
 */
void chopper_start(void);

void chopper_stop(void);

int chopper_speed(uint16_t speed);
