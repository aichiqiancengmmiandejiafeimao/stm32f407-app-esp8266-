#include "main.h"
#include "bsp_dht11.h"
/*
    DHT11       
	  VCC   ------  3.3V
	  DQ    ------  PD11   
	  GND   ------  GND
*/


/* 设置DHT11_DATA引脚输出高/低电平 */

#define DHT11_OUT_L()		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);      //输出低电平
#define DHT11_OUT_H()		 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);			  //输出高电平

/* 判断DHT11_DATA引脚输入是否为低 */
#define DHT11_IS_LOW()	(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11) == GPIO_PIN_RESET) //读引脚
					

/*
*********************************************************************************************************
*	函 数 名: bsp_DelayUS
*	功能说明: us级延时，1us的延时与SYSCLCK有关系
           机制生成的代码中，SYSCLCK 为72MHZ
*	形    参: us数
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_DelayUS (uint16_t uS_val){

	uint16_t i,j;
	
	for(j=0; j<30*uS_val; j++)
	    ;
}

static uint8_t DHT11_ReadByte(void);

/*
*********************************************************************************************************
*	函 数 名: bsp_InitDHT11
*	功能说明: 配置STM32的GPIO,连接 DHT11
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitDHT11(void)
{
	DHT11_OUT_H();
}

/*
*********************************************************************************************************
*	函 数 名: DHT11_ReadData
*	功能说明: 复位DHT11。 拉低DHT11_DATA为低，持续最少480us，然后等待
*	形    参: 无
*	返 回 值: 0 失败； 1 表示成功
*********************************************************************************************************
*/
uint8_t DHT11_ReadData(DHT11_T *_pDTH)
{
	/*
		时序:
		1. MCU拉低QD持续时间大于 18ms, 然后释放QD = 1
	*/
	uint8_t i;
	uint8_t k;
	uint8_t sum;

	/* 1. 主机发送起始信号, DHT11_DATA引脚 拉低时间 ＞ 18ms */
	DHT11_OUT_L();		
	HAL_Delay(20);
	DHT11_OUT_H();		
	bsp_DelayUS(30);	/* 等待30us */

	/* 2. 等待DHT11_DATA引脚电平变低 ( 超时100us) */
	for (k = 0; k < 10; k++)
	{
		if (DHT11_IS_LOW())
		{
			break;
		}
		bsp_DelayUS(10);
	}
	if (k >= 10)
	{
		goto quit;		/* 超时无应答，失败 */
	}

	/* 3.等待DQ电平变高 (超时100us) */
	for (k = 0; k < 10; k++)
	{
		if (!DHT11_IS_LOW())
		{
			break;
		}
		bsp_DelayUS(10);
	}
	if (k >= 10)
	{
		goto quit;		/* 超时无应答，失败 */
	}

	/* 4.等待DQ电平变低 (超时100us) */
	for (k = 0; k < 10; k++)
	{
		if (DHT11_IS_LOW())
		{
			break;
		}
		bsp_DelayUS(10);
	}
	if (k >= 10)
	{
		goto quit;		/* 超时无应答，失败 */
	}

	/* 读40bit 数据 */
	for (i = 0; i < 5; i++)
	{
		_pDTH->Buf[i] = DHT11_ReadByte();
	}
	bsp_DelayUS(100);

	/* 计算校验和 */
	sum = _pDTH->Buf[0] + _pDTH->Buf[1] + _pDTH->Buf[2] + _pDTH->Buf[3];
	if (sum == _pDTH->Buf[4])
	{
		_pDTH->Temp = _pDTH->Buf[2];	/* 温度整数部分 */
		_pDTH->Temp_decimal =  _pDTH->Buf[3]; /* 温度小数部分 */
		_pDTH->Hum = _pDTH->Buf[0];	/* 湿度整数部分 */
		return 0;
	}
quit:
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: DHT11_ReadByte
*	功能说明: 向DHT11读取1字节数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static uint8_t DHT11_ReadByte(void)
{
	/*
		写数据时序
	*/
	uint8_t i,k;
	uint8_t read = 0;

	for (i = 0; i < 8; i++)
	{
		read <<= 1;
		/* 等待DQ电平变高 (超时100us) */
		for (k = 0; k < 10; k++)
		{
			if (!DHT11_IS_LOW())
			{
				break;
			}
			bsp_DelayUS(10);
		}
		if (k >= 10)
		{
			goto quit;		/* 超时无应答，失败 */
		}

		/* 等待DQ电平变低，统计DQ高电平时长 (超时100us) */
		for (k = 0; k < 10; k++)
		{
			if (DHT11_IS_LOW())
			{
				break;
			}
			bsp_DelayUS(10);
		}

		if (k > 3)		/* 高脉冲持续时间大于40us ，认为是1，否则是0 */
		{
			read++;
		}
	}

	return read;

quit:
	return 1;
}

 
