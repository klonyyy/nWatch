#include "main.h"
#include <stm32f4xx.h>
#include <system_stm32f4xx.h>
#include <core_cm4.h>
#include "itoa.h"
#include "LCD_6300.h"
#include "string.h"
#include <misc.h>
#include <stm32f4xx_fsmc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_dma.h>
#include <stm32f4xx_i2c.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_rtc.h>
#include <stm32f4xx_adc.h>
//#include <stm32f4xx_flash.h>
#include "jpeglib.h"
#include "decode.h"
#include "ff.h"
#include "tjpgd.h"
//////////USB//////////////////
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
//////////touch///////////////
#include "stmpe811.h"
#include "button.h"
#include "list1.h"
#include "textbox.h"
////////LE////////////////////
#include "stm32f4xx_pwr.h"
////////EXPLORER///////////
#include "explorer.h"
/////////////sram//////////////////
#include "sram.h"
////////graphic////////////
#include "bsp.h"
#include "win.h"
#include "GUIDRV_Template.h"
extern __IO int32_t OS_TimeMS;
#define CHECBOX_BUTTON_SIZE 20
////////////////////////////////////MUSIC////////////////////////////////////////////
#include "vs1003.h"
////////////////////////////////////FreeRTOS///////////////////////////////////////////
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "portmacro.h"
#include "trcUser.h"
/////////////////////////////////////APPS//////////////////////////////////////////////
#include "mp3.h"
#include "bar.h"
#include "heading.h"
#include "menu.h"
////////////////////////////////////RNG///////////////////////////////////////////
#include "stm32f4xx_rng.h"
/////////////////////////////////////USB///////////////////////////////////////////////////////
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_Core __ALIGN_END;
volatile uint8_t usb_host_connected_flag = 0;
volatile uint8_t usb_host_disconnected_flag = 0;
///////////////////////////////////////RCC//////////////////////////////////////////////////////////
#define PLL_Q	7			/* Dzielnik dla USB, SDIO, RND - 48MHz */
#define PLL_M	8
#define PLL_N	336  	    /* Glowny mnoznik - 336 MHz */
//#define PLL_N	400
#define PLL_P	2			/* Dzielnik przed VCO - 2 MHz */
/////////////////////////////////////LED's/////////////////////////////////////////////////////////
#define LED_ON GPIOD->BSRRL|=GPIO_BSRR_BS_6
#define LED_OFF GPIOD->BSRRH|=GPIO_BSRR_BS_6
////////////////////////////////////BUTTONS///////////////////////////////////////////////////////
#define button  (!((GPIOE->IDR) & GPIO_IDR_IDR_4))
#define button1  (!((GPIOE->IDR) & GPIO_IDR_IDR_3))
///////////////////////////////////FUNCTIONS////////////////////////////////////////////////////
void EXTI9_5_IRQHandler(void)__attribute__((interrupt));
void TIM3_IRQHandler(void)__attribute__((interrupt));
void RCC_cfg (void);
void RCC_Config (void);
void GPIO_cfg(void);
void NVIC_cfg(void);
void delay(unsigned int ms);
void FSMC_init(void);
void GPIO_Config(void);
void SPI_Config(void);
void DMA_Config(int ele, u8* buf);
void exti_init(void);
void delay_init(void);
int get_random(int form,int to);
void ADC1_Configuration(void);
//////////////////////////////////////TASKs//////////////////////////////////////////////////////
static void Background_Task(void * pvParameters);
static void vTimerCallback( xTimerHandle pxTimer );

uint8_t pagebuff[2340]__attribute((section(".ExRam")));
uint8_t _aucLine[8192]__attribute((section(".ExRam")));
FRESULT f=0;

