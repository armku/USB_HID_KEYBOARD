/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : usb_core.c
* Author             : MCD Application Team
* Version            : V3.0.1
* Date               : 04/27/2009
* Description        : Standard protocol processing (USB v2.0)
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ValBit(VAR,Place)    (VAR & (1 << Place))
#define SetBit(VAR,Place)    (VAR |= (1 << Place))
#define ClrBit(VAR,Place)    (VAR &= ((1 << Place) ^ 255))

#define Send0LengthData() { _SetEPTxCount(ENDP0, 0); \
    vSetEPTxStatus(EP_TX_VALID); \
  }

#define vSetEPRxStatus(st) (SaveRState = st)
#define vSetEPTxStatus(st) (SaveTState = st)

#define USB_StatusIn() Send0LengthData()
#define USB_StatusOut() vSetEPRxStatus(EP_RX_VALID)

#define StatusInfo0 StatusInfo.bw.bb1 /* Reverse bb0 & bb1 */
#define StatusInfo1 StatusInfo.bw.bb0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t_uint8_t StatusInfo;
bool Data_Mul_MaxPacketSize = FALSE;
/* Private function prototypes -----------------------------------------------*/
static void DataStageOut(void);
static void DataStageIn(void);
static void NoData_Setup0(void);
static void Data_Setup0(void);
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Standard_GetConfiguration.
* Description    : Return the current configuration variable address.
* Input          : Length - How many bytes are needed.
* Output         : None.
* Return         : Return 1 , if the request is invalid when "Length" is 0.
*                  Return "Buffer" if the "Length" is not 0.
*******************************************************************************/
uint8_t *Standard_GetConfiguration(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength =
      sizeof(pInformation->Current_Configuration);
    return 0;
  }
  pUser_Standard_Requests->User_GetConfiguration();
  return (uint8_t *)&pInformation->Current_Configuration;
}

/*******************************************************************************
* Function Name  : Standard_SetConfiguration.
* Description    : This routine is called to set the configuration value
*                  Then each class should configure device themself.
* Input          : None.
* Output         : None.
* Return         : Return USB_SUCCESS, if the request is performed.
*                  Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetConfiguration(void)
{

  if ((pInformation->USBwValue0 <=
      Device_Table.Total_Configuration) && (pInformation->USBwValue1 == 0)
      && (pInformation->USBwIndex == 0)) /*call Back usb spec 2.0*/
  {
    pInformation->Current_Configuration = pInformation->USBwValue0;
    pUser_Standard_Requests->User_SetConfiguration();
    return USB_SUCCESS;
  }
  else
  {
    return USB_UNSUPPORT;
  }
}

/*******************************************************************************
* Function Name  : Standard_GetInterface.
* Description    : Return the Alternate Setting of the current interface.
* Input          : Length - How many bytes are needed.
* Output         : None.
* Return         : Return 0, if the request is invalid when "Length" is 0.
*                  Return "Buffer" if the "Length" is not 0.
*******************************************************************************/
uint8_t *Standard_GetInterface(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength =
      sizeof(pInformation->Current_AlternateSetting);
    return 0;
  }
  pUser_Standard_Requests->User_GetInterface();
  return (uint8_t *)&pInformation->Current_AlternateSetting;
}

/*******************************************************************************
* Function Name  : Standard_SetInterface.
* Description    : This routine is called to set the interface.
*                  Then each class should configure the interface them self.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetInterface(void)
{
  RESULT Re;
  /*Test if the specified Interface and Alternate Setting are supported by
    the application Firmware*/
  Re = (*pProperty->Class_Get_Interface_Setting)(pInformation->USBwIndex0, pInformation->USBwValue0);

  if (pInformation->Current_Configuration != 0)
  {
    if ((Re != USB_SUCCESS) || (pInformation->USBwIndex1 != 0)
        || (pInformation->USBwValue1 != 0))
    {
      return  USB_UNSUPPORT;
    }
    else if (Re == USB_SUCCESS)
    {
      pUser_Standard_Requests->User_SetInterface();
      pInformation->Current_Interface = pInformation->USBwIndex0;
      pInformation->Current_AlternateSetting = pInformation->USBwValue0;
      return USB_SUCCESS;
    }

  }

  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Standard_GetStatus.
