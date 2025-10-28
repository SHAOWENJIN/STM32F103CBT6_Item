#include "main.h"
#include "math.h"
#include "cmsis_os.h"
#include "adc.h"
#include "MyRtos.h"
#include "usart.h"
#include "stdio.h"


void float2tu8arry(uint8_t *arr,float *fff)
{
	uint8_t farray[4];
	*(float*)farray = *fff;
	arr[3] = farray[0];
	arr[2] = farray[1];
	arr[1] = farray[2];
	arr[0] = farray[3];
}

uint16_t g_adc_data_IT; 
uint16_t flag1 = 0;
float g_temprature;     
uint8_t m_flag = 0;
uint8_t RX_Data1 = 0;
uint16_t tem_flag = 0;
uint8_t rec_flag = 0;
void adc1_init()  
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_IT(&hadc1);
}

float cal_tem(uint16_t data)
{
    float Rx = (float)(data)/(4096-data);               
    return 1/(log(Rx/10)/3950+1/(273.15+25.0))-273.15;	
    
}

void fresh_tem()
{
    uint16_t adc_data_raw;
    adc_data_raw = g_adc_data_IT;               
    g_temprature = cal_tem(adc_data_raw);
		if(tem_flag != 0)
			{
				uint8_t arr[4];
				float2tu8arry(arr,&g_temprature);
				HAL_UART_Transmit(&huart1,arr,sizeof(arr), 0xff);
				//printf("the tem is %f",g_temprature);
				tem_flag = 0;
			}
    HAL_ADC_Start_IT(&hadc1);
}

void adc_task(void const * argument)
{
  /* USER CODE BEGIN adc_task */
  /* Infinite loop */
  for(;;)
  {
      adc1_init();
     if (xSemaphoreTake(ADC_SEMAPHOREHandle,100))
     {
         fresh_tem();
     }         
    osDelay(1);
  }
  /* USER CODE END adc_task */
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    BaseType_t tHigherPriorityTaskWoken;
    
    g_adc_data_IT = HAL_ADC_GetValue(hadc);
    xSemaphoreGiveFromISR(ADC_SEMAPHOREHandle,&tHigherPriorityTaskWoken);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
		
        if (RX_Data == 0xB1)
        {
					 tem_flag = 1;
           //printf("The temprature is %f\n",g_temprature);
				}
				else if(RX_Data == 0xE1||RX_Data == 0xE2||RX_Data == 0xE3||RX_Data == 0xE4||RX_Data == 0xE5||RX_Data == 0xE6||RX_Data == 0xE7||RX_Data == 0xE8||RX_Data == 0xE9||RX_Data == 0xE0)
				{
					flag_matrix = 1;
					PosData = 10-RX_Data&0x0f;
					if(PosData == 10) PosData = 0;
					m_flag = RX_Data;
					
				}
				else if(RX_Data == 0xC1)
				{
					rec_flag = 1;
					flag_matrix = 0;
				}
        HAL_UART_Receive_IT(&huart1, &RX_Data, 1); 
    }
}
