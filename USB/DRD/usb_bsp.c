/*
 ******************************************************************************
 * @file    usb_bsp.c
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    19-March-2012
 * @brief   This file is responsible to offer board support package and is
 *          configurable by user.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/*
 * Updated by Moreto to work with NucleoF401
 *
 * Using TIM3 for delay functions using the USE_ACCURATE_TIME definition.
 * NucleoF401 USB MSC Host and Device Demo
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "usb_bsp.h"
#include "usbd_conf.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
* @{
*/

/** @defgroup USB_BSP
* @brief This file is responsible to offer board support package
* @{
*/ 

/** @defgroup USB_BSP_Private_Defines
  * @{
  */
#define USE_ACCURATE_TIME
/* Configured to work with TIM3. */
#define TIM_MSEC_DELAY                     0x01
#define TIM_USEC_DELAY                     0x02
#define HOST_OVRCURR_PORT                  GPIOB
#define HOST_OVRCURR_LINE                  GPIO_Pin_2
#define HOST_OVRCURR_PORT_SOURCE           GPIO_PortSourceGPIOB
#define HOST_OVRCURR_PIN_SOURCE            GPIO_PinSource2
#define HOST_OVRCURR_PORT_RCC              RCC_AHB1Periph_GPIOB
#define HOST_OVRCURR_EXTI_LINE             EXTI_Line2
#define HOST_OVRCURR_IRQn                  EXTI2_IRQn

/* Pin to control the external 5V switch of step up converter */
#define HOST_POWERSW_PORT_RCC             RCC_AHB1Periph_GPIOA
#define HOST_POWERSW_PORT                 GPIOA
#define HOST_POWERSW_VBUS                 GPIO_Pin_8

/** @defgroup USB_BSP_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USB_BSP_Private_Macros
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_BSP_Private_Variables
* @{
*/ 
ErrorStatus HSEStartUpStatus;
#ifdef USE_ACCURATE_TIME
volatile uint32_t BSP_delay = 0;
#endif
/**
* @}
*/ 

/** @defgroup USBH_BSP_Private_FunctionPrototypes
* @{
*/ 
#ifdef USE_ACCURATE_TIME
static void BSP_SetTime(uint8_t Unit);
static void BSP_Delay(uint32_t nTime,uint8_t Unit);
static void USB_OTG_BSP_TimeInit ( void );
#endif
/**
* @}
*/ 

/** @defgroup USB_BSP_Private_Functions
* @{
*/ 


/**
* @brief  USB_OTG_BSP_Init
*         Initilizes BSP configurations
* @param  None
* @retval None
*/

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
 
#ifdef USE_STM3210C_EVAL

  RCC_OTGFSCLKConfig(RCC_OTGFSCLKSource_PLLVCO_Div3);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_OTG_FS, ENABLE) ;

#else // USE_STM322xG_EVAL  
  GPIO_InitTypeDef GPIO_InitStructure;   
 #ifdef USE_USB_OTG_FS 
 
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);  
  
  /* Configure ID DM DP Pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  //GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_OTG1_FS) ;
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG1_FS) ; 
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG1_FS) ;

#ifdef VBUS_SENSING_ENABLED
  /* Configure  VBUS Pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
#if 0 // Not using ID pin.
  /* Configure ID pin */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_OTG1_FS) ;   
#endif

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE) ; 
 #else // USE_USB_OTG_HS 

  #ifdef USE_ULPI_PHY // ULPI
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | 
                         RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOH | 
                           RCC_AHB1Periph_GPIOI, ENABLE);    
  
  
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource3, GPIO_AF_OTG2_HS) ; // D0
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource5, GPIO_AF_OTG2_HS) ; // CLK
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource0, GPIO_AF_OTG2_HS) ; // D1
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource1, GPIO_AF_OTG2_HS) ; // D2
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource5, GPIO_AF_OTG2_HS) ; // D7
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_OTG2_HS) ; // D3
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_OTG2_HS) ; // D4
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_OTG2_HS) ; // D5
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_OTG2_HS) ; // D6
  GPIO_PinAFConfig(GPIOH,GPIO_PinSource4, GPIO_AF_OTG2_HS) ; // NXT
  GPIO_PinAFConfig(GPIOI,GPIO_PinSource11,GPIO_AF_OTG2_HS) ; // DIR
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource0, GPIO_AF_OTG2_HS) ; // STP
  
  // CLK
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  // D0
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3  ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  
  
  // D1 D2 D3 D4 D5 D6 D7
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1  |
    GPIO_Pin_5 | GPIO_Pin_10 | 
      GPIO_Pin_11| GPIO_Pin_12 | 
        GPIO_Pin_13 ;
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  
  // STP
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
  
  //NXT  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOH, &GPIO_InitStructure);  
  
  
  //DIR
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOI, &GPIO_InitStructure);  
  
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_OTG_HS | 
                         RCC_AHB1Periph_OTG_HS_ULPI, ENABLE) ;    
   
  #else
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | 
                                GPIO_Pin_14 | 
                                GPIO_Pin_15;
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource12, GPIO_AF_OTG2_FS) ; 
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_OTG2_FS) ; 
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_OTG2_FS) ;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_OTG_HS, ENABLE) ;   
  #endif
 #endif //USB_OTG_HS
#endif //USE_STM322xG_EVAL
  /* Initialize Timer for delay function */
#ifdef USE_ACCURATE_TIME
  USB_OTG_BSP_TimeInit();
#endif
}
/**
* @brief  USB_OTG_BSP_EnableInterrupt
*         Enable USB Global interrupt
* @param  None
* @retval None
*/
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
#ifdef USE_USB_OTG_HS   
  NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_IRQn;
#else
  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;  