int main(void)
{
	  SystemInit();
	  RCC_cfg();
	  NVIC_Config();
	  GPIO_cfg();
	  backlight();
	  LED_ON;
	  SRAM_Init();
	  FSMC_NAND_Init();
	  LCD_init();
	  RNG_Cmd(ENABLE);
	  i2c_ini();
	  stmpe811_init();
	  VS1003_Config();
	  Mp3Reset();
	  VS1053_Start();

	  if(button)
	  {

		  USBD_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
		  while(1);
	  }

	  vTraceInitTraceData();

	  WM_SetCreateFlags(WM_CF_MEMDEV);
	  GUI_Init();
	  WM_MOTION_Enable(1);
	  BUTTON_SetReactOnLevel();

//	  GPIOE->BSRRL|=GPIO_BSRR_BS_2;
//
//
//		f=f_mount(&fs,"",0);
//	    f=f_open(&fsrc,"jas.rgb", FA_READ | FA_OPEN_EXISTING );
//	    f=f_open(&fsrc,"0:moje/car1.jpg", FA_READ | FA_OPEN_EXISTING );
//	    jpeg_decode(&fsrc, 320, _aucLine, 0);
//	    f=f_open(&fsrc,"0:big.jpg", FA_READ | FA_OPEN_EXISTING );
//	    jpeg_decode(&fsrc, 320, _aucLine, 0);
//	    play_video("bird19.avi");
//	    while(1);
//
//	   for(int i=0;i<100;i++)
//	   {
//	    f=f_read(&fsrc,buffer,230400,0);
//	   	   SRAM_WriteBuffer1(buffer,i*2304,2304);
//	   }
//
//	    f=f_open(&fsrc,"ikony/tree.rgb", FA_READ | FA_OPEN_EXISTING );
//
//	   int start=230400;
//
//	   for(int i=0;i<100;i++)
//	   {
//		   f=f_read(&fsrc,buffer,2304,0);
//		   SRAM_WriteBuffer1(buffer,i*2304+start,2304);
//	   }
//
//
//	  LCD_area(0,0,239,319);
//
//      for(int i=0;i<10;i++)
//	  {
//	   	   DMA_Config(23040,Bank1_SRAM3_ADDR+i*23040+start);
//	   	   while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
//	   	   DMA2->LISR=0x00000000;
//	  }
//	  LCD_area(0,0,239,319);
//
//      for(int i=0;i<10;i++)
//	  {
//	   	   DMA_Config(23040,Bank1_SRAM3_ADDR+i*23040);
//    	  DMA_Config(230400,buffer);
//	   	   while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
//	   	   DMA2->LISR=0x00000000;
//	  }
//      while(1){};

//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
//	 RCC->APB1ENR |= RCC_APB1ENR_PWREN;
//	   GPIO_InitTypeDef GPIO_InitStructure;
//	   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
//	   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	   GPIO_Init(GPIOA, &GPIO_InitStructure);
//	   GPIO_Init(GPIOB, &GPIO_InitStructure);
//	   GPIO_Init(GPIOC, &GPIO_InitStructure);
//	   GPIO_Init(GPIOD, &GPIO_InitStructure);
//	   GPIO_Init(GPIOE, &GPIO_InitStructure);
//	   GPIO_Init(GPIOF, &GPIO_InitStructure);
//	   GPIO_Init(GPIOG, &GPIO_InitStructure);
//	  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
//	  PWR_EnterSTANDBYMode();
//	   __WFI();
//	  LCD_String("blalal",100,100,1,RED,BLUE,2);
//	  GPIO_WriteBit(CS_PORT,CS_PIN,!State);
//	  GPIO_WriteBit(XDCS_PORT,XDCS_PIN,!State);
//	  GPIO_WriteBit(CS_PORT,CS_PIN,!State);
//	  while(1){};
//
//	  GUI_Init();
//	  GUI_CURSOR_Show();
//	  WM_MOTION_Enable(1);
//	  play_video("avifiles/birds_small2.avi");


	GPIO_InitTypeDef gpio;
	ADC_InitTypeDef adc;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	ADC1_Configuration();

	  xTaskCreate(Background_Task,(char const*)"Background",1024,NULL,tskIDLE_PRIORITY + 7,&Task_Handle);
	  xTaskCreate(Menu,(char const*)"Menu",512,NULL,tskIDLE_PRIORITY + 6,&Menu_Handle);
	  xTaskCreate(Heading_Task,(char const*)"Heading",512,NULL,tskIDLE_PRIORITY  + 6,&Heading_Handle);
	  TouchScreenTimer = xTimerCreate ("Timer",20, pdTRUE,( void * ) 1, vTimerCallback);
//	  xTaskCreate(Manager,(char const*)"Manager",512,NULL,tskIDLE_PRIORITY + 6,&Manager_Handle);

	  if( TouchScreenTimer != NULL )
	  {
	     if( xTimerStart( TouchScreenTimer, 0 ) != pdPASS )
	     {
	     }
	  }
	    vTaskStartScheduler();
}

