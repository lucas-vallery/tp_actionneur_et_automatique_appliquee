/**
 * @file 	chopper.h
 * @brief 	This file contains all the functions prototypes for
 * 			the chopper.c file
 *
 * This file contains all the functions to use Microchip's MC1L 3-PHASE LOW VOLTAGE POWER MODULE with a 2-phased DC motor
 *
 */

#pragma once
#include "main.h"
#include "tim.h"

/**
 * @def MAX_SPEED
 * Maximum speed allowed. Corresponds to alpha = 90%
 */
#define MAX_SPEED 	1575
/**
 * @def NO_SPEED
 * Correspond to alpha = 50%
 */
#define NO_SPEED 	875
/**
 * @def STARTING_TIME
 * Minimum pulse width needed on ISO_RESET to start the chopper. (1.2.4.1 in the datasheet)
 */
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
 * @brief Restart the chopper and play the starting sequence
 */
void chopper_restart(void);

/**
 * @brief Set the chopper speed
 * @warning A call to this function blocks the CPU due to the time needed to accelerate to the desired speed
 *
 * @param speed A speed of 850 corresponds to alpha = 50% (rotation speed of 0 rpm)
 */
int chopper_speed(uint16_t speed);