* Description    : Copy the device request data to "StatusInfo buffer".
* Input          : - Length - How many bytes are needed.
* Output         : None.
* Return         : Return 0, if the request is at end of data block,
*                  or is invalid when "Length" is 0.
*******************************************************************************/
uint8_t *Standard_GetStatus(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 2;
    return 0;
  }

  StatusInfo.w = 0;
  /* Reset Status Information */

  if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))
  {
    /*Get Device Status */
    uint8_t Feature = pInformation->Current_Feature;

    /* Remote Wakeup enabled */
    if (ValBit(Feature, 5))
    {
      SetBit(StatusInfo0, 1);
    }

    /* Bus-powered */
    if (ValBit(Feature, 6))
    {
      ClrBit(StatusInfo0, 0);
    }
    else /* Self-powered */
    {
      SetBit(StatusInfo0, 0);
    }
  }
  /*Interface Status*/
  else if (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
  {
    return (uint8_t *)&StatusInfo;
  }
  /*Get EndPoint Status*/
  else if (Type_Recipient == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
  {
    uint8_t Related_Endpoint;
    uint8_t wIndex0 = pInformation->USBwIndex0;

    Related_Endpoint = (wIndex0 & 0x0f);
    if (ValBit(wIndex0, 7))
    {
      /* IN endpoint */
      if (_GetTxStallStatus(Related_Endpoint))
      {
        SetBit(StatusInfo0, 0); /* IN Endpoint stalled */
      }
    }
    else
    {
      /* OUT endpoint */
      if (_GetRxStallStatus(Related_Endpoint))
      {
        SetBit(StatusInfo0, 0); /* OUT Endpoint stalled */
      }
    }

  }
  else
  {
    return NULL;
  }
  pUser_Standard_Requests->User_GetStatus();
  return (uint8_t *)&StatusInfo;
}

/*******************************************************************************
* Function Name  : Standard_ClearFeature.
* Description    : Clear or disable a specific feature.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_ClearFeature(void)
{
  uint32_t     Type_Rec = Type_Recipient;
  uint32_t     Status;


  if (Type_Rec == (STANDARD_REQUEST | DEVICE_RECIPIENT))
  {/*Device Clear Feature*/
    ClrBit(pInformation->Current_Feature, 5);
    return USB_SUCCESS;
  }
  else if (Type_Rec == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
  {/*EndPoint Clear Feature*/
    DEVICE* pDev;
    uint32_t Related_Endpoint;
    uint32_t wIndex0;
    uint32_t rEP;

    if ((pInformation->USBwValue != ENDPOINT_STALL)
        || (pInformation->USBwIndex1 != 0))
    {
      return USB_UNSUPPORT;
    }

    pDev = &Device_Table;
    wIndex0 = pInformation->USBwIndex0;
    rEP = wIndex0 & ~0x80;
    Related_Endpoint = ENDP0 + rEP;

    if (ValBit(pInformation->USBwIndex0, 7))
    {
      /*Get Status of endpoint & stall the request if the related_ENdpoint
      is Disabled*/
      Status = _GetEPTxStatus(Related_Endpoint);
    }
    else
    {
      Status = _GetEPRxStatus(Related_Endpoint);
    }

    if ((rEP >= pDev->Total_Endpoint) || (Status == 0)
        || (pInformation->Current_Configuration == 0))
    {
      return USB_UNSUPPORT;
    }


    if (wIndex0 & 0x80)
    {
      /* IN endpoint */
      if (_GetTxStallStatus(Related_Endpoint ))
      {
        ClearDTOG_TX(Related_Endpoint);
        SetEPTxStatus(Related_Endpoint, EP_TX_VALID);
      }
    }
    else
    {
      /* OUT endpoint */
      if (_GetRxStallStatus(Related_Endpoint))
      {
        if (Related_Endpoint == ENDP0)
        {
          /* After clear the STALL, enable the default endpoint receiver */
          SetEPRxCount(Related_Endpoint, Device_Property.MaxPacketSize);
          _SetEPRxStatus(Related_Endpoint, EP_RX_VALID);
        }
        else
        {
          ClearDTOG_RX(Related_Endpoint);
          _SetEPRxStatus(Related_Endpoint, EP_RX_VALID);
        }
      }
    }
    pUser_Standard_Requests->User_ClearFeature();
    return USB_SUCCESS;
  }

  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Standard_SetEndPointFeature
* Description    : Set or enable a specific feature of EndPoint
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetEndPointFeature(void)
{
  uint32_t    wIndex0;
  uint32_t    Related_Endpoint;
  uint32_t    rEP;
  uint32_t   Status;

  wIndex0 = pInformation->USBwIndex0;
  rEP = wIndex0 & ~0x80;
  Related_Endpoint = ENDP0 + rEP;

  if (ValBit(pInformation->USBwIndex0, 7))
  {
    /* get Status of endpoint & stall the request if the related_ENdpoint
    is Disabled*/
    Status = _GetEPTxStatus(Related_Endpoint);
  }
  else
  {
    Status = _GetEPRxStatus(Related_Endpoint);
  }

  if (Related_Endpoint >= Device_Table.Total_Endpoint
      || pInformation->USBwValue != 0 || Status == 0
      || pInformation->Current_Configuration == 0)
  {
    return USB_UNSUPPORT;
  }
  else
  {
    if (wIndex0 & 0x80)
    {
      /* IN endpoint */
      _SetEPTxStatus(Related_Endpoint, EP_TX_STALL);
    }

    else
    {
      /* OUT endpoint */
      _SetEPRxStatus(Related_Endpoint, EP_RX_STALL);
    }
  }
  pUser_Standard_Requests->User_SetEndPointFeature();
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Standard_SetDeviceFeature.
* Description    : Set or enable a specific feature of Device.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetDeviceFeature(void)
{
  SetBit(pInformation->Current_Feature, 5);
  pUser_Standard_Requests->User_SetDeviceFeature();
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Standard_GetDescriptorData.
* Description    : Standard_GetDescriptorData is used for descriptors transfer.
*                : This routine is used for the descriptors resident in Flash
*                  or RAM
*                  pDesc can be in either Flash or RAM
*                  The purpose of this routine is to have a versatile way to
*                  response descriptors request. It allows user to generate
*                  certain descriptors with software or read descriptors from
*                  external storage part by part.
* Input          : - Length - Length of the data in this transfer.
*                  - pDesc - A pointer points to descriptor struct.
*                  The structure gives the initial address of the descriptor and
*                  its original size.
* Output         : None.
* Return         : Address of a part of the descriptor pointed by the Usb_
*                  wOffset The buffer pointed by this address contains at least
*                  Length bytes.
*******************************************************************************/
uint8_t *Standard_GetDescriptorData(uint16_t Length, ONE_DESCRIPTOR *pDesc)
{
  uint32_t  wOffset;

  wOffset = pInformation->Ctrl_Info.Usb_wOffset;
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = pDesc->Descriptor_Size - wOffset;
    return 0;
  }

  return pDesc->Descriptor + wOffset;
}

/*******************************************************************************
* Function Name  : DataStageOut.
* Description    : Data stage of a Control Write Transfer.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void DataStageOut(void)
{
  ENDPOINT_INFO *pEPinfo = &pInformation->Ctrl_Info;
  uint32_t save_rLength;

  save_rLength = pEPinfo->Usb_rLength;

  if (pEPinfo->CopyData && save_rLength)   //�û����������Ⱥͻ�Ҫ�������ݳ��ȶ���Ϊ0������������
  {
    uint8_t *Buffer;
    uint32_t Length;

    Length = pEPinfo->PacketSize;		  //��ʼ��Ϊ���ݰ���
    if (Length > save_rLength)
    {
      Length = save_rLength;			  //ʵ�ʲ���Ҫ��ô���ⳤ�ȸ�Ϊʵ�ʳ���
    }

    Buffer = (*pEPinfo->CopyData)(Length);	   //��λ�û�������
    pEPinfo->Usb_rLength -= Length;
    pEPinfo->Usb_rOffset += Length;

    PMAToUserBufferCopy(Buffer, GetEPRxAddr(ENDP0), Length);
  }

  if (pEPinfo->Usb_rLength != 0)
  {
    vSetEPRxStatus(EP_RX_VALID);/* re-enable for next data reception */
    SetEPTxCount(ENDP0, 0);
    vSetEPTxStatus(EP_TX_VALID);/* Expect the host to abort the data OUT stage */
  }
  /* Set the next State*/
  if (pEPinfo->Usb_rLength >= pEPinfo->PacketSize)
  {										  //����ʣ�೤��ȷ�����ƴ���״̬
    pInformation->ControlState = OUT_DATA;
  }
  else
  {
    if (pEPinfo->Usb_rLength > 0)		 //�������ݵ���С��һ����������һ��Ϊ���һ�δ���
    {
      pInformation->ControlState = LAST_OUT_DATA;
    }
    else if (pEPinfo->Usb_rLength == 0)	 //û�������ˣ��ڴ�״̬����
    {
      pInformation->ControlState = WAIT_STATUS_IN;
      USB_StatusIn();					  //�Ȱ�0�ֽ����ݰ�׼����
    }
  }
}