static void vTimerCallback( xTimerHandle pxTimer )
{
	 taskENTER_CRITICAL();
	 BSP_Pointer_Update();
	 taskEXIT_CRITICAL();
}

static void Background_Task(void * pvParameters)
{
	portTickType xLastFlashTime;
	xLastFlashTime = xTaskGetTickCount();
	xQueue_men = xQueueCreate(10, sizeof(int));

	  while(1)
	  {
		  GUI_Delay(300);
	  }
}
void exti_init(void)
{
	EXTI->IMR= EXTI_IMR_MR5;
	EXTI->RTSR = EXTI_RTSR_TR5;
    NVIC_SetPriority(EXTI9_5_IRQn,1 );
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    SYSCFG ->EXTICR[1] = SYSCFG_EXTICR2_EXTI5_PC;
}

void DMA_Config(int ele, u8* buf)
{

	DMA_InitTypeDef SDDMA_InitStructure;

	DMA2->LIFCR=0x00000000;
	DMA2->HIFCR=0x00000000;
	/* DMA2 Stream3  or Stream6 disable */
	DMA_Cmd(DMA2_Stream0, DISABLE);
	DMA_Cmd(DMA2_Stream0, ENABLE);
	/* DMA2 Stream3  or Stream6 Config */
	DMA_DeInit(DMA2_Stream0);

	SDDMA_InitStructure.DMA_Channel = DMA_Channel_0;
	SDDMA_InitStructure.DMA_PeripheralBaseAddr = buf;
	SDDMA_InitStructure.DMA_Memory0BaseAddr =&LCD_WRITE_DATA;
	SDDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
	SDDMA_InitStructure.DMA_BufferSize = ele;
	SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; /* DMA_MemoryDataSize_Word; */
	SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	SDDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_3QuartersFull; /* DMA_FIFOThreshold_Full */
	SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single; /* DMA_MemoryBurst_INC4 */
	SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init (DMA2_Stream0, &SDDMA_InitStructure);
	/* DMA2 Stream3  or Stream6 enable */
	DMA_Cmd(DMA2_Stream0, ENABLE);

}
void backlight(void)
{
	TIM5->PSC =   1000;
	TIM5->ARR =   1;
//	TIM5->CCR1 =  33;
	TIM5->CCR1 =  1;
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1PE;
	TIM5->CCER|=TIM_CCER_CC1NE|TIM_CCER_CC1P;
	TIM5->CCER |= TIM_CCER_CC1E;
	TIM5->CR1 =   TIM_CR1_CEN;
}

void IR_config(void)
{
	TIM4->PSC = 1;
	TIM4->ARR = 1130; //1166
	TIM4->CCR1 = 350; ///583
	TIM4->CCMR1|= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1PE;
	TIM4->CCER|= TIM_CCER_CC1NE|TIM_CCER_CC1P;
	TIM4->CCER|= TIM_CCER_CC1E;
//	TIM4->CR1&=~TIM_CR1_CEN;
}

void delay_init(void)
{
	TIM3->PSC=1000;
	TIM3->ARR= 500;
	TIM3->DIER|= TIM_DIER_UIE;
	TIM3->CR1|=TIM_CR1_CEN;
}
void RCC_cfg(void)
{
   /* Pin MCO taktowany z PLL, prescaler 3 - 50MHz */
	RCC->CFGR = RCC_CFGR_MCO1PRE_2 | RCC_CFGR_MCO1PRE_0 | RCC_CFGR_MCO1;

	/* Uruchamiamy HSE i czekamy na gotowosc */
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY));

