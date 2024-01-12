#ifndef __DS18B20_H
#define __DS18B20_H 

#include "stm32f4xx.h"
  

//DS18B20 驱动函数 
//IO方向设置


#define DS18B20_PIN                         GPIO_PIN_6
#define DS18B20_GPIO_PORT                   GPIOG

//#define DS18B20_GPIO_CLK                    RCC_AHB1Periph_GPIOG  

#define DS18B20_DATA_IN   HAL_GPIO_ReadPin(DS18B20_GPIO_PORT, DS18B20_PIN)
#define DS18B20_OUT_HIGH  HAL_GPIO_WritePin(DS18B20_GPIO_PORT, DS18B20_PIN, GPIO_PIN_SET)
#define DS18B20_OUT_LOW   HAL_GPIO_WritePin(DS18B20_GPIO_PORT, DS18B20_PIN, GPIO_PIN_RESET)



//   	
uint8_t DS18B20_Init(void);//初始化DS18B20
short DS18B20_Get_Temp(void);//获取温度
void DS18B20_Write_Byte(uint8_t dat);//写入一个字节
uint8_t DS18B20_Read_Byte(void);//读出一个字节
uint8_t DS18B20_Read_Bit(void);//读出一个位
uint8_t DS18B20_Check(void);//检测是否存在DS18B20
void DS18B20_Reset(void);       //复位DS18B20    
uint8_t DS18B20_Get_Serial(uint8_t *serial);
uint8_t GetCRC(uint8_t *str, uint32_t length);
void HAL_Delay(__IO uint32_t Delay);
void HAL_usDelay(__IO uint32_t Delay);

#endif