/*******************************************************************************
* Function Name  : DataStageIn.
* Description    : Data stage of a Control Read Transfer.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void DataStageIn(void)	 //���������Ҫ�ǽ��û������������ݣ���������Ϊ����ͣ������Ƶ����ƶ˵�0"TxADDR"��ָ��Ķ˵��������������
{
  ENDPOINT_INFO *pEPinfo = &pInformation->Ctrl_Info;
  uint32_t save_wLength = pEPinfo->Usb_wLength;		//ʣ�µ���Ҫ����ĳ���
  uint32_t ControlState = pInformation->ControlState;

  uint8_t *DataBuffer;
  uint32_t Length;

  if ((save_wLength == 0) && (ControlState == LAST_IN_DATA))
  {
    if(Data_Mul_MaxPacketSize == TRUE)
    {
      /* No more data to send and empty packet */
      Send0LengthData();		 //��0�ֽ����ݰ���
      ControlState = LAST_IN_DATA;
      Data_Mul_MaxPacketSize = FALSE;
    }							  //0�ֽ����ݰ�Ҳ���ˣ�����״̬�׶�
    else 
    {		 //�Ѿ�������ɣ�����״̬�׶Ρ�
      /* No more data to send so STALL the TX Status*/
      ControlState = WAIT_STATUS_OUT;
      vSetEPTxStatus(EP_TX_STALL);
    }
    
    goto Expect_Status_Out;
  }

  Length = pEPinfo->PacketSize;
  ControlState = (save_wLength <= Length) ? LAST_IN_DATA : IN_DATA;
				  //�������ʣ��Ҫ���͵��ֽ�����ÿ���ܷ��͵�����ֽ�����ȣ�ȷ���Ƿ����һ�η���
  if (Length > save_wLength)
  {			  
    Length = save_wLength;
  }			   //length����Ϊʵ�ʴ����ֽ������������û���������ʣ����ֽ�����Ҳ���ܾ͵������ݰ���

  DataBuffer = (*pEPinfo->CopyData)(Length);	//�����ȡ���û��������������ĵ�ַ����һ�δ���ʱ��ƫ��Ϊ0��DataBufferָ�򻺳�����ͷ

  UserToPMABufferCopy(DataBuffer, GetEPTxAddr(ENDP0), Length);	  //����Length���ֽڵ��˵㷢�ͻ�������ֻҪ������"IN"һ�����������Ϸ��ظ�����

  SetEPTxCount(ENDP0, Length);	   //���÷����ֽڵ���Ŀ

  pEPinfo->Usb_wLength -= Length;
  pEPinfo->Usb_wOffset += Length;
  vSetEPTxStatus(EP_TX_VALID);	   //ʹ�ܶ˵㷢�ͣ�ֻҪ������IN���ư�һ����SIE�ͻὫ���������ظ�����

  USB_StatusOut();/* Expect the host to abort the data IN stage ���ʵ������ʹ����Ҳ��Ч.������ȡ��IN,�������"Out0_Process()"����ɵġ�*/

