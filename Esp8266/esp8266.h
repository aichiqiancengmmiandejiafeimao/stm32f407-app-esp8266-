#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f4xx_hal.h"
#include <string.h>  // 添加string.h头文件

//定义wifi账号密码  
extern const char *ssid;
extern const char *password;
//定义华为云全局变量
extern const char *device_id;
extern const char *device_password;
extern const char *client_id;
extern const char *hostname;
extern const char *server_id;
// ESP8266Ĵ
extern UART_HandleTypeDef huart2;

// 初始化ESP8266模块,连接WIFI
void ESP8266_Init(void);
// 发送AT指令
void ESP8266_SendCommand(char* cmd);
//配置MQTT连接
void ESP8266_ConfigMQTT(void);
//连接MQTT服务器
void ESP8266_ConnectMQTT(void);
// 订阅主题
void ESP8266_SubscribeTopic(void);
// 发布测试消息
void ESP8266_PublishMessage(void);
//发布DHT11温度和湿度数据
void ESP8266_PublishDHT11Data(uint32_t temperature, uint32_t temperature_decimal, uint32_t humidity);

#endif