//		RCC->CR |= RCC_CR_HSION;
//		while (!(RCC->CR & RCC_CR_HSIRDY))

	/* Konfiguracja flasha */
	FLASH->ACR = FLASH_ACR_ICEN |       /* instruction cache                   */
			FLASH_ACR_DCEN |              /* data cache                          */
			FLASH_ACR_PRFTEN |            /* prefetch enable                     */
			FLASH_ACR_LATENCY_4WS;        /* 4 wait states                       */

	/* Konfiguracja PLL HSE jako zrodlo PLL, ustawienie dzielnikow Q, M, N, P  */

	RCC->PLLCFGR = (PLL_Q << 24) | RCC_PLLCFGR_PLLSRC_HSE |
			(((PLL_P >> 1) - 1) << 16) | (PLL_N << 6) | PLL_M;

	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY))
		;

	/* PLL jak sygnal taktowania rdzenia, psk 2 dla APB2, psk 4 dla APB1 */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_SW_PLL;
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL))
		;

	/* Inicjalizacja koprocesora FPU */
	SCB->CPACR |= ((3 << 10*2)|(3 << 11*2));

	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
	 RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	 RCC->AHB1ENR |= RCC_AHB1Periph_DMA2;
	 RCC->AHB3ENR |= RCC_AHB3ENR_FSMCEN;
	 RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	 RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	 RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	 RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
	 RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
	 RCC->APB2ENR |= RCC_APB2ENR_SDIOEN;
}

void NVIC_Config(void)
{

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
//	NVIC_EnableIRQ(DMA2_Stream5_IRQn);
//	NVIC_EnableIRQ(TIM3_IRQn);
}
void GPIO_cfg(void)
{

	//*****************LED_CONFIG*******************

	GPIOD->MODER|=GPIO_MODER_MODER6_0;
	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR6;
	GPIOD->ODR|=GPIO_ODR_ODR_6;

	//******************LCD_CONFIG**************************

	GPIOD->MODER|=GPIO_MODER_MODER0_1;
	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR0;
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0, GPIO_AF_FSMC);

	GPIOD->MODER|=GPIO_MODER_MODER1_1;
	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR1;
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1, GPIO_AF_FSMC);

	GPIOD->MODER|=GPIO_MODER_MODER4_1;
	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR4;
//	GPIOD->PUPDR|=GPIO_PUPDR_PUPDR4_0;
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4, GPIO_AF_FSMC);

	GPIOD->MODER|=GPIO_MODER_MODER5_1;
	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR5;
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5, GPIO_AF_FSMC);

	GPIOD->MODER|=GPIO_MODER_MODER7_1;
	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR7;
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource7, GPIO_AF_FSMC);

	GPIOD->MODER|=GPIO_MODER_MODER11_1;
	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR11;
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource11, GPIO_AF_FSMC);

	GPIOD->MODER|=GPIO_MODER_MODER14_1;
	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR14;
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14, GPIO_AF_FSMC);

	GPIOD->MODER|=GPIO_MODER_MODER15_1;
	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR15;
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15, GPIO_AF_FSMC);

	GPIOE->MODER|=GPIO_MODER_MODER7_1;
	GPIOE->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR7;
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7, GPIO_AF_FSMC);

	GPIOE->MODER|=GPIO_MODER_MODER8_1;
	GPIOE->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR8;
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8, GPIO_AF_FSMC);

	GPIOE->MODER|=GPIO_MODER_MODER9_1;
	GPIOE->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR9;
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9, GPIO_AF_FSMC);

	GPIOE->MODER|=GPIO_MODER_MODER10_1;
	GPIOE->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR10;
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource10, GPIO_AF_FSMC);

	GPIOA->MODER|=GPIO_MODER_MODER1_0;
	GPIOA->OTYPER|=GPIO_OTYPER_ODR_1;
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR1;     ////lcd CS
	GPIOA->ODR|=GPIO_ODR_ODR_1;
	GPIOA->BSRRH|=GPIO_BSRR_BS_1;

	GPIOA->MODER|=GPIO_MODER_MODER2_0;
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR2;     ////lcd RST
	GPIOA->ODR|=GPIO_ODR_ODR_2;