Expect_Status_Out:
  pInformation->ControlState = ControlState;
}
 //�˺���ִ����������λ�������Ҫ����������һ�δ�����pInformation->ControlState=IN_DATA/LAST_IN_DATA��
 //���ϴ������һ�δ��䣬���������ڴ�����pInformation->ControlState=WAIT_STATUS_OUT
/*******************************************************************************
* Function Name  : NoData_Setup0.
* Description    : Proceed the processing of setup request without data stage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NoData_Setup0(void)
{
  RESULT Result = USB_UNSUPPORT;
  uint32_t RequestNo = pInformation->USBbRequest;
  uint32_t ControlState;

  if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))
  {		 //���������ﴦ��������׼�����������á����õ�ַ�������豸���ԣ�������Զ�̻������ԣ������ǵ�����Ӧ�ı�׼���ú���ʵ�ֵ�
    /* Device Request*/
    /* SET_CONFIGURATION*/
    if (RequestNo == SET_CONFIGURATION)
    {
      Result = Standard_SetConfiguration();
    }

    /*SET ADDRESS*/
    else if (RequestNo == SET_ADDRESS)
    {
      if ((pInformation->USBwValue0 > 127) || (pInformation->USBwValue1 != 0)
          || (pInformation->USBwIndex != 0)
          || (pInformation->Current_Configuration != 0))
        /* Device Address should be 127 or less*/
      {
        ControlState = STALLED;
        goto exit_NoData_Setup0;
      }
      else
      {
        Result = USB_SUCCESS;
      }
    }
    /*SET FEATURE for Device*/
    else if (RequestNo == SET_FEATURE)
    {
      if ((pInformation->USBwValue0 == DEVICE_REMOTE_WAKEUP)
          && (pInformation->USBwIndex == 0)
          && (ValBit(pInformation->Current_Feature, 5)))
      {
        Result = Standard_SetDeviceFeature();
      }
      else
      {
        Result = USB_UNSUPPORT;
      }
    }
    /*Clear FEATURE for Device */
    else if (RequestNo == CLEAR_FEATURE)
    {
      if (pInformation->USBwValue0 == DEVICE_REMOTE_WAKEUP
          && pInformation->USBwIndex == 0
          && ValBit(pInformation->Current_Feature, 5))
      {
        Result = Standard_ClearFeature();
      }
      else
      {
        Result = USB_UNSUPPORT;
      }
    }

  }

  /* Interface Request*/
  else if (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
  {
    /*SET INTERFACE*/
    if (RequestNo == SET_INTERFACE)
    {
      Result = Standard_SetInterface();
    }
  }

  /* EndPoint Request*/
  else if (Type_Recipient == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
  {
    /*CLEAR FEATURE for EndPoint*/
    if (RequestNo == CLEAR_FEATURE)
    {
      Result = Standard_ClearFeature();
    }
    /* SET FEATURE for EndPoint*/
    else if (RequestNo == SET_FEATURE)
    {
      Result = Standard_SetEndPointFeature();
    }
  }
  else
  {
    Result = USB_UNSUPPORT;
  }
	//��������������ϲ��֣��������洦��

  if (Result != USB_SUCCESS)
  {			 //�����û��ṩ���ദ����Class_NoData_Setup����
    Result = (*pProperty->Class_NoData_Setup)(RequestNo);
    if (Result == USB_NOT_READY)
    {		   //����û���Ҳ��֧�����������SETUPʧ��
      ControlState = PAUSE;
      goto exit_NoData_Setup0;
    }
  }

  if (Result != USB_SUCCESS)
  {
    ControlState = STALLED;
    goto exit_NoData_Setup0;
  }
   //������󱻳ɹ�ִ�У�  ����״̬���̣��ȴ�������INָ�Ȼ�󷵻�һ��0�ֽڵ�״̬���ݰ�
  ControlState = WAIT_STATUS_IN;/* After no data stage SETUP ״̬����ΪWAIT_STATUS_IN�Ǹ�IN���ư��Ĵ����ṩָʾ*/

  USB_StatusIn();

exit_NoData_Setup0:
  pInformation->ControlState = ControlState;
  return;
}

/*******************************************************************************
* Function Name  : Data_Setup0.
* Description    : Proceed the processing of setup request with data stage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Data_Setup0(void)
{
  uint8_t *(*CopyRoutine)(uint16_t);   //����һ������ָ�룬ͨ�������������û������ݻ�������ַ
  RESULT Result;
  uint32_t Request_No = pInformation->USBbRequest;

  uint32_t Related_Endpoint, Reserved;
  uint32_t wOffset, Status;



  CopyRoutine = NULL;
  wOffset = 0;

  if (Request_No == GET_DESCRIPTOR)
  {
    if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))  //������usb_def.h������USBbmRequestType
    {
      uint8_t wValue1 = pInformation->USBwValue1;
      if (wValue1 == DEVICE_DESCRIPTOR)
      {
        CopyRoutine = pProperty->GetDeviceDescriptor;
      }
      else if (wValue1 == CONFIG_DESCRIPTOR)
      {
        CopyRoutine = pProperty->GetConfigDescriptor;
      }
      else if (wValue1 == STRING_DESCRIPTOR)
      {
        CopyRoutine = pProperty->GetStringDescriptor;
      }  /* End of GET_DESCRIPTOR */
    }
  }

  /*GET STATUS*/
  else if ((Request_No == GET_STATUS) && (pInformation->USBwValue == 0)
           && (pInformation->USBwLength == 0x0002) //��������Ҫ��2���ֽ�
           && (pInformation->USBwIndex1 == 0))
  {
    /* GET STATUS for Device*/
    if ((Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))  //�������豸����ʱwIndex��0
        && (pInformation->USBwIndex == 0))
    {
      CopyRoutine = Standard_GetStatus;
    }

    /* GET STATUS for Interface*/
    else if (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))  //�����ǽӿڣ���ʱwIndex�ǽӿں�
    {
      if (((*pProperty->Class_Get_Interface_Setting)(pInformation->USBwIndex0, 0) == USB_SUCCESS)
          && (pInformation->Current_Configuration != 0))
      {
        CopyRoutine = Standard_GetStatus;
      }
    }

    /* GET STATUS for EndPoint*/
    else if (Type_Recipient == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))	 //�����Ƕ˵㣬��ʱwIndex�Ƕ˵��
    {
      Related_Endpoint = (pInformation->USBwIndex0 & 0x0f);
      Reserved = pInformation->USBwIndex0 & 0x70;

      if (ValBit(pInformation->USBwIndex0, 7))	 //��0�˵㣿��������������
      {
        /*Get Status of endpoint & stall the request if the related_ENdpoint
        is Disabled*/
        Status = _GetEPTxStatus(Related_Endpoint);
      }
      else										 //�˵�0 ��������������������������
      {
        Status = _GetEPRxStatus(Related_Endpoint);
      }

      if ((Related_Endpoint < Device_Table.Total_Endpoint) && (Reserved == 0)
          && (Status != 0))
      {
        CopyRoutine = Standard_GetStatus;
      }
    }

  }

  /*GET CONFIGURATION*/
  else if (Request_No == GET_CONFIGURATION)
  {
    if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))
    {
      CopyRoutine = Standard_GetConfiguration;
    }
  }
  /*GET INTERFACE*/
  else if (Request_No == GET_INTERFACE)
  {
    if ((Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
        && (pInformation->Current_Configuration != 0) && (pInformation->USBwValue == 0)
        && (pInformation->USBwIndex1 == 0) && (pInformation->USBwLength == 0x0001)
        && ((*pProperty->Class_Get_Interface_Setting)(pInformation->USBwIndex0, 0) == USB_SUCCESS))
    {
      CopyRoutine = Standard_GetInterface;
    }

  }
  		 //���������������Ҫ������ݣ�׼�������ݻ�������λ�á�����	��CopyRoutine
  if (CopyRoutine)		 //����Ǳ�׼�����Ҳ�����ȷ��ִ����������
  {
    pInformation->Ctrl_Info.Usb_wOffset = wOffset;		  //��ʼƫ��Ϊ0
    pInformation->Ctrl_Info.CopyData = CopyRoutine;
    /* sb in the original the cast to word was directly */
    /* now the cast is made step by step */
    (*CopyRoutine)(0);		//���ʵ�ʻ����Ҫ��������ݵĳ��ȡ��������������õ�Ŀ��ֻ��������pInformation����Ҫд����������ĳ���
    Result = USB_SUCCESS;
  }
  else			 //if the request is not supported, CopyRoutine returns 0��������������������û�����
  {
    Result = (*pProperty->Class_Data_Setup)(pInformation->USBbRequest);
    if (Result == USB_NOT_READY)
    {						  //����û�Ҳ��֧�֣�ͨ��ʧ��
      pInformation->ControlState = PAUSE;
      return;
    }
  }
   
  if (pInformation->Ctrl_Info.Usb_wLength == 0xFFFF)
  {
    /* Data is not ready, wait it */
    pInformation->ControlState = PAUSE;
    return;
  }
  if ((Result == USB_UNSUPPORT) || (pInformation->Ctrl_Info.Usb_wLength == 0))
  {
    /* Unsupported request */
    pInformation->ControlState = STALLED;
    return;
  }

  //��������ȷ�����ݽ׶εĴ��䷽������������Ҫ�������ݣ���Set_Descripter��Ҳ������Ҫȡ�����ݣ��磺Get_Descripter��
  if (ValBit(pInformation->USBbmRequestType, 7))
  {							 //������������Ҫ���жϴ��䷽�����Ϊ1�������豸������
    /* Device ==> Host */
    __IO uint32_t wLength = pInformation->USBwLength;	  //����Ҫ��ȡ�ĳ���
     
    /* Restrict the data length to be the one host asks */
    if (pInformation->Ctrl_Info.Usb_wLength > wLength)
    {				   //���ʵ�ʳ��ȳ��������󳤶ȣ���ʵ�ʳ��ȼ���Ϊ�������󳤶�
      pInformation->Ctrl_Info.Usb_wLength = wLength;
    }
    
    else if (pInformation->Ctrl_Info.Usb_wLength < pInformation->USBwLength)
    {
      if (pInformation->Ctrl_Info.Usb_wLength < pProperty->MaxPacketSize)
      {
        Data_Mul_MaxPacketSize = FALSE;
      }
      else if ((pInformation->Ctrl_Info.Usb_wLength % pProperty->MaxPacketSize) == 0)
      {				   //���ʵ�ʳ�����������ݰ�������������Ҫ����0�ֽ����ݰ�
        Data_Mul_MaxPacketSize = TRUE;
      }
    }   		  //���������£�����ָ������Ӧ����ʵ�ʴ��䳤��Ҫ��Ӧ

    pInformation->Ctrl_Info.PacketSize = pProperty->MaxPacketSize;
    DataStageIn();		   //������ݽ׶���"IN"�������ھ�Ҫ׼�������ݣ����ö˵�TX״̬��
  }
  else
  {		//������ݽ׶���"OUT"����Ҫ��"Out0_Process()"�����д�������ֻ��Ҫ�ı�״̬��ʹ�ܶ˵���վ�����
    pInformation->ControlState = OUT_DATA;
    vSetEPRxStatus(EP_RX_VALID); /* enable for next data reception */
  }

  return;
}

