/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hal_abstraction.h"
#include "shell.h"
#include "chopper.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CURRENT_CONVERSION_FACTOR		(3.3/4096.0)*12.0
#define CURRENT_OFFSET					2.5*12.0
#define TICK_PER_ROTATION				4096.0
#define MS_TO_MIN						1000 * 60
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
h_shell_t shell;

const uint8_t prompt[]="user@Nucleo-STM32G431>>";
uint8_t started[]=
		"\r\n*-----------------------------*"
		"\r\n| Welcome on Nucleo-STM32G474 |"
		"\r\n*-----------------------------*"
		"\r\n";
const uint8_t newline[]="\r\n";
const uint8_t cmdNotFound[]="Command not found\r\n";
const uint8_t startmsg[] = "Power ON\r\n";
const uint8_t stopmsg[] = "Power OFF\r\n";
const uint8_t restartmsg[] = "Restarting...\r\nPower ON\r\n";

const uint8_t help[6][32]=
{
		"set <pin> <state>\r\n",
		"get <value name>\r\n",
		"start\r\n",
		"stop\r\n",
		"pinout\r\n",
		"restart\r\n"
};

const uint8_t pinoutmsg[7][64]=
{
		"Pinout\r\n",
		" ------------------\r\n"
		"| PA8  | TIM1_CH1  |\r\n",
		"| PA9  | TIM1_CH2  |\r\n",
		"| PA11 | TIM1_CH1N |\r\n",
		"| PA12 | TIM1_CH2N |\r\n",
		" ------------------\r\n"
};
uint32_t 	rawCurrent[1];
float 	current;

