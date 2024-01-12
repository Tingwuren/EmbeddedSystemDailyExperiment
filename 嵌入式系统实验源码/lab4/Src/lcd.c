#include "lcd.h"
#include "font.h"
#include "stdlib.h"

#define MAX_HZ_POSX 480
#define MAX_HZ_POSY 227 

#define LCD_RAM    *(__IO uint16_t *) (0x6C000002)    	//地址寄存器
#define LCD_REG    *(__IO uint16_t *) (0x6C000000)	 	//指令寄存器

#define rw_data_prepare()               write_cmd(34)

void write_cmd(unsigned short cmd);
unsigned short read_data(void);
unsigned short DeviceCode;


/*
 *	
 */
void write_cmd(unsigned short cmd)
{	
	LCD_REG = cmd;	
}

unsigned short read_data(void)
{
	unsigned short temp;	
	temp = LCD_RAM;
	temp = LCD_RAM;
	return temp;
}

void write_data(unsigned short data_code )
{	
	LCD_RAM = data_code;
}

void write_reg(unsigned char reg_addr,unsigned short reg_val)
{
	write_cmd(reg_addr);
	write_data(reg_val);
}

unsigned short read_reg(unsigned char reg_addr)
{
	unsigned short val=0;
	write_cmd(reg_addr);
	val = read_data();
	return (val);
}


void lcd_SetCursor(unsigned int x,unsigned int y)
{
	write_reg(0x004e,x);    /* 0-239 */
	write_reg(0x004f,y);    /* 0-319 */
}
/* 读取指定地址的GRAM */
static unsigned short lcd_read_gram(unsigned int x,unsigned int y)
{
	unsigned short temp;
	lcd_SetCursor(x,y);
	rw_data_prepare();
	/* dummy read */
	temp = read_data();
	temp = read_data();
	return temp;
}
//static void lcd_data_bus_test(void)
//{
//	unsigned short temp1;
//	unsigned short temp2;
//	/* wirte */
//	lcd_SetCursor(0,0);
//	rw_data_prepare();
//	write_data(0x5555);

//	lcd_SetCursor(1,0);
//	rw_data_prepare();
//	write_data(0xAAAA);

//	/* read */
//	lcd_SetCursor(0,0);
//	temp1 = lcd_read_gram(0,0);
//	temp2 = lcd_read_gram(1,0);

//	if( (temp1 == 0x5555) && (temp2 == 0xAAAA) )
//	{
//		//printf(" data bus test pass!\r\n");
//	}
//	else
//	{
//		//printf(" data bus test error: %04X %04X\r\n",temp1,temp2);
//	}
//}

void lcd_clear(unsigned short Color)
{
    unsigned int count;

	write_cmd(0x002a);	
	write_data(0);	    
	write_data(0);
	write_data(HDP>>8);	    
	write_data(HDP&0x00ff);
    write_cmd(0x002b);	
	write_data(0);	    
	write_data(0);
	write_data(VDP>>8);	    
	write_data(VDP&0x00ff);
	write_cmd(0x002c);	
	write_cmd(0x002c);


	for(count=0;count<130560;count++){
		
		write_data(Color);
	}
}