/*******************************************************************************
* Function Name  : Setup0_Process
* Description    : Get the device request data and dispatch to individual process.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
uint8_t Setup0_Process(void)
{

  union
  {
    uint8_t* b;
    uint16_t* w;
  } pBuf;

  pBuf.b = PMAAddr + (uint8_t *)(_GetEPRxAddr(ENDP0) * 2); /* *2 for 32 bits addr ����ȡ�ö˵�0���ջ���������ʼ��ַ*/
	  //��SETUP�ж�ʱ����������������������ݰ����Ѿ������˶˵�0�Ľ��ջ�������������ʽ��ȡ�øõ�ַ
  if (pInformation->ControlState != PAUSE)
  {
    pInformation->USBbmRequestType = *pBuf.b++; /* bmRequestType�������ͣ��������򡢷����ߡ��ͽ��ն����豸���ӿڻ��Ƕ˵㣩��ʱΪ80�������豸������*/
    pInformation->USBbRequest = *pBuf.b++; /* bRequest������룬��һ��ʱӦ����6����������Ҫ��ȡ�豸������ */
    pBuf.w++;  /* word not accessed because of 32 bits addressing */
    pInformation->USBwValue = ByteSwap(*pBuf.w++); /* wValue */
    pBuf.w++;  /* word not accessed because of 32 bits addressing */
    pInformation->USBwIndex  = ByteSwap(*pBuf.w++); /* wIndex ���С�ˣ�������������������*/
    pBuf.w++;  /* word not accessed because of 32 bits addressing */
    pInformation->USBwLength = *pBuf.w; /* wLength */
  }

  pInformation->ControlState = SETTING_UP;
  if (pInformation->USBwLength == 0)  // �������ݴ��ͽ׶Σ���Ϊ�����ֽ�����
  {
    /* Setup with no data stage */
    NoData_Setup0();		 //�����õ�ַ������������Щ������û�����ݹ��̵ģ����øú�������
  }
  else
  {
    /* Setup with data stage */
    Data_Setup0();			  //����������ݵĳ��Ȳ�����0�������Data_Setup0���д���.���ȡ�����������ñ�����Щ�Ǵ����ݵġ�
  }
  return Post0_Process();
}

