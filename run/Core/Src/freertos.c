/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MyRtos.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern uint8_t m_flag;
extern uint8_t rec_flag;
uint8_t PosData = 0;
uint16_t flag_matrix = 0;
uint16_t dat[10][8] = { {0x80ef, 0x20ef, 0x08ef, 0x0200, 0x40ef, 0x10ef, 0x04ef, 0x01ef},
												{0x80df, 0x20df, 0x0803, 0x02db, 0x40db, 0x109a, 0x04ba, 0x0178},
												{0x80f7, 0x20d7, 0x08d7, 0x02d3, 0x40db, 0x109b, 0x04bd, 0x013c},
												{0x80ef, 0x20ef, 0x08e9, 0x0207, 0x40ef, 0x10ed, 0x04ed, 0x01e1},
												{0x80ef, 0x20f7, 0x0800, 0x02ff, 0x40d3, 0x10d9, 0x04bd, 0x017d},
												{0x8001, 0x20ef, 0x08ef, 0x0283, 0x40db, 0x10db, 0x04db, 0x0100},
												{0x80ff, 0x2001, 0x0855, 0x0255, 0x4051, 0x103d, 0x0401, 0x017d},
												{0x80ff, 0x2000, 0x08ff, 0x0281, 0x40ff, 0x10ff, 0x0400, 0x01ff},
												{0x80ff, 0x2081, 0x08ff, 0x02ff, 0x40ff, 0x10ff, 0x0400, 0x01ff},
												{0x80ff, 0x20ff, 0x08ff, 0x0200, 0x40ff, 0x10ff, 0x04ff, 0x01ff}  };
/* USER CODE END Variables */
osThreadId matrixHandle;
osThreadId tempHandle;
osSemaphoreId ADC_SEMAPHOREHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void SendToLedMatrix(uint16_t Data)
{    
	
		 //SCK
	   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
	   //RCK
	   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
	
	   //send PosData
		 for(int i=0;i<16;i++)
		{
			if(Data&0x8000)
			{
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);
			}
		 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
		 Data<<=1;
		}
}
/* USER CODE END FunctionPrototypes */

void Mmatrix(void const * argument);
void adc_task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of ADC_SEMAPHORE */
  osSemaphoreDef(ADC_SEMAPHORE);
  ADC_SEMAPHOREHandle = osSemaphoreCreate(osSemaphore(ADC_SEMAPHORE), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of matrix */
  osThreadDef(matrix, Mmatrix, osPriorityNormal, 0, 128);
  matrixHandle = osThreadCreate(osThread(matrix), NULL);

  /* definition and creation of temp */
  osThreadDef(temp, adc_task, osPriorityIdle, 0, 128);
  tempHandle = osThreadCreate(osThread(temp), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Mmatrix */
/**
  * @brief  Function implementing the matrix thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Mmatrix */
void Mmatrix(void const * argument)
{
  /* USER CODE BEGIN Mmatrix */
  /* Infinite loop */
	uint32_t sum = 0;
//	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,500);
//	__HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_4, 500); 
  for(;;)
  {
		if(PosData > 9) PosData = 0;
		if (m_flag != 0)
		{
			HAL_UART_Transmit(&huart1,&m_flag,1, 0xff);
			m_flag = 0;
		}
		if(rec_flag == 1)
		{
			HAL_UART_Transmit(&huart1,&rec_flag,1, 0xff);
			rec_flag = 0;
		}
		for(int i=0;i<8;i++)
		{
			SendToLedMatrix(dat[PosData][i]);
			//RCK
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
			sum=sum+1;
		}
		if(sum >= 50000)
		{
			sum = 0;
			osDelay(1);
			if(flag_matrix == 0)PosData++;
		}
  }
  /* USER CODE END Mmatrix */
}

/* USER CODE BEGIN Header_adc_task */
/**
* @brief Function implementing the temp thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_adc_task */
__weak void adc_task(void const * argument)
{
  /* USER CODE BEGIN adc_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END adc_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

