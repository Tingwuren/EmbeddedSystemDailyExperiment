#include "stm32f4xx_hal.h"
#include "ds18b20.h"
#include <stdio.h>

GPIO_InitTypeDef GPIO_InitStruct;

//CRC = X8 + X5 + X4 + 1
const uint8_t Crc8Table [256]={
0,  94, 188,  226,  97,  63,  221,  131,  194,  156,  126,  32,  163,  253,  31,  65,
157,  195,  33,  127,  252,  162,  64,  30,  95,  1,  227,  189,  62,  96,  130,  220,
35,  125,  159,  193,  66,  28,  254,  160,  225,  191,  93,  3,  128,  222,  60,  98,
190,  224,  2,  92,  223,  129,  99,  61,  124,  34,  192,  158,  29,  67,  161,  255,
70,  24,  250,  164,  39,  121,  155,  197,  132,  218,  56,  102,  229,  187,  89,  7,
219,  133, 103,  57,  186,  228,  6,  88,  25,  71,  165,  251,  120,  38,  196,  154,
101,  59, 217,  135,  4,  90,  184,  230,  167,  249,  27,  69,  198,  152,  122,  36,
248,  166, 68,  26,  153,  199,  37,  123,  58,  100,  134,  216,  91,  5,  231,  185,
140,  210, 48,  110,  237,  179,  81,  15,  78,  16,  242,  172,  47,  113,  147,  205,
17,  79,  173,  243,  112,  46,  204,  146,  211,  141,  111,  49,  178,  236,  14,  80,
175,  241, 19,  77,  206,  144,  114,  44,  109,  51,  209,  143,  12,  82,  176,  238,
50,  108,  142,  208,  83,  13,  239,  177,  240,  174,  76,  18,  145,  207,  45,  115,
202,  148, 118,  40,  171,  245,  23,  73,  8,  86,  180,  234,  105,  55,  213, 139,
87,  9,  235,  181,  54,  104,  138,  212,  149,  203,  41,  119,  244,  170,  72,  22,
233,  183,  85,  11,  136,  214,  52,  106,  43,  117,  151,  201,  74,  20,  246,  168,
116,  42,  200,  150,  21,  75,  169,  247,  182,  232,  10,  84,  215,  137,  107,  53}; 

// ����DS18B20��IO��DQ Ϊ����
//����1:������
//����0:����    	 
void DS18B20_Set_Input(void)
{
	GPIO_InitStruct.Pin = DS18B20_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DS18B20_GPIO_PORT, &GPIO_InitStruct);
  /* Configure the GPIO pin */
}
// ����DS18B20��IO��DQ Ϊ���
//����1:������
//����0:����    	 
void DS18B20_Set_Output(void)
{
  /* Configure the GPIO pin */
  GPIO_InitStruct.Pin = DS18B20_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP ;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
  HAL_GPIO_Init(DS18B20_GPIO_PORT, &GPIO_InitStruct);
}
//��λDS18B20  ��λ���壨���Ϊ480uS�ĵ͵�ƽ�źţ�
void DS18B20_Reset(void)	   
{        
		DS18B20_Set_Output(); //SET AS OUTPUT
		DS18B20_OUT_LOW;      //����DQ
		HAL_usDelay(650);        //����650us
		DS18B20_OUT_HIGH;
		HAL_usDelay(10);       // 10US
}

//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
    
	DS18B20_Set_Input();    // �����������ͷŴ��߲�������շ�ʽ��Rx����
	// DSl820 �ȴ�15.60uS ���ҽ��ŷ��ʹ������壨60-240uS�ĵ͵�ƽ�źš���
    while ((DS18B20_DATA_IN == 1) && (retry<60))
	{
		retry++;
		HAL_usDelay(2);
	};	 
    
	if(retry>=60)
        return 1;
    
	retry=0;
    while ((DS18B20_DATA_IN == 0) && (retry<245))
	{
		retry++;
		HAL_usDelay(2);
	};
	if(retry>=245)
        return 1;    
    
	return 0;
}
//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
uint8_t DS18B20_Init(void)
{
  /* Configure the GPIO pin */
  DS18B20_Set_Output();
  DS18B20_Reset();
  return DS18B20_Check();
}
//��DS18B20��ȡһ��λ
//����ֵ��1/0
uint8_t DS18B20_Read_Bit(void) 			 // read one bit
{
    uint8_t data;
    
	DS18B20_Set_Output();   //SET AS OUTPUT
    DS18B20_OUT_LOW; 
	HAL_usDelay(3);
    DS18B20_OUT_HIGH; 
	DS18B20_Set_Input();    //SET AS INPUT
	HAL_usDelay(6);
	if(DS18B20_DATA_IN)
    {
        data=1;
    }
    else
    {
        data=0;  
    }
    HAL_usDelay(50); 
    
    return data;
}

