
#include <stdio.h>
#include "stm32f10x.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
extern u8 Shift_Flag = 0 ;  //数字键盘    0/0x55:数字键盘无效/数字键盘有效
/***************************************************************
*函数名         : Delay_NS(u32 NS)
*描述           : 演示函数
*入口函数       : u32 NS
*出口函数       : 无
*返回           : 无
*****************************************************************/
void Delay_NS(u32 NS)
{
  u32 x,y;
  for(x=30;x>0;x--)
    for(y=NS;y>0;y--);
}

void Delay_5uS(void)
{
  u16 i,k;
  for(k=0;k<6;k++) 	  //5uS
    for(i=0;i<7; 
    
    i++);
}

void Delay_mS(u16 t)
{
  u16 j;
  while(t--) 	 //5uS
    for(j=0;j<7000;j++);
}


/***************************************************************
* 函数名         :void UART_Configuration()
*描述            :串口初始化
* 入口函数       : 无
* 出口函数       : 无
* 返回           : 无
*****************************************************************/
void UART1_Configuration(void)

{
  
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  /* 配置PA9为 USART1 Tx  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* 配置PA10为 USART1 Rx  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  
  USART_InitStructure.USART_BaudRate = 115200;
  
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  
  USART_InitStructure.USART_Parity = USART_Parity_No;
  
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  
  
  USART_Init(USART1 , &USART_InitStructure);
  
  USART_Cmd(USART1, ENABLE);
  
  
  // USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); /*接收中断使能*/
  
  
}


/***************************************************************
* 函数名         : USART1_SendCmd(void)
*描述            : 串口字符串发送
* 入口函数       : 无
* 出口函数       : 无
* 返回           : 无
*****************************************************************/
void USART1_SendCmd(u8* ptr)
{
  u16 i=0;
  for(i=0;i<200;i++)
  {
    if(*ptr==0)
    {
      return ;
    }
    USART_SendData(USART1, *ptr);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }
    ptr++;	
  }
}