/*******************************************************************************
* Function Name  : In0_Process
* Description    : Process the IN token on all default endpoint.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
uint8_t In0_Process(void)
{
  uint32_t ControlState = pInformation->ControlState;

  if ((ControlState == IN_DATA) || (ControlState == LAST_IN_DATA))
  {
    DataStageIn();	//����������׼�����������.�˴ε��ú󣬵�ǰ״̬���WAIT_STATUS_OUT�������豸�ȴ�״̬���̣��������0�ֽ�
    /* ControlState may be changed outside the function */
    ControlState = pInformation->ControlState;
  }

  else if (ControlState == WAIT_STATUS_IN)
  {
    if ((pInformation->USBbRequest == SET_ADDRESS) &&
        (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT)))
    {
      SetDeviceAddress(pInformation->USBwValue0);
      pUser_Standard_Requests->User_SetDeviceAddress();//���������һ����ֵ��䣬bDeviceState = ADDRESSED
    }
    (*pProperty->Process_Status_IN)();	//����һ���պ���
    ControlState = STALLED;
  }

  else
  {
    ControlState = STALLED;
  }

  pInformation->ControlState = ControlState;

  return Post0_Process();
}

/*******************************************************************************
* Function Name  : Out0_Process
* Description    : Process the OUT token on all default endpoint.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
uint8_t Out0_Process(void)
{
  uint32_t ControlState = pInformation->ControlState;

  if ((ControlState == OUT_DATA) || (ControlState == LAST_OUT_DATA))
  {
    DataStageOut();	  //�����������Ҫ���ǵ��ý������ݴ������˵���ջ������յ������ݸ��Ƶ��û�������
    ControlState = pInformation->ControlState; /* may be changed outside the function */
  }

  else if (ControlState == WAIT_STATUS_OUT)	  //IN�����״̬���̣�������OUT����0���ݰ����豸��ACK
  {
    (*pProperty->Process_Status_OUT)();	   //���Ǹ��պ�����ʲôҲ����
    ControlState = STALLED;
  }

  else if ((ControlState == IN_DATA) || (ControlState == LAST_IN_DATA))
  {
    /* host aborts the transfer before finish */
    ControlState = STALLED;
  }

  /* Unexpect state, STALL the endpoint */
  else
  {
    ControlState = STALLED;
  }

  pInformation->ControlState = ControlState;

  return Post0_Process();
}