//	GPIOA->BSRRL|=GPIO_BSRR_BS_2;

	//////////////////////////backlight///////////////////////////////
	GPIOA->MODER|=GPIO_MODER_MODER0_1;
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR0;
	GPIOA->AFR[0]|=GPIO_AF_TIM5;
	//////////////////////////ON'y-PB9////////////////////////////////////
	GPIOB->MODER|=GPIO_MODER_MODER9_0;
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR9_1;
	GPIOB->ODR|=GPIO_ODR_ODR_9;
	GPIOB->BSRRH|=GPIO_BSRR_BS_9;

//	GPIOD->MODER|=GPIO_MODER_MODER7_0;
//	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR7_1;
//	GPIOD->ODR|=GPIO_ODR_ODR_7;
//	GPIOD->BSRRH|=GPIO_BSRR_BS_7;

	GPIOB->MODER|=GPIO_MODER_MODER3_0;
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR3_1;
	GPIOB->ODR|=GPIO_ODR_ODR_3;
	GPIOB->BSRRL|=GPIO_BSRR_BS_3;

	GPIOF->MODER|=GPIO_MODER_MODER9_0;
	GPIOF->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR9_1;
	GPIOF->ODR|=GPIO_ODR_ODR_9;
	GPIOF->BSRRH|=GPIO_BSRR_BS_9;

//	GPIO_WriteBit(GPIOF,GPIO_PinSource9,1);

	//////////////////////////BLUETOOTH//////////////////////////////////

	GPIOE->MODER|=GPIO_MODER_MODER2_0;         ///////////////////ENABLE PIN
	GPIOE->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR2_1;
	GPIOE->ODR|=GPIO_ODR_ODR_2;
	GPIO_WriteBit(GPIOE,GPIO_PinSource2,0);

	GPIOC->MODER|=GPIO_MODER_MODER3_0;         ///////////////////BRTS
	GPIOC->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR3_1;
	GPIOC->ODR|=GPIO_ODR_ODR_3;
	GPIOC->BSRRH|=GPIO_BSRR_BS_3;

	//////////////////////////TSOP-PB14///////////////////////////////////
//	GPIOB->MODER
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR14;
	GPIOB->PUPDR |=GPIO_PUPDR_PUPDR14_0;
	//////////////////////////LED////////////////////////////////////////
	GPIOB->MODER|=GPIO_MODER_MODER6_1;
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR6;
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);
	/////////////////////// button///////////////////////////////////////
	GPIOE->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR4;
	GPIOE->PUPDR |=GPIO_PUPDR_PUPDR4_0;

	GPIOE->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR3;
	GPIOE->PUPDR |=GPIO_PUPDR_PUPDR3_0;
	///////////////////////I2C//////////////////////////////////////////
	GPIOB->MODER|=GPIO_MODER_MODER10_1;
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR10;
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_I2C2);

	GPIOB->MODER|=GPIO_MODER_MODER11_1;
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR11;
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_I2C2);
	///////////////////////TOUCH_INT////////////////////////////////////
	GPIOC->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR5_0;
//	GPIOC->OTYPER|=GPIO_OTYPER_IDR_5;
	GPIOC->PUPDR|=GPIO_PUPDR_PUPDR5_0;
	////////////////////////////////////////////////////////////////////

	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DREQ_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(DREQ_PORT, &GPIO_InitStructure);
	 LED_OFF;



}
void ADC1_Configuration(void)
{
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_InitTypeDef       ADC_InitStructure;

  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;

  ADC_StructInit(&ADC_InitStructure);

  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_480Cycles);
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles);
  ADC_EOCOnEachRegularChannelCmd(ADC1, ENABLE);

  ADC_Cmd(ADC1, ENABLE);    //The ADC is powered on by setting the ADON bit in the ADC_CR2 register.
  //When the ADON bit is set for the first time, it wakes up the ADC from the Power-down mode.
}

void USBH_ConnectEventCallback(void)
{
	usb_host_connected_flag = 1;
}
void USBH_DisconnectEventCallback(void)
{
	usb_host_disconnected_flag = 1;
}
void delay(unsigned int ms)
{

}
int get_random(int from,int to)
{
		  int rnd;
		  while(RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);
		  rnd=RNG_GetRandomNumber()%to + from;
		  RNG_ClearFlag(RNG_SR_DRDY);
		  return rnd;
}

