/**
 * @file 	chopper.h
 * @brief 	This file contains all the functions prototypes for
 * 			the chopper.c file
 */

#pragma once
#include "main.h"
#include "tim.h"

#define MAX_SPEED 	1500
#define NO_SPEED 	875
#define STARTING_TIME 1

/**
 * @brief Execute the chopper's starting sequence
 */
void chopper_start(void);

/**
 * @brief Turn off the PWMs
 */
void chopper_stop(void);

/**
 * @brief Set the chopper speed
 *
 * @param speed
 */
int chopper_speed(uint16_t speed);