void lcd_init(void)
{

//	GPIO_SetBits(GPIOF,GPIO_Pin_9);
	HAL_Delay(50);
	DeviceCode = read_reg(0x0000);

  write_cmd(0x002b);	
	write_data(0);

	HAL_Delay(50); // delay 50 ms 
	write_cmd(0x00E2);					//PLL multiplier, set PLL clock to 120M
	write_data(0x001D);					//N=0x36 for 6.5M, 0x23 for 10M crystal
	write_data(0x0002);
	write_data(0x0004);
	
	write_cmd(0x00E0);					//PLL enable
	write_data(0x0001);
	HAL_Delay(10);
	write_cmd(0x00E0);
	write_data(0x0003);
	HAL_Delay(12);
	write_cmd(0x0001);  					//software reset
	HAL_Delay(10);
	write_cmd(0x00E6);					//PLL setting for PCLK, depends on resolution
//	LCD_WriteRAM(0x0001);
//	LCD_WriteRAM(0x0033);
//	LCD_WriteRAM(0x0032);
	write_data(0x0000);
	write_data(0x00D9);
	write_data(0x0016);

	write_cmd(0x00B0);					//LCD SPECIFICATION
	write_data(0x0020);
	write_data(0x0000);
	write_data((HDP>>8)&0X00FF);			//Set HDP
	write_data(HDP&0X00FF);
    write_data((VDP>>8)&0X00FF);			//Set VDP
	write_data(VDP&0X00FF);
    write_data(0x0000);

	write_cmd(0x00B4);					//HSYNC
	write_data((HT>>8)&0X00FF); 			//Set HT
	write_data(HT&0X00FF);
	write_data((HPS>>8)&0X00FF);			//Set HPS
	write_data(HPS&0X00FF);
	write_data(HPW);						//Set HPW
	write_data((LPS>>8)&0X00FF); 			//Set HPS
	write_data(LPS&0X00FF);
	write_data(0x0000);

	write_cmd(0x00B6);					//VSYNC
	write_data((VT>>8)&0X00FF);   		//Set VT
	write_data(VT&0X00FF);
	write_data((VPS>>8)&0X00FF); 			//Set VPS
	write_data(VPS&0X00FF);
	write_data(VPW);						//Set VPW
	write_data((FPS>>8)&0X00FF);			//Set FPS
	write_data(FPS&0X00FF);

	//=============================================

	//=============================================
	write_cmd(0x00BA);
	write_data(0x0005);//0x000F);    //GPIO[3:0] out 1

	write_cmd(0x00B8);
	write_data(0x0007);    //GPIO3=input, GPIO[2:0]=output
	write_data(0x0001);    //GPIO0 normal

	write_cmd(0x0036); //rotation
	write_data(0x0000);

	HAL_Delay(50);

	write_cmd(0x00BE); //set PWM for B/L
	write_data(0x0006);
	write_data(0x0080);
	
	write_data(0x0001);
	write_data(0x00f0);
	write_data(0x0000);
	write_data(0x0000);

	write_cmd(0x00d0);//设置动态背光控制配置 
	write_data(0x000d);
   
	write_cmd(0x00F0); //pixel data interface
	write_data(0x0003); //03:16位   02:24位

	write_cmd(0x0029); //display on

	//lcd_data_bus_test();
	lcd_clear(Yellow);//初始化全屏填充颜色
}
/*********************************************************************************************************
** Functoin name:       LCD_SetCursor
** Descriptions:        设置坐标
** input paraments:     Xpos、Ypos 坐标
** output paraments:    无    
** Returned values:     无
*********************************************************************************************************/

void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	write_cmd(0x002A);	
	write_data(Xpos>>8);	    
	write_data(Xpos&0x00ff);
	write_data(479>>8);	    
	write_data(479&0x00ff);
    write_cmd(0x002b);	
	write_data(Ypos>>8);	    
	write_data(Ypos&0x00ff);
	write_data(271>>8);	    
	write_data(271&0x00ff);
}
/*********************************************************************************************************
** Functoin name:       LCD_DrawPoint
** Descriptions:        画点
** input paraments:     x.y:画点的坐标
						color：颜色
** output paraments:    无    
** Returned values:     无
*********************************************************************************************************/