#endif
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_EP1_OUT_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_EP1_IN_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);   
#endif
}

/**
  * @brief  BSP_Drive_VBUS
  *         Drives the Vbus signal through IO
  * @param  state : VBUS states
  * @retval None
  */

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{
  /*
  On-chip 5 V VBUS generation is not supported. For this reason, a charge pump
  or, if 5 V are available on the application board, a basic power switch, must
  be added externally to drive the 5 V VBUS line. The external charge pump can
  be driven by any GPIO output. When the application decides to power on VBUS
  using the chosen GPIO, it must also set the port power bit in the host port
  control and status register (PPWR bit in OTG_FS_HPRT).

  Bit 12 PPWR: Port power
  The application uses this field to control power to this port, and the core
  clears this bit on an overcurrent condition.
  */
#ifndef USE_USB_OTG_HS
  if (0 == state)
  {
    /* DISABLE is needed on output of the Power Switch */
    GPIO_SetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  }
  else
  {
    /*ENABLE the Power Switch by driving the Enable LOW */
    GPIO_ResetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  }
#endif
}


/**
  * @brief  USB_OTG_BSP_ConfigVBUS
  *         Configures the IO for the Vbus and OverCurrent
  * @param  None
  * @retval None
  */

void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{
#ifndef USE_USB_OTG_HS
  GPIO_InitTypeDef GPIO_InitStructure;

#ifdef USE_STM3210C_EVAL
  RCC_APB2PeriphClockCmd(HOST_POWERSW_PORT_RCC, ENABLE);


  /* Configure Power Switch Vbus Pin */
  GPIO_InitStructure.GPIO_Pin = HOST_POWERSW_VBUS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(HOST_POWERSW_PORT, &GPIO_InitStructure);
#else
  #ifdef USE_USB_OTG_FS
  RCC_AHB1PeriphClockCmd(HOST_POWERSW_PORT_RCC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = HOST_POWERSW_VBUS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(HOST_POWERSW_PORT,&GPIO_InitStructure);
  #endif
#endif

  /* By Default, DISABLE is needed on output of the Power Switch */
  GPIO_SetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);

  USB_OTG_BSP_mDelay(200);   /* Delay is need for stabilising the Vbus Low
  in Reset Condition, when Vbus=1 and Reset-button is pressed by user */
#endif
}

/**
  * @brief  USB_OTG_BSP_TimeInit
  *         Initializes delay unit using Timer2
  * @param  None
  * @retval None
  */
#ifdef USE_ACCURATE_TIME
static void USB_OTG_BSP_TimeInit ( void )
{

  NVIC_InitTypeDef NVIC_InitStructure;

  /* Set the Vector Table base address at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);

  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

}
#endif

/**
  * @brief  USB_OTG_BSP_uDelay
  *         This function provides delay time in micro sec
  * @param  usec : Value of delay required in micro sec
  * @retval None
  */
void USB_OTG_BSP_uDelay (const uint32_t usec)
{

#ifdef USE_ACCURATE_TIME
  BSP_Delay(usec,TIM_USEC_DELAY);
#else
  volatile uint32_t count = 0;
  const uint32_t utime = (120 * usec);
  do
  {
    if ( ++count > utime )
    {
      return ;
    }
  }
  while (1);
#endif
}

#if 0
/**
* @brief  USB_OTG_BSP_uDelay
*         This function provides delay time in micro sec
* @param  usec : Value of delay required in micro sec
* @retval None
*/
void USB_OTG_BSP_uDelay (const uint32_t usec)
{
  uint32_t count = 0;
  const uint32_t utime = (120 * usec);
  do
  {
    if ( ++count > utime )
    {
      return ;
    }
  }
  while (1);
}
#endif

/**
* @brief  USB_OTG_BSP_mDelay
*          This function provides delay time in milli sec
* @param  msec : Value of delay required in milli sec
* @retval None
*/
void USB_OTG_BSP_mDelay (const uint32_t msec)
{
#ifdef USE_ACCURATE_TIME
    BSP_Delay(msec,TIM_MSEC_DELAY);
#else
    USB_OTG_BSP_uDelay(msec * 1000);
#endif
}

/**
  * @brief  USB_OTG_BSP_TimerIRQ
  *         Time base IRQ
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_TimerIRQ (void)
{
#ifdef USE_ACCURATE_TIME

  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if (BSP_delay > 0x00)
    {
      BSP_delay--;
    }
    else
    {
      TIM_Cmd(TIM3,DISABLE);
    }
  }
#endif
}

#ifdef USE_ACCURATE_TIME
/**
  * @brief  BSP_Delay
  *         Delay routine based on TIM2
  * @param  nTime : Delay Time
  * @param  unit : Delay Time unit : mili sec / micro sec
  * @retval None
  */
static void BSP_Delay(uint32_t nTime, uint8_t unit)
{

  BSP_delay = nTime;
  BSP_SetTime(unit);
  while(BSP_delay != 0);
  TIM_Cmd(TIM3,DISABLE);
}

/**
  * @brief  BSP_SetTime
  *         Configures TIM2 for delay routine based on TIM2
  * @param  unit : msec /usec
  * @retval None
  */
static void BSP_SetTime(uint8_t unit)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  TIM_Cmd(TIM3,DISABLE);
  TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);


  if(unit == TIM_USEC_DELAY)
  {
    TIM_TimeBaseStructure.TIM_Period = 41;
  }
  else if(unit == TIM_MSEC_DELAY)
  {
    TIM_TimeBaseStructure.TIM_Period = 41999;
  }
  TIM_TimeBaseStructure.TIM_Prescaler = 2;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

  TIM_ARRPreloadConfig(TIM3, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  /* TIM2 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}

#endif
/**
* @}
*/ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
