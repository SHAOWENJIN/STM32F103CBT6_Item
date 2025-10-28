#include "main.h"
#include "usart.h"
#include "uart.h"

uint8_t u_flag = 0;
uint8_t t_flag = 0;
uint8_t t1_flag = 0;
uint8_t matrix_flag = 0;
uint8_t sum = 4,uc[4];
float b = 0;
uint8_t rec_flag = 0;
uint8_t PC_flag = 0;
uint8_t stm_flag = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart2)
    {
				PC_flag = 1;
        if (RX_Data == 0xB1)
        {
					t_flag = 1;
					HAL_UART_Transmit(&huart1,&RX_Data,1, 0xff);
				}
				else if(RX_Data == 0xE1||RX_Data == 0xE2||RX_Data == 0xE3||RX_Data == 0xE4||RX_Data == 0xE5||RX_Data == 0xE6||RX_Data == 0xE7||RX_Data == 0xE8||RX_Data == 0xE9||RX_Data == 0xE0||RX_Data == 0xC1)
				{
						if(RX_Data != 0xC1)
						{
							matrix_flag = 1;
						}
						else if(RX_Data == 0xC1)
						{
							rec_flag = 1;
						}
						HAL_UART_Transmit(&huart1,&RX_Data,1, 0xff);
				}
        HAL_UART_Receive_IT(&huart2, &RX_Data, 1); 
    }
		else if(huart == &huart1)
		{
			stm_flag = 1;
			u_flag = RX_Data1;
			if(t_flag == 1) 
			{
				sum -= 1;
				uc[sum] = RX_Data1;
				if(sum == 0)
				{
					memcpy(&b,uc,4);
					sum = 4;
					t1_flag = 1;
				}
			}
			HAL_UART_Receive_IT(&huart1,&RX_Data1,1);
			//printf("%x",RX_Data1);
		}
}