void LCD_DrawPoint(uint16_t xsta, uint16_t ysta, uint16_t color)
{
	LCD_SetCursor(xsta, ysta);  		/*设置光标位置  */
	write_data_Prepare();          	/*开始写入GRAM */
	write_data(color); 

}
/**
	*名称：void WriteOneHz(uint16_t x0, uint16_t y0, uint8_t *pucMsk, uint16_t PenColor, uint16_t BackColor)
	*参数：x0,y0     起始坐标
		   *pucMsk   指向
		   PenColor	 字符颜色
		   BackColor 背景颜色
	*功能：
	*备注：此函数不能单独作为汉字字符显示											  
**/					
void WriteOneHz(uint16_t x0, uint16_t y0, uint8_t *pucMsk, uint16_t PenColor, uint16_t BackColor)
{
    uint16_t i,j;
    uint16_t mod[16];                                      /* 当前字模 16*16 */
    uint16_t *pusMsk;                                      /* 当前字库地址  */
    uint16_t y;

    pusMsk = (uint16_t *)pucMsk;


    for(i=0; i<16; i++)                                    /* 保存当前汉字点阵式字模       */
    {
        mod[i] = *pusMsk;                                /* 取得当前字模，半字对齐访问   */
        mod[i] = ((mod[i] & 0xff00) >> 8) | ((mod[i] & 0x00ff) << 8);/* 字模交换高低字节*/
		pusMsk = pusMsk+1;
    }
    y = y0;
    for(i=0; i<16; i++)                                    /* 16行   */
    {                                              
        for(j=0; j<16; j++)                                /* 16列   */
        {
		    if((mod[i] << j) & 0x8000)       /* 显示第i行 共16个点 */
            {
				LCD_DrawPoint(x0+j, y0+i, PenColor); 
            } 
			else 
			{
				LCD_DrawPoint(x0+j, y0+i, BackColor);
			}
        }
        y++;
    }
}
/**
	*名称: uint16_t findHzIndex(uint8_t *hz)
	*参数：hz
	*功能：索引汉字存储的内存地址
	*备注：
**/
uint16_t findHzIndex(uint8_t *hz)                            /* 在自定义汉字库在查找所要显示 */
                                                      /* 的汉字的位置 */
{
	uint16_t i=0;
	FNT_GB16 *ptGb16 = (FNT_GB16 *)GBHZ_16;		  /*ptGb16指向GBHZ_16*/
	while(ptGb16[i].Index[0] > 0x80)
	{
	    if ((*hz == ptGb16[i].Index[0]) && (*(hz+1) == ptGb16[i].Index[1])) /*汉字用两位来表示地址码*/
		{
	        return i;
	  }
	    i++;
	    if(i > (sizeof((FNT_GB16 *)GBHZ_16) / sizeof(FNT_GB16) - 1))  /* 搜索下标约束 */
	    {
		    break;
	    }
	}
	return 0;
}
/**
	*名称：void LCD_ShowHzString(uint16_t x0, uint16_t y0, uint8_t *pcStr, uint16_t PenColor, uint16_t BackColor)
	*参数：x0，y0    起始坐标
		   pcStr     指向
		   PenColor  字体颜色
		   BackColor 字体背景
	*功能：显示汉字字符串
	*备注：这个函数不能单独调用	       
**/
void LCD_ShowHzString(uint16_t x0, uint16_t y0, uint8_t *pcStr, uint16_t PenColor, uint16_t BackColor)
{
	uint16_t usIndex;
	uint8_t size = 16; 
	FNT_GB16 *ptGb16 = 0;    
    ptGb16 = (FNT_GB16 *)GBHZ_16; 

	if(x0>MAX_HZ_POSX){x0=0;y0+=size;}			         /*超出X轴字体最小单位，换行*/
    if(y0>MAX_HZ_POSY){y0=x0=0;lcd_clear(White);}	   /*超出Y轴字体最小单位，回到原点，并且清屏*/

	usIndex = findHzIndex(pcStr);
	WriteOneHz(x0, y0, (uint8_t *)&(ptGb16[usIndex].Msk[0]),  PenColor, BackColor); /* 显示字符 */
}
/**
	*名称：void LCD_ShowChar(uint8_t x, uint16_t y, uint8_t num, uint8_t size, uint16_t PenColor, uint16_t BackColor)
	*参数：x，y      起始坐标（x:0~234 y:0~308）
		   num       字符ASCII码值
		   size      字符大小，使用默认8*16
		   PenColor  字体颜色
		   BackColor 字体背景颜色
	*功能：
	*备注：注意屏幕大小
**/
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint16_t PenColor, uint16_t BackColor)
{       
#define MAX_CHAR_POSX (272-8)
#define MAX_CHAR_POSY (480-16) 
    uint8_t temp;
    uint8_t pos,t;
    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;		    
	num=num-' ';                         /*得到偏移后的值 */
	for(pos=0;pos<size;pos++)
	{
		if(size==12)
			temp=asc2_1206[num][pos];/*调用1206字体*/
		else 
			temp=asc2_1608[num][pos];		 /*调用1608字体	*/
		for(t=0;t<size/2;t++)
	    {                 
	        if(temp&0x01)			   /*从低位开始*/
			{
				LCD_DrawPoint(x+t, y+pos, PenColor);  /*画字体颜色 一个点*/
			}
			else 
				LCD_DrawPoint(x+t, y+pos, BackColor);	   /*画背景颜色 一个点*/     
	        temp>>=1; 
	    }
	}			
}
/**
	名称：void LCD_ShowCharString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t PenColor, uint16_t BackColor)
	参数：x，y      起始坐标
	      p         指向字符串起始地址
		  PenColor  字符颜色
		  BackColor 背景颜色
	功能：
	备注：用16字体，可以调节 此函数不能单独调用
**/
void LCD_ShowCharString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t PenColor, uint16_t BackColor)
{   
	uint8_t size = 16;     /*---字符大小默认16*8---*/
	 
    if(x>MAX_HZ_POSX){x=0;y+=size;}			         /*超出X轴字体最小单位，换行*/
    if(y>MAX_HZ_POSY){y=x=0;lcd_clear(White);}	 /*超出Y轴字体最小单位，回到原点，并且清屏*/
    LCD_ShowChar(x, y, *p, size, PenColor, BackColor);			   /*0表示非叠加方式*/
}

