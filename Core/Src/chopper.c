#include "chopper.h"

void chopper_start(void){
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin, SET);
	HAL_Delay(STARTING_TIME);
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin, RESET);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	//HAL_TIM_Base_Start(&htim1);
}

void chopper_stop(void){
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t) NO_SPEED);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint16_t) NO_SPEED);

	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);

}

int chopper_speed(uint16_t targetSpeed){
	uint16_t currentSpeed = __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1);
	uint16_t period = __HAL_TIM_GET_AUTORELOAD(&htim1);
	uint16_t accelStep = 1;

	if(targetSpeed > MAX_SPEED){
		targetSpeed = MAX_SPEED;
	}
	if(targetSpeed < NO_SPEED){
		targetSpeed = NO_SPEED;
	}

	if(targetSpeed < currentSpeed){
			while(currentSpeed - accelStep > targetSpeed){
				currentSpeed -= accelStep;
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t) currentSpeed);
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint16_t) period-currentSpeed);
				HAL_Delay(10);
			}
			return 0;
	}else {
		while(currentSpeed + accelStep < targetSpeed){
			currentSpeed += accelStep;
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t) currentSpeed);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint16_t) period-currentSpeed);
			HAL_Delay(10);
		}
		return 0;
	}
}
