/*
*********************************************************************************************************
*
 
*
*********************************************************************************************************
*/

#ifndef _BSP_DHT11_H
#define _BSP_DHT11_H
#include "main.h"


typedef struct
{
	uint8_t Buf[5];
	uint8_t Temp;		/* Temperature �¶� ���϶� �������� */
	uint8_t Temp_decimal;		/* Temperature �¶� ���϶� �������� */
	uint8_t Hum;		/* Humidity ʪ�� �ٷֱ� */
}DHT11_T;

void bsp_InitDHT11(void);
uint8_t DHT11_ReadData(DHT11_T *_pDHT);

#endif



 