int bufftoint(char *buff)
{
	int x=buff[3];
	x=x<<8;
	x=x|buff[2];
	x=x<<8;
	x=x|buff[1];
	x=x<<8;
	x=x|buff[0];

	return x;
}
u8 play_video(char *sc)
{
	u8 flag=1;
	int nobytsread=0;
	long long int indexx=0,movilocation=0,framesize=0,framelocation=0;
	FRESULT fres=0;
	FATFS fsrc;

//	NVIC_DisableIRQ(EXTI9_5_IRQn);
	fres=f_open(&fsrc,sc, FA_READ | FA_OPEN_EXISTING);
	if(fres==0)fres=f_read(&fsrc,pagebuff,4,&nobytsread);

	int act=f_size(&fsrc);

	while(pagebuff[0]!='m' || pagebuff[1]!='o'|| pagebuff[2]!='v'|| pagebuff[3]!='i')
	{
	    	indexx++;
	        fres=f_lseek(&fsrc,indexx); // pointer forward
	    	fres=f_read(&fsrc,pagebuff,4,&nobytsread);
	 }
	       indexx+=4;
	       movilocation=indexx;       //movi tag location set
	       indexx=f_size(&fsrc);
//	 	  BUTTON_Handle hButton = BUTTON_Create(110, 110, 100, 60, GUI_ID_SKIP, WM_CF_SHOW);
//	 	  BUTTON_SetText(hButton, "Skip the video");
	while(pagebuff[0]!='i' || pagebuff[1]!='d'|| pagebuff[2]!='x'|| pagebuff[3]!='1')
	{
	       	  indexx--;
	          fres=f_lseek(&fsrc,indexx); // pointer forward
	       	  fres=f_read(&fsrc,pagebuff,4,&nobytsread);
	}

	  cinfo.err = jpeg_std_error(&jerr);
	  jpeg_create_decompress(&cinfo);

	while(flag==1)
	{

		while(pagebuff[0]!='0' || pagebuff[1]!='0'|| pagebuff[2]!='d'|| pagebuff[3]!='c')   ///dc - compressed    db-decompressed
		{
				  indexx++;
				  fres=f_lseek(&fsrc,indexx); // pointer forward
				  fres=f_read(&fsrc,pagebuff,4,&nobytsread);

				  if(indexx>=act)
				{
					  flag=0;
					  jpeg_destroy_decompress(&cinfo);
					  return 0;
				}
		}

	          indexx+=8;          //4bytes from 00dc---->dwflag +4bytes from dwflag--->dwoffset=frame_loaction
	          fres=f_lseek(&fsrc,indexx);
	          fres=f_read(&fsrc,pagebuff,4,&nobytsread);
	          framelocation=movilocation+bufftoint(pagebuff);
	          f_lseek(&fsrc,framelocation);//length+data
	          fres=f_read(&fsrc,pagebuff,4,&nobytsread);
	          framesize=bufftoint(pagebuff);
	          framelocation+=4;  //only data length removed.
	          f_lseek(&fsrc,framelocation);//data     locating finished
	          jpeg_decode(&fsrc, 320, _aucLine, NULL);
//	          load_jpg(&fsrc,"-",Buff,sizeof(Buff));
	          f_lseek(&fsrc,indexx);
	}

	jpeg_destroy_decompress(&cinfo);
	return 0;
}

void vApplicationMallocFailedHook( void )
{
  while (1)
  {
	  LCD_String_lc("MALLOC FAILED",5,10,RED,BLACK,2);
  }
}
//void vApplicationIdleHook(void)
//{
//	int i=0;
//
//	while(1)
//	{
//		i++;
//
//		if(i>9000000)
//		{
//			LCD_String_lc("IN IDLE",5,10,RED,GREEN,2);
//			GUI_DispStringAt("IN IDLE",50,50);
//			LCD_box_mid_fill(318,238,4,4,RED);
//			i=0;
//		}
//		LCD_box_mid_fill(0,0,5,5,RED);
//		GUI_DispStringAt("IN IDLE",50,50);
//		LCD_String_lc("in idle",5,10,RED,GREEN,2);

//	}
//}
//	  USBD_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
//	  while(1){};




