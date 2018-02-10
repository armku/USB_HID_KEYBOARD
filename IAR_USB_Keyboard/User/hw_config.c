/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : hw_config.c
* Author             : MCD Application Team
* Version            : V3.0.1
* Date               : 04/27/2009
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "platform_config.h"
#include "usb_pwr.h"


ErrorStatus HSEStartUpStatus;
EXTI_InitTypeDef EXTI_InitStructure;


void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* PB0输出 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;  //LED灯
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50M时钟速度
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  //BEEN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50M时钟速度
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5| GPIO_Pin_6;  //KEYX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//开漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50M时钟速度
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;  //KEYX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//开漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50M时钟速度
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;  //KEYY
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50M时钟速度
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_7|GPIO_Pin_9;  //KEYY
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50M时钟速度
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  
  
  
  
}



//USB上拉线控,如果上拉生效,USB从设备就关闭
void USB_Cable_Config (FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
  }
  else
  {
    GPIO_SetBits(GPIOA, GPIO_Pin_8);
  }
}



void USB_Configurage(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //USB 用的IO
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //PA8
  //USB时钟
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
  
  
  /* C6-DEV控制口 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  USB_Cable_Config(DISABLE);
  USB_Cable_Config(ENABLE);
  
}
/***************************************************************
* 函数名         : void RCC_Configuration(void)
*描述            : 时钟初始化
* 入口函数       : 无
* 出口函数       : 无
* 返回           : 无
*****************************************************************/
void RCC_Configuration(void)
{
  /* 系统时钟复位 */
  RCC_DeInit();
  
  /* 开外部时钟 */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* 等待外部时钟起振*/
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  
  if(HSEStartUpStatus == SUCCESS)
  {
    
    RCC_HCLKConfig(RCC_SYSCLK_Div1);       
    
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 	  // 设置AHB时钟
    
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);	  //设置高速AHB时钟
    
    /* ADCCLK = PCLK2/4 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div4); 	  //设置低速AHB时钟
    FLASH_SetLatency(FLASH_Latency_2);	  //2周期延时
    
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);	   //设置PLL时钟及其倍频
    
    
    RCC_PLLCmd(ENABLE);
    
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    
    
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    
    
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);	     //使能ADC1时钟
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);
}
/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
  
  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
  initialize the PLL and update the SystemFrequency variable. */
  //SystemInit();
  
  /* enable the PWR clock */
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  //初始化GPIO部分，LED灯以及上下左右键部分
  GPIO_Configuration();
  //初始化USB管脚，上拉线控脚
  USB_Configurage();
  GPIO_ResetBits(GPIOA, GPIO_Pin_8);
  GPIO_ResetBits(GPIOA, GPIO_Pin_8);
  GPIO_ResetBits(GPIOA, GPIO_Pin_8);
  /* 上下左右键盘*/
  
#if 0
  //这个是唤醒脚暂时去掉
  /* Connect EXTI Line9 */
  GPIO_EXTILineConfig(GPIO_KEY_PORTSOURCE, GPIO_KEY_PINSOURCE);
  
  /* Configure EXTI Line9 to generate an interrupt on falling edge */
  EXTI_InitStructure.EXTI_Line = GPIO_KEY_EXTI_Line;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;                                                                                                                                                    1ucture.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  EXTI_ClearITPendingBit(EXTI_Line18);
  EXTI_InitStructure.EXTI_Line = EXTI_Line18; // USB resume from suspend mode
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  EXTI_ClearITPendingBit(GPIO_KEY_EXTI_Line);
#endif
}


/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;
  
  /* Clear EXTI Line18 pending bit */
  EXTI_ClearITPendingBit(GPIO_KEY_EXTI_Line);
  
  /* Request to enter STOP mode with regulator in low power mode */
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;
  
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  
  /* Enable PLL */
  RCC_PLLCmd(ENABLE);
  
  /* Wait till PLL is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {}
  
  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  /* Wait till PLL is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x08)
  {}
  
  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else
  {
    bDeviceState = ATTACHED;
  }
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : Configures the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* USBLP中断 */
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* USB唤醒*/
  NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#if 0
  //白菜屏蔽
  /* Enable the Key EXTI line Interrupt */
  //NVIC_InitStructure.NVIC_IRQChannel = EXTI_KEY_IRQChannel;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 // NVIC_Init(&NVIC_InitStructure);
#endif
}