//��DS18B20��ȡһ���ֽ�
//����ֵ������������
uint8_t DS18B20_Read_Byte(void)    // read one byte
{        
    uint8_t i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=dat>>1;
        dat+=(j<<7);
    }	

    return dat;
}

//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
/******************************* дDS18B20���� ******************************************/
void DS18B20_Write_Byte(uint8_t dat)     
{             
    uint8_t j;
    uint8_t testb;
	DS18B20_Set_Output();//SET AS OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;

        DS18B20_OUT_LOW;// Write 1
        HAL_usDelay(2);  
        if (testb) 
        {
            DS18B20_OUT_HIGH;
        }
        HAL_usDelay(55);
        DS18B20_OUT_HIGH;
        HAL_usDelay(5);                          
    }
}

//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS18B20_Get_Temp(void)
{
    uint8_t i, temp;
    uint8_t TL,TH;
	short tem;
    uint8_t str[9];
    
    //��ʼ�¶�ת��
    DS18B20_Reset();	   
	DS18B20_Check();
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert

    //��ʼ��ȡ�¶�
    DS18B20_Reset();
	DS18B20_Check();
 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert	
    //TL=DS18B20_Read_Byte(); // LSB   
    //TH=DS18B20_Read_Byte(); // MSB  

    for (i=0;i<9;i++) 
	{    
	    str[i] = DS18B20_Read_Byte();
    }
    if(GetCRC(str, 9) == 0)
        printf(" CRC OK  ");
    else
        printf(" CRC ERR ");

//    for (i=0;i<9;i++) 
//	{    
//	    printf(" %02X", str[i]);
//    }  
//    
    TL = str[0]; // LSB   
    TH = str[1]; // MSB      
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//�¶�Ϊ��  
    }
    else
    {
        temp=1;//�¶�Ϊ��         
    }
    tem=TH<<8 | TL; //��ò�������λ��11λ�¶�ֵ

    //ת�� *0.625
    tem = tem*10;
    tem = tem>>4;

	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
} 

/************************************************************
*Function:CRCУ��
*parameter:
*Return:
*Modify:
*************************************************************/
uint8_t GetCRC(uint8_t *str, uint32_t length)
{ 
    uint8_t crc_data=0;
   	uint32_t i;
  	for(i=0;i<length;i++)  //���У��
    { 
    	crc_data = Crc8Table[crc_data^str[i]];
    }
    return (crc_data);
}

uint8_t DS18B20_Get_Serial(uint8_t *serial)
{
    uint8_t i;
    uint8_t crcdata;    

    DS18B20_Reset();
	DS18B20_Check();

    DS18B20_Write_Byte(0x33);// read rom

    for(i=0;i<8;i++)
    {
       *(serial + i) = DS18B20_Read_Byte();
    }
    
    crcdata = GetCRC(serial,8); // Ϊ0��У����ȷ

    return (crcdata);
}
 





/**
  * @brief 
  * @note 
  * @note 
  * @param Delay: specifies the delay time length, in us.
  * @retval None
  */
 void HAL_usDelay(__IO uint32_t Delay)
{
	uint8_t  fac_us=168;//��������ʱ��Ƶ����ͬ
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				//LOAD��ֵ	    	 
	ticks=Delay*fac_us; 						//��Ҫ�Ľ����� 
	told=SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
}
void HAL_Delay(__IO uint32_t Delay)
{
	uint32_t i;
	for(i=0;i<Delay;i++) HAL_usDelay(1000);
}


