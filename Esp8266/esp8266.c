#include "esp8266.h"

#include "string.h"


// 定义wifi账号密码  
const char *ssid = "xiaohuihui";
const char *password = "123456789";
// 定义华为云全局变量
const char *device_id = "67d41413aa00d1577019d9be_ceshi";
const char *device_password = "e202bece3e80cadcfac566174b82069f9131b6d1af0aabede72450cffad32218";
const char *client_id = "67d41413aa00d1577019d9be_ceshi_0_0_2025031812";
const char *hostname = "7205da2530.st1.iotda-device.cn-north-4.myhuaweicloud.com";
const char *server_id = "stm32";

//初始化AT指令
#define AT_TEST "AT\r\n"
#define AT_CWMODE "AT+CWMODE=3\r\n"
#define AT_CWJAP "AT+CWJAP=\"xiaohuihui\",\"123456789\"\r\n"

// 发送AT指令
void ESP8266_SendCommand(char* cmd)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 1000);
    HAL_Delay(1000); // 等待模拟响应
}

// 初始化ESP8266模块，连接WIFI
void ESP8266_Init(void)
{
    // AT指令测试
   ESP8266_SendCommand(AT_TEST);
   OLED_ShowString(0, 0, (uint8_t *)"AT_TEST", 8);
   HAL_Delay(1000);
    // 设置WiFi模式为Station+AP模式 
   ESP8266_SendCommand(AT_CWMODE);
   OLED_ShowString(0, 2, (uint8_t *)"AT_CWMODE", 8);
   HAL_Delay(1000);
    // 连接到WiFi
    ESP8266_SendCommand(AT_CWJAP);
   OLED_ShowString(0, 4, (uint8_t *)"AT_CWJAP", 8);
   HAL_Delay(10000);
   OLED_Clear();

}

//配置MQTT连接
void ESP8266_ConfigMQTT(void)
{
    char cmd[256];  // 定义cmd变量
    sprintf(cmd, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", client_id, device_id, device_password);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 1000);
    OLED_ShowString(0, 0, (uint8_t *)"MQTT Config OK", 8);  // 显示MQTT配置成功
    HAL_Delay(1000);
}

//连接MQTT服务器
void ESP8266_ConnectMQTT(void)
{
    char cmd[128];  // 定义cmd变量
    sprintf(cmd, "AT+MQTTCONN=0,\"%s\",1883,1\r\n", hostname);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 1000);
    OLED_ShowString(0, 2, (uint8_t *)"MQTT Connect OK", 8);  // 显示MQTT连接成功
    HAL_Delay(1000);    
}

// 订阅主题
void ESP8266_SubscribeTopic(void)
{
    char cmd[256];  // 定义cmd变量
    sprintf(cmd, "AT+MQTTSUB=0,\"$oc/devices/%s/sys/properties/report\",1\r\n", device_id);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 1000);
    OLED_ShowString(0, 4, (uint8_t *)"MQTT Subscribe OK", 8);  // 显示MQTT订阅成功
    HAL_Delay(1000);
    OLED_Clear();
}

// 发布测试消息
void ESP8266_PublishMessage(void)
{
    char cmd[256];
    sprintf(cmd, "AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"stm32\\\"\\\,\\\"properties\\\":{\\\"temperature\\\":20}}]}\",0,0\r\n", device_id);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 1000);
    HAL_Delay(10);
    sprintf(cmd, "AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"stm32\\\"\\\,\\\"properties\\\":{\\\"humidity\\\":66}}]}\",0,0\r\n", device_id);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 1000);
    OLED_ShowString(0, 0, (uint8_t *)"Test one OK", 8);  // 显示MQTT发布成功
    HAL_Delay(5000);
    sprintf(cmd, "AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"stm32\\\"\\\,\\\"properties\\\":{\\\"temperature\\\":25}}]}\",0,0\r\n", device_id);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 1000);
    HAL_Delay(10);
    sprintf(cmd, "AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"stm32\\\"\\\,\\\"properties\\\":{\\\"humidity\\\":88}}]}\",0,0\r\n", device_id);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 1000);
    OLED_ShowString(0, 2, (uint8_t *)"Test two OK", 8);  // 显示MQTT发布成功
    HAL_Delay(1000);
    OLED_Clear();
}
//发布DHT11温度和湿度数据
void ESP8266_PublishDHT11Data(uint32_t temperature, uint32_t temperature_decimal, uint32_t humidity)
{
    char cmd[256];  // 定义cmd变量
    sprintf(cmd, "AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"stm32\\\"\\\,\\\"properties\\\":{\\\"temperature\\\":%d.%d}}]}\",0,0\r\n", device_id, temperature, temperature_decimal);
    ESP8266_SendCommand(cmd);  // 发送温度数据到华为云temperature
    HAL_Delay(10);
    sprintf(cmd, "AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"stm32\\\"\\\,\\\"properties\\\":{\\\"humidity\\\":%d}}]}\",0,0\r\n", device_id, humidity);
    ESP8266_SendCommand(cmd);  // 发送湿度数据到华为云humidity  
    HAL_Delay(2000);

}   
// 发送心跳
//void ESP8266_SendHeartbeat(char* uid) 
//{
//    char cmd[64];
//    sprintf(cmd, "cmd=0&msg=ping\r\n");
//    ESP8266_SendCommand(cmd);
//}

// 串口接收回调函数
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//    if (huart->Instance == USART2)
//    {
//        rx_buffer[rx_index++] = (char)huart->Instance->DR;
//        if (rx_buffer[rx_index - 1] == '\n' || rx_index >= sizeof(rx_buffer) - 1)
//        {
//            rx_buffer[rx_index] = '\0';
//            rx_index = 0;
//            // 接收完成处理
//            HAL_UART_Transmit(&huart2, (uint8_t *)"Received: ", 10, 10);
//            HAL_UART_Transmit(&huart2, (uint8_t *)rx_buffer, strlen(rx_buffer), 10);
//        }
//    }
//}