/**
	*名称：void LCD_ShowString(uint16_t x0, uint16_t y0, uint8_t *pcstr, uint16_t PenColor, uint16_t BackColor)
	*参数：x0 y0     起始坐标
		   pcstr     字符串指针
		   PenColor  字体颜色
		   BackColor 字体背景色
	*功能：调用字符和汉字显示函数，实现字符串显示
	*备注：	
**/
void LCD_ShowString(uint16_t x0, uint16_t y0, uint8_t *pcStr, uint16_t PenColor, uint16_t BackColor)
{
	while(*pcStr!='\0')
	{
	 	if(*pcStr>0x80) /*显示汉字*/
		{
			LCD_ShowHzString(x0, y0, pcStr, PenColor, BackColor);
			pcStr += 2;
			x0 += 16;	
		}
		else           /*显示字符*/
		{
			LCD_ShowCharString(x0, y0, pcStr, PenColor, BackColor);	
			pcStr +=1;
			x0+= 8;
		}
	
	}	

}
//
///*********************************************************************************************************
//** Functoin name:       write_data_Prepare
//** Descriptions:        写数据开始
//** input paraments:     无
//** output paraments:    无    
//** Returned values:     无
//*********************************************************************************************************/

void write_data_Prepare(void)
{
	write_cmd(0x002C);
}
/*********************************************************************************************************
** Functoin name:       LCD_WindowMax
** Descriptions:        设置窗口
** input paraments:     窗口的位置
** output paraments:    无    
** Returned values:     无
*********************************************************************************************************/
void LCD_WindowMax (unsigned int xsta,unsigned int ysta,unsigned int xend,unsigned int yend) 
{
	write_cmd(0X002A);
	write_data(xsta>>8);
	write_data(xsta&0X00FF);
	write_data(xend>>8);
	write_data(xend&0X00FF);
		
	write_cmd(0X002B);	
	write_data(ysta>>8);	
	write_data(ysta&0X00FF);
	write_data(yend>>8);	
	write_data(yend&0X00FF);			
}
/*********************************************************************************************************
** Functoin name:       LCD_Fill
** Descriptions:       填充窗口
** input paraments:     窗口的位置
						colour：颜色
** output paraments:    无    
** Returned values:     无
*********************************************************************************************************/
void LCD_Fill(uint8_t xsta, uint16_t ysta, uint8_t xend, uint16_t yend, uint16_t colour)
{                    
    uint32_t n;

	/*设置窗口	*/	
	LCD_WindowMax (xsta, ysta, xend, yend); 
	write_data_Prepare();         /*开始写入GRAM*/	 	   	   
	n=(uint32_t)(yend-ysta+1)*(xend-xsta+1);    
	while(n--){write_data(colour);} /*显示所填充的颜色*/	 
} 
/*********************************************************************************************************
** Functoin name:       LCD_DrawLine
** Descriptions:        指定坐表(两点)，画线
** input paraments:     xsta X起始坐标
	   	   				ysta Y起始坐标
						xend X终点坐标
						yend Y终点坐标
						colour：颜色
** output paraments:    无    
** Returned values:     无
*********************************************************************************************************/

