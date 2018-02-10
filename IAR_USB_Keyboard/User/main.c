/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V3.0.1
* Date               : 04/27/2009
* Description        : Joystick Mouse demo main file
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
u8 USBsendBuffer[8] ={0,0,0,0,0,0,0,0};
extern u8 Shift_Flag ;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main.
* Description    : main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 Vale=0;//����ֵ
int main(void)
{
  
  u8 i=0;
  RCC_Configuration();
 // for(i=0;i<85;i++)
 // {
 //   Delay_mS(1000);
 // }
  IWDG_Configuration();//���Ź���λ
  
  Set_System(); 
  USB_Interrupts_Config();
  USB_Init();
  
  BEEN_ON();
   IWDG_ReloadCounter(); //ι��
  Delay_mS(250); 
  BEEN_OFF();
  LED3_OFF();
  // UART1_Configuration();     
  LED2_ON();
  Delay_mS(250); 
  KEY_Y1_HIGHT;
  KEY_Y2_HIGHT;
  KEY_Y3_HIGHT;
  KEY_Y4_HIGHT;
  
  
  while (1)
    
  { 
    
    IWDG_ReloadCounter(); //ι��
    
    LED2_OFF();
    
    Vale = KeyScsn();
    if(Vale==255)   
    {
      if(Shift_Flag==0)
      {
        Shift_Flag=0x55;
        LED3_ON();
      }
      else
      { Shift_Flag=0;
      LED3_OFF();
      }
      Vale=0;
      BEEN_ON();
      Delay_mS(150); 
      BEEN_OFF();
      
    }
    
    
    if(Vale!=0&&Vale!=255)
    {
      
      BEEN_ON();
      Delay_mS(250); 
      BEEN_OFF();
      
      //  USBsendBuffer[0] = 0x02;
      USBsendBuffer[2] = Vale;
      while(GetEPTxStatus(ENDP1)!=EP_TX_NAK)  
      {}
      UserToPMABufferCopy(USBsendBuffer, GetEPTxAddr(ENDP1), 8);
      SetEPTxCount(ENDP1, 8);                     
      
      SetEPTxValid(ENDP1); 
      
      Vale=0;
    }
    else
    {
      for(i=0;i<8;i++)
      {
        USBsendBuffer[i] =0;
      }
      while(GetEPTxStatus(ENDP1)!=EP_TX_NAK)  
      {}
      UserToPMABufferCopy(USBsendBuffer, GetEPTxAddr(ENDP1), 8);
      SetEPTxCount(ENDP1, 8);                     
      
      SetEPTxValid(ENDP1); 
      
    }
    
    LED2_ON();
    
    Delay_mS(50); 
  }
}
/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(__IO uint32_t nCount)
{
  for (; nCount != 0;nCount--);
}

#ifdef  USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif




/***************************************************************
* ������         : USART1_Send
*����            :�򴮿�һ����һ�����ȵ��ֽ���
* ��ں���       : u8 *rxBuffer--������������
u8 length   --���������ֽ���
* ���ں���       : ��
* ����           : ��
*****************************************************************/
void USART1_Send(u8 *rxBuffer, u8 length)
{
  u8 i;
  for(i=0;i<length;i++)
  {
    USART_SendData(USART1, (u8)rxBuffer[i]);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }
  }
  
}



/***************************************************************
* ������         : EP1_IN_Callback()
*����            :PC��ʱ�����жϺ���
* ��ں���       : 
* ���ں���       :
* ����           : 
*****************************************************************/

void EP1_IN_Callback(void)
{
  
}
/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
 /***************************************************************
* ������         : IWDG_Configuration
*����            :���Ź�����
* ��ں���       : 
* ���ں���       : ��
* ����           : ��
*****************************************************************/
void IWDG_Configuration(void)
{
 /* д��0x5555,�����������Ĵ���д�빦�� */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
 
 /* ����ʱ�ӷ�Ƶ,40K/256=156HZ(6.4ms)*/
 IWDG_SetPrescaler(IWDG_Prescaler_256);

 
 /* ι��ʱ�� 5s/6.4MS=781 .ע�ⲻ�ܴ���0xfff*/
 IWDG_SetReload(781);
 
 /* ι��*/
 IWDG_ReloadCounter();

  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  {
    RCC_ClearFlag();
  }
 
 /* ʹ�ܹ���*/
 IWDG_Enable();

}