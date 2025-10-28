#ifndef _MYRTOS_H_
#define _MYRTOS_H_

#include "gpio.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include "MyRtos.h"
extern osSemaphoreId ADC_SEMAPHOREHandle;

extern uint8_t RX_Data;

extern uint8_t PosData;
extern uint16_t flag_matrix;
#endif