/*******************************************************************************
* Function Name : JoyState.
* Description   : Decodes the Joystick direction.
* Input         : None.
* Output        : None.
* Return value  : The direction value.
*******************************************************************************/
uint8_t JoyState(void)
{
  return 1;
}

/*******************************************************************************
* Function Name : Joystick_Send.
* Description   : prepares buffer to be sent containing Joystick event infos.
* Input         : Keys: keys received from terminal.
* Output        : None.
* Return value  : None.
*******************************************************************************/
void Joystick_Send(uint8_t Keys)
{
  uint8_t Mouse_Buffer[10] = {0,0};
  //  int8_t X = 0, Y = 0;
  //
  //  switch (Keys)
  //  {
  //    case LEFT:
  //      X -= CURSOR_STEP;
  //      break;
  //    case RIGHT:
  //
  //      X += CURSOR_STEP;
  //      break;
  //    case UP:
  //      Y -= CURSOR_STEP;
  //      break;
  //    case DOWN:
  //      Y += CURSOR_STEP;
  //      break;
  //    default:
  //      return;
  //  }
  //
  //  /* prepare buffer to send */
  //  Mouse_Buffer[1] = X;
  //  Mouse_Buffer[2] = Y;
  Mouse_Buffer[0] = 0;
  Mouse_Buffer[1] = 0;
  Mouse_Buffer[2] = 0x1e;
  Mouse_Buffer[3] = 0x1f;
  Mouse_Buffer[4] = 0x20;
  Mouse_Buffer[5] = 0x21;
  Mouse_Buffer[6] = 0x28;
  Mouse_Buffer[7] = 0;
  Mouse_Buffer[8] = 0;
  Mouse_Buffer[9] = 0;
  /*copy mouse position info in ENDP1 Tx Packet Memory Area*/
  UserToPMABufferCopy(Mouse_Buffer, GetEPTxAddr(ENDP1), 8);
  /* enable endpoint for transmission */
  SetEPTxValid(ENDP1);
  Mouse_Buffer[0] = 0;
  Mouse_Buffer[1] = 0;
}

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
  
  Device_Serial0 = *(uint32_t*)(0x1FFFF7E8);
  Device_Serial1 = *(uint32_t*)(0x1FFFF7EC);
  Device_Serial2 = *(uint32_t*)(0x1FFFF7F0);
  
  if (Device_Serial0 != 0)
  {
    Joystick_StringSerial[2] = (uint8_t)(Device_Serial0 & 0x000000FF);
    Joystick_StringSerial[4] = (uint8_t)((Device_Serial0 & 0x0000FF00) >> 8);
    Joystick_StringSerial[6] = (uint8_t)((Device_Serial0 & 0x00FF0000) >> 16);
    Joystick_StringSerial[8] = (uint8_t)((Device_Serial0 & 0xFF000000) >> 24);
    
    Joystick_StringSerial[10] = (uint8_t)(Device_Serial1 & 0x000000FF);
    Joystick_StringSerial[12] = (uint8_t)((Device_Serial1 & 0x0000FF00) >> 8);
    Joystick_StringSerial[14] = (uint8_t)((Device_Serial1 & 0x00FF0000) >> 16);
    Joystick_StringSerial[16] = (uint8_t)((Device_Serial1 & 0xFF000000) >> 24);
    
    Joystick_StringSerial[18] = (uint8_t)(Device_Serial2 & 0x000000FF);
    Joystick_StringSerial[20] = (uint8_t)((Device_Serial2 & 0x0000FF00) >> 8);
    Joystick_StringSerial[22] = (uint8_t)((Device_Serial2 & 0x00FF0000) >> 16);
    Joystick_StringSerial[24] = (uint8_t)((Device_Serial2 & 0xFF000000) >> 24);
  }
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