/***************************************************************
* 函数名         : u8 KeyCsn(void)
*描述            : 串口字符串发送
* 入口函数       : 无
* 出口函数       : 无
* 返回           : 无
*****************************************************************/
u8 KeyScsn(void)
{
  
  KEY_Y1_LOW;   //第1线
  KEY_Y2_HIGHT;
  KEY_Y3_HIGHT;
  KEY_Y4_HIGHT;
  Delay_mS(5); 
  if(KEY_X1_Read==0)
  {
    Delay_mS(20);
    if(KEY_X1_Read==0)
    {
      
      return 0x14;   //q
      
    }
  }
  if(KEY_X2_Read==0)
  {
    Delay_mS(20);
    if(KEY_X2_Read==0)
    {
      return 0x1a;   //w
      
    }
  }
  
  if(KEY_X3_Read==0)
  {
    Delay_mS(20);
    if(KEY_X3_Read==0)
    {
      
      if(Shift_Flag==0)
        
        return 0x08;   //e
      else 
        
        return 0x59;   //1 
    }
  }
  
  if(KEY_X4_Read==0)
  {
    Delay_mS(20);
    if(KEY_X4_Read==0)
    {
      
      if(Shift_Flag==0)
        
        return 0x15;   //r
      else 
        return 0x5a;   //2 
    }
  }
  if(KEY_X5_Read==0)
  {
    Delay_mS(20);
    if(KEY_X5_Read==0)
    {
      
      
      if(Shift_Flag==0)
        return 0x17;   //t
      else 
        return 0x5b;   //3 
    }
  }
  
  if(KEY_X6_Read==0)
  {
    Delay_mS(20);
    if(KEY_X6_Read==0)
    {
      
      
      if(Shift_Flag==0)
        return 0x1c;   //y
      else 
        return 0x5c;   //4 
    }
  }
  
  if(KEY_X7_Read==0)
  {
    Delay_mS(20);
    if(KEY_X7_Read==0)
    {
      
      return 0x18;   //u
      
    }
  }
  
  if(KEY_X8_Read==0)
  {
    Delay_mS(20);
    if(KEY_X8_Read==0)
    {
      
      
      return 0x0c;   //i
      
    }
  }
  
  
  
  
  
  KEY_Y2_LOW;   //第2线
  KEY_Y1_HIGHT;
  KEY_Y3_HIGHT;
  KEY_Y4_HIGHT;
  Delay_mS(5); 
  
  
  if(KEY_X1_Read==0)
  {
    Delay_mS(20);
    if(KEY_X1_Read==0)
    {
      
      
      return 0x12;   //0
      
    }
  }
  if(KEY_X2_Read==0)
  {
    Delay_mS(20);
    if(KEY_X2_Read==0)
    {
      
      
      return 0x13;   //p
      
    }
  }
  
  if(KEY_X3_Read==0)
  {
    Delay_mS(20);
    if(KEY_X3_Read==0)
    {
      
      
      if(Shift_Flag==0)
        return 0x04;   //a
      else 
        return 0x5d;   //5 
    }
  }
  
  if(KEY_X4_Read==0)
  {
    Delay_mS(20);
    if(KEY_X4_Read==0)
    {
      
      
      if(Shift_Flag==0)
        return 0x16;   //s
      else 
        return 0x5e;   //6
    }
  }
  if(KEY_X5_Read==0)
  {
    Delay_mS(20);
    if(KEY_X5_Read==0)
    {
      
      if(Shift_Flag==0)
        return 0x07;   //d
      else 
        return 0x5f;   //7
    }
  }
  
  if(KEY_X6_Read==0)
  {
    Delay_mS(20);
    if(KEY_X6_Read==0)
    {
      
      
      if(Shift_Flag==0)
        return 0x09;   //f
      else 
        return 0x60;   //8 
    }
  }
  
  if(KEY_X7_Read==0)
  {
    Delay_mS(20);
    if(KEY_X7_Read==0)
    {
      
      return 0x0a;   //g
      
    }
  }
  
  if(KEY_X8_Read==0)
  {
    Delay_mS(20);
    if(KEY_X8_Read==0)
    {
      
      return 0x0b;   //h
      
    }
  }
  
  KEY_Y3_LOW;   //第2线
  KEY_Y1_HIGHT;
  KEY_Y2_HIGHT;
  KEY_Y4_HIGHT;
  
  KEY_Y3_LOW;   //第3线
  Delay_mS(5); 
  if(KEY_X1_Read==0)
  {
    Delay_mS(20);
    if(KEY_X1_Read==0)
    {
      
      return 0x0d;   //j
      
    }
  }
  if(KEY_X2_Read==0)
  {
    Delay_mS(20);
    if(KEY_X2_Read==0)
    {
      
      return 0x0e;   //k
      
    }
  }
  
  if(KEY_X3_Read==0)
  {
    Delay_mS(20);
    if(KEY_X3_Read==0)
    {
      
      
      if(Shift_Flag==0)
        return 0x0f;   //l
      else 
        return 0x26;   //9
    }
  }
  
  if(KEY_X4_Read==0)
  {
    Delay_mS(20);
    if(KEY_X4_Read==0)
    {
      
      if(Shift_Flag==0)
        
        return 0x1d;   //z
      else 
        return 0x27;   //0
    }
  }
  if(KEY_X5_Read==0)
  {
    Delay_mS(20);
    if(KEY_X5_Read==0)
    {
      
      return 0x1b;   //x
      
    }
  }
  
  if(KEY_X6_Read==0)
  {
    Delay_mS(20);
    if(KEY_X6_Read==0)
    {
      
      
      return 0x06;   //c
    }
  }
  
  if(KEY_X7_Read==0)
  {
    Delay_mS(20);
    if(KEY_X7_Read==0)
    {
      
      return 0x19;   //v
      
    }
  }
  
  if(KEY_X8_Read==0)
  {
    Delay_mS(20);
    if(KEY_X8_Read==0)
    {
      
      return 0x05;   //b
      
    }
  }
  KEY_Y3_HIGHT; 
  KEY_Y4_LOW;   //第2线
  KEY_Y1_HIGHT;
  KEY_Y2_HIGHT;
  KEY_Y3_HIGHT;
  Delay_mS(5); 
  KEY_Y4_LOW;   //第4线
  if(KEY_X1_Read==0)
  {
    Delay_mS(20);
    if(KEY_X1_Read==0)
    {
      
      return 0x2b;   //tab
      
    }
  }
  if(KEY_X2_Read==0)
  {
    Delay_mS(20);
    if(KEY_X2_Read==0)
    {
      return 0x11;   //n
      
    }
  }
  
  if(KEY_X3_Read==0)
  {
    Delay_mS(20);
    if(KEY_X3_Read==0)
    {
      
      return 0x10;   //m
      
    }
  }
  if(KEY_X4_Read==0)
  {
    Delay_mS(20);
    if(KEY_X4_Read==0)
    {
      
      return 0x38;   //  "/"
      
    }
  }
  if(KEY_X5_Read==0)
  {
    Delay_mS(20);
    if(KEY_X5_Read==0)
    {
      
      return 0x28;   //enter
      
    }
  }
  
  if(KEY_X6_Read==0)
  {
    Delay_mS(20);
    if(KEY_X6_Read==0)
    {
      
      return 255;   //
    }
  }
  if(KEY_X7_Read==0)
  {
    Delay_mS(20);
    if(KEY_X7_Read==0)
    {
      
      return 0x2a;   //back
      
    }
  }
  
  if(KEY_X8_Read==0)
  {
    Delay_mS(20);
    if(KEY_X8_Read==0)
    {
      
      return 0x4c;   //delete
      
    }
  }
  KEY_Y1_HIGHT; 
  KEY_Y2_HIGHT;
  KEY_Y3_HIGHT;
  KEY_Y4_HIGHT;
  Delay_mS(5); 
  
  return 0;
  
}