uint32_t currentCNT, previousCNT;
uint32_t diff;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	shell.serial.transmit = serial_transmit;
	shell.serial.receive = serial_receive;


	char	 	cmdBuffer[CMD_BUFFER_SIZE];
	int 		idx_cmd;
	char* 		argv[MAX_ARGS];
	int		 	argc = 0;
	char*		token;
	int 		newCmdReady = 0;
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_TIM1_Init();
	MX_USART2_UART_Init();
	MX_ADC1_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
	/* USER CODE BEGIN 2 */
	/*
	 * MX_GPIO_Init();
  	  MX_DMA_Init();
  	  MX_TIM1_Init();
  	  MX_USART2_UART_Init();
  	  MX_ADC1_Init();
  	  MX_TIM3_Init();
  	  MX_TIM4_Init();
	 */
	memset(argv,(int) NULL,MAX_ARGS*sizeof(char*));
	memset(cmdBuffer,(int) NULL,CMD_BUFFER_SIZE*sizeof(char));
	memset(shell.uartRxBuffer,(int) NULL,UART_RX_BUFFER_SIZE*sizeof(char));
	memset(shell.uartTxBuffer,(int) NULL,UART_TX_BUFFER_SIZE*sizeof(char));

	HAL_UART_Receive_IT(&huart2, shell.uartRxBuffer, UART_RX_BUFFER_SIZE);
	HAL_Delay(10);
	HAL_UART_Transmit(&huart2, started, sizeof(started), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&rawCurrent, 1);

	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{	// uartRxReceived is set to 1 when a new character is received on uart 1
		if(shell.uartRxReceived){
			switch(shell.uartRxBuffer[0]){
			// Nouvelle ligne, instruction à traiter
			case ASCII_CR:
				shell.serial.transmit((uint8_t*) newline, sizeof(newline), HAL_MAX_DELAY);
				cmdBuffer[idx_cmd] = '\0';
				argc = 0;
				token = strtok(cmdBuffer, " ");
				while(token!=NULL){
					argv[argc++] = token;
					token = strtok(NULL, " ");
				}

				idx_cmd = 0;
				newCmdReady = 1;
				break;
				// Suppression du dernier caractère
			case ASCII_DEL:
				cmdBuffer[idx_cmd--] = '\0';
				shell.serial.transmit(shell.uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
				break;
				// Nouveau caractère
			default:
				cmdBuffer[idx_cmd++] = shell.uartRxBuffer[0];
				shell.serial.transmit(shell.uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
			}
			shell.uartRxReceived = 0;
		}

		if(newCmdReady){
			if(strcmp(argv[0],"set")==0){
				if(strcmp(argv[1],"PA5")==0){
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, atoi(argv[2]));
					sprintf((char*) &(shell.uartTxBuffer),"Switch on/off led : %d\r\n",atoi(argv[2]));
					shell.serial.transmit(shell.uartTxBuffer, 32, HAL_MAX_DELAY);
				}
				else{
					shell.serial.transmit((uint8_t*)cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
				}
			}
			else if(strcmp(argv[0],"get")==0)
			{
				if(strcmp(argv[1],"current")==0){
					current = (float)rawCurrent[0]*CURRENT_CONVERSION_FACTOR - CURRENT_OFFSET;
					//current+=0.36;

					sprintf((char*) &(shell.uartTxBuffer), "current : %.2f A \r\n", current);
					shell.serial.transmit(shell.uartTxBuffer, 32, HAL_MAX_DELAY);
				}else if(strcmp(argv[1],"speed")==0){
					float speed = diff/TICK_PER_ROTATION * MS_TO_MIN;
					sprintf((char*) &(shell.uartTxBuffer), "speed : %.0f rpm\r\n", speed);
					shell.serial.transmit(shell.uartTxBuffer, 32, HAL_MAX_DELAY);
				}else if(strcmp(argv[1], "diff") == 0){
					sprintf((char*) &(shell.uartTxBuffer), "diff : %ld\r\n", diff);
					shell.serial.transmit(shell.uartTxBuffer, 32, HAL_MAX_DELAY);
				}
			}
			else if(strcmp(argv[0],"start")==0) {
				chopper_start();

				shell.serial.transmit((uint8_t*)startmsg, sizeof(startmsg), HAL_MAX_DELAY);
			}
			else if(strcmp(argv[0],"stop")==0) {
				chopper_stop();

				shell.serial.transmit((uint8_t*)stopmsg, sizeof(stopmsg), HAL_MAX_DELAY);
			}
			else if(strcmp(argv[0],"restart")==0) {
				chopper_restart();
				shell.serial.transmit((uint8_t*)&restartmsg, sizeof(restartmsg), HAL_MAX_DELAY);
			}
			else if(strcmp(argv[0],"help")==0)
			{
				if(strcmp(argv[1], "set")==0){
					shell.serial.transmit((uint8_t*)&help[0], sizeof(help[0]), HAL_MAX_DELAY);
				}
				else if(strcmp(argv[1], "get")==0){
					shell.serial.transmit((uint8_t*)&help[1], sizeof(help[1]), HAL_MAX_DELAY);
				}
				else if(strcmp(argv[1], "start")==0){
					shell.serial.transmit((uint8_t*)&help[2], sizeof(help[2]), HAL_MAX_DELAY);
				}
				else if(strcmp(argv[1], "stop")==0){
					shell.serial.transmit((uint8_t*)&help[3], sizeof(help[3]), HAL_MAX_DELAY);
				}
				else if(strcmp(argv[1], "pinout")==0){

					shell.serial.transmit((uint8_t*)&help[4], sizeof(help[4]), HAL_MAX_DELAY);
				}
				else if(strcmp(argv[1], "restart")==0){
					shell.serial.transmit((uint8_t*)&help[5], sizeof(help[5]), HAL_MAX_DELAY);
				}
				else{
					shell.serial.transmit((uint8_t*)&help, sizeof(help), HAL_MAX_DELAY);
				}
			}
			else if(strcmp(argv[0],"pinout")==0) {
				shell.serial.transmit((uint8_t*)&pinoutmsg, sizeof(pinoutmsg), HAL_MAX_DELAY);
			}
			else if(strcmp(argv[0],"speed")==0) {
				if(argv[1] != NULL){
					uint16_t speed;
					sscanf(argv[1], "%hd", &speed);
					chopper_speed(speed);
				}
			}
			else{
				shell.serial.transmit((uint8_t*)cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
			}
			shell.serial.transmit((uint8_t*)prompt, sizeof(prompt), HAL_MAX_DELAY);
			newCmdReady = 0;
		}
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
	RCC_OscInitStruct.PLL.PLLN = 85;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback (UART_HandleTypeDef * huart){
	shell.uartRxReceived = 1;
	HAL_UART_Receive_IT(&huart2, shell.uartRxBuffer, UART_RX_BUFFER_SIZE);
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if(htim->Instance == TIM4){
		previousCNT = currentCNT;
		currentCNT = TIM3->CNT;
		diff = -(currentCNT-previousCNT);
	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
