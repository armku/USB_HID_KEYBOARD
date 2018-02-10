/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : hw_config.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "usb_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define DOWN            0x08//1
#define LEFT            0x01//2
#define RIGHT           0x04//3
#define UP              0x02//4
#define CURSOR_STEP     6



 #define KEY_X1_GPIO                        GPIOA  
 #define KEY_X1_GPIO_PIN                  GPIO_Pin_6

 #define KEY_X2_GPIO                        GPIOA  
 #define KEY_X2_GPIO_PIN                  GPIO_Pin_5

 #define KEY_X3_GPIO                        GPIOA  
 #define KEY_X3_GPIO_PIN                  GPIO_Pin_4

 #define KEY_X4_GPIO                        GPIOA  
 #define KEY_X4_GPIO_PIN                  GPIO_Pin_3

 #define KEY_X5_GPIO                        GPIOA  
 #define KEY_X5_GPIO_PIN                  GPIO_Pin_2

 #define KEY_X6_GPIO                        GPIOA  
 #define KEY_X6_GPIO_PIN                  GPIO_Pin_1

 #define KEY_X7_GPIO                        GPIOA  
 #define KEY_X7_GPIO_PIN                  GPIO_Pin_0

 #define KEY_X8_GPIO                        GPIOB 
 #define KEY_X8_GPIO_PIN                  GPIO_Pin_6


 #define KEY_Y1_GPIO                        GPIOB  
 #define KEY_Y1_GPIO_PIN                  GPIO_Pin_0

 #define KEY_Y2_GPIO                        GPIOA  
 #define KEY_Y2_GPIO_PIN                  GPIO_Pin_7

 #define KEY_Y3_GPIO                        GPIOB  
 #define KEY_Y3_GPIO_PIN                  GPIO_Pin_7

 #define KEY_Y4_GPIO                        GPIOB  
 #define KEY_Y4_GPIO_PIN                  GPIO_Pin_9




/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void GPIO_AINConfig(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void Joystick_Send(uint8_t Keys);
uint8_t JoyState(void);
void Get_SerialNum(void);
void RCC_Configuration(void);
void Delay_NS(u32 NS);
void Delay_5uS(void);
void Delay_mS(u16 t);
 u8 KeyScsn(void);
void IWDG_Configuration(void);

#define LED2_ON()	GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define LED2_OFF()	GPIO_SetBits(GPIOB, GPIO_Pin_13)

#define LED3_ON()	GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define LED3_OFF()	GPIO_SetBits(GPIOB, GPIO_Pin_15)
 
#define BEEN_ON()	GPIO_ResetBits(GPIOC, GPIO_Pin_13)
#define BEEN_OFF()	GPIO_SetBits(GPIOC, GPIO_Pin_13)

#define KEY_X1_Read    GPIO_ReadInputDataBit(KEY_X1_GPIO,KEY_X1_GPIO_PIN)   
#define KEY_X2_Read    GPIO_ReadInputDataBit(KEY_X2_GPIO,KEY_X2_GPIO_PIN)  
#define KEY_X3_Read    GPIO_ReadInputDataBit(KEY_X3_GPIO,KEY_X3_GPIO_PIN)  
#define KEY_X4_Read    GPIO_ReadInputDataBit(KEY_X4_GPIO,KEY_X4_GPIO_PIN)  
#define KEY_X5_Read    GPIO_ReadInputDataBit(KEY_X5_GPIO,KEY_X5_GPIO_PIN)  
#define KEY_X6_Read    GPIO_ReadInputDataBit(KEY_X6_GPIO,KEY_X6_GPIO_PIN)  
#define KEY_X7_Read    GPIO_ReadInputDataBit(KEY_X7_GPIO,KEY_X7_GPIO_PIN)  
#define KEY_X8_Read    GPIO_ReadInputDataBit(KEY_X8_GPIO,KEY_X8_GPIO_PIN) 

#define KEY_Y1_LOW    GPIO_ResetBits(KEY_Y1_GPIO, KEY_Y1_GPIO_PIN)
#define KEY_Y1_HIGHT   GPIO_SetBits(KEY_Y1_GPIO, KEY_Y1_GPIO_PIN)

#define KEY_Y2_LOW    GPIO_ResetBits(KEY_Y2_GPIO, KEY_Y2_GPIO_PIN)
#define KEY_Y2_HIGHT   GPIO_SetBits(KEY_Y2_GPIO, KEY_Y2_GPIO_PIN)

#define KEY_Y3_LOW    GPIO_ResetBits(KEY_Y3_GPIO, KEY_Y3_GPIO_PIN)
#define KEY_Y3_HIGHT   GPIO_SetBits(KEY_Y3_GPIO, KEY_Y3_GPIO_PIN)

#define KEY_Y4_LOW    GPIO_ResetBits(KEY_Y4_GPIO, KEY_Y4_GPIO_PIN)
#define KEY_Y4_HIGHT   GPIO_SetBits(KEY_Y4_GPIO, KEY_Y4_GPIO_PIN)

#endif  /*__HW_CONFIG_H*/

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