/*******************************************************************************
* Function Name  : Post0_Process
* Description    : Stall the Endpoint 0 in case of error.
* Input          : None.
* Output         : None.
* Return         : - 0 if the control State is in PAUSE
*                  - 1 if not.
*******************************************************************************/
uint8_t Post0_Process(void)
{
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize); //���˵�0�Ļ�������С����Ϊ64�ֽ�

  if (pInformation->ControlState == STALLED)
  {
    vSetEPRxStatus(EP_RX_STALL);
    vSetEPTxStatus(EP_TX_STALL);
  }				// ���˵�0�ķ��ͺͽ��ն�����Ϊ��STALL������״̬��ֻ����SETUP���ư���������Ӧ"IN"��"OUT"��

  return (pInformation->ControlState == PAUSE);
}

/*******************************************************************************
* Function Name  : SetDeviceAddress.
* Description    : Set the device and all the used Endpoints addresses.
* Input          : - Val: device adress.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SetDeviceAddress(uint8_t Val)
{
  uint32_t i;
  uint32_t nEP = Device_Table.Total_Endpoint;

  /* set address in every used endpoint */
  for (i = 0; i < nEP; i++)
  {
    _SetEPAddress((uint8_t)i, (uint8_t)i);
  } /* for */
  _SetDADDR(Val | DADDR_EF); /* set device address and enable function */
}

/*******************************************************************************
* Function Name  : NOP_Process
* Description    : No operation function.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NOP_Process(void)
{
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