void LCD_DrawLine(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint16_t x, y, t;	 
	if((xsta==xend)&&(ysta==yend))
		LCD_DrawPoint(xsta, ysta, color);
	else 
		if(abs(yend-ysta)>abs(xend-xsta))//斜率大于1  
		{
			if(ysta>yend) 
			{
				t=ysta;
				ysta=yend;
				yend=t; 
				t=xsta;
				xsta=xend;
				xend=t; 
			}
			for(y=ysta;y<yend;y++)            /*以y轴为基准*/ 
			{
				x=(uint32_t)(y-ysta)*(xend-xsta)/(yend-ysta)+xsta;
				LCD_DrawPoint(x, y, color);  
			}
		}
		else     /*斜率小于等于1 */
		{
			if(xsta>xend)
			{
				t=ysta;
				ysta=yend;
				yend=t;
				t=xsta;
				xsta=xend;
				xend=t;
			}   
			for(x=xsta;x<=xend;x++)  /*以x轴为基准*/ 
			{
				y =(uint32_t)(x-xsta)*(yend-ysta)/(xend-xsta)+ysta;
				LCD_DrawPoint(x, y, color); 
			}
		} 
}
/**
	*名称：void Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r)
	*参数：x0 中心点横坐标
	       y0 中心点纵坐标
		   r  半径
	*返回：无
	*功能：在指定位置画一个指定大小的圆
	*备注：加入颜色参数，是否填充等
**/
void Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             /*判断下个点位置的标志*/
	while(a<=b)
	{
		LCD_DrawPoint(x0-b, y0-a, color);             //3           
		LCD_DrawPoint(x0+b, y0-a, color);             //0           
		LCD_DrawPoint(x0-a, y0+b, color);             //1       
		LCD_DrawPoint(x0-b, y0-a, color);             //7           
		LCD_DrawPoint(x0-a, y0-b, color);             //2             
		LCD_DrawPoint(x0+b, y0+a, color);             //4               
		LCD_DrawPoint(x0+a, y0-b, color);             //5
		LCD_DrawPoint(x0+a, y0+b, color);             //6 
		LCD_DrawPoint(x0-b, y0+a, color);             
		a++;

		/*使用Bresenham算法画圆*/     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a, y0+b, color);
	}
}
/**
	*名称：void LCD_DrawRectangle(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend)
	*参数：xsta X起始坐标
	       ysta Y起始坐标
		   xend X结束坐标
		   yend Y结束坐标
	*返回：无
	*功能：在指定区域画矩形
	*备注：

**/
void LCD_DrawRectangle(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
	LCD_DrawLine(xsta, ysta, xend, ysta, color);
	LCD_DrawLine(xsta, ysta, xsta, yend, color);
	LCD_DrawLine(xsta, yend, xend, yend, color);
	LCD_DrawLine(xend, ysta, xend, yend, color);
} 
/****************************************************************************
* 名    称：void LCD_DrawPicture(uint16_t StartX,uint16_t StartY,uint16_t EndX,uint16_t EndY,uint16_t *pic)
* 功    能：在指定座标范围显示一副图片
* 入口参数： StartX     行起始座标
*           StartY     列起始座标
*           EndX       行结束座标
*           EndY       列结束座标
            pic        图片头指针
* 出口参数：无
* 说    明：图片取模格式为水平扫描，16位颜色模式
* 调用方法：LCD_DrawPicture(0,0,100,100,(uint16_t*)demo);
****************************************************************************/
void LCD_DrawPicture(uint16_t StartX,uint16_t StartY,uint16_t Xend,uint16_t Yend,uint8_t *pic)
{
	static	uint16_t i=0,j=0;
	uint16_t *bitmap = (uint16_t *)pic;
	for(j=0; j<Yend-StartY; j++)
	{
		for(i=0; i<Xend-StartX; i++) 
		LCD_DrawPoint(StartX+i, StartY+j, *bitmap++); 	
	}
}
