#include <stm32f4xx.h>
#include <system_stm32f4xx.h>
#include <core_cm4.h>
#include "itoa.h"
#include "LCD_6300.h"
#include "string.h"
#include "main.h"
#include <misc.h>
#include <stm32f4xx_fsmc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_dma.h>
#include <stm32f4xx_i2c.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_rtc.h>
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
static TCHAR lfname[_MAX_LFN];
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
//////////////////////////////////////TASKs//////////////////////////////////////////////////////
xTaskHandle                   Task_Handle;
xTaskHandle                   Demo_Handle;
xTaskHandle                   MP3_Handle;
xTaskHandle                   video_Handle;
xTimerHandle                  TouchScreenTimer;

static void Background_Task(void * pvParameters);
static void Demo_Task(void * pvParameters);
static void vTimerCallback( xTimerHandle pxTimer );
static void MP3_player(void *pvParameters);
static void video_player(void *pvParameters);


uint8_t buffer[512];
char name[107];
//char buffer[1];
//char name[1];
uint8_t _aucLine[1];
uint8_t pagebuff[1];
FIL fsrc;
FATFS fs;
DIR dir;
FILINFO fno;
FRESULT f=0;
char buer[24]="dziala to";

int main(void)
{
	  SystemInit();
	  RCC_cfg();
	  NVIC_Config();
	  GPIO_cfg();
//	  backlight();
	  LED_ON;
	  FSMC_NAND_Init();
	  LCD_init();
	  LCD_WRITE_COMMAND=(MADCTR);
	  LCD_WRITE_DATA = (0x66);
	  LCD_paint(0x00fff0);
	  RNG_Cmd(ENABLE);
	  FATFS fs;
	  f_mount(&fs,"",0);
	  i2c_ini();
	  stmpe811_init();
	  VS1003_Config();
	  Mp3Reset();
	  VS1053_Start();

//	   SRAM_Init();
//	   SRAM_WriteBuffer(buer,0x00000000,9);
//
//	   int k=123456789;
//	   SRAM_WriteBuffer(&k,0x00000000,2);

//	   f=f_open(&fsrc,"moje/jas.rgb",FA_READ|FA_OPEN_EXISTING);

//	   LCD_area(0,0,239,319);
//
//	   for(int i=0;i<450;i++)
//	   {
//		   f_read(&fsrc,buffer,512,&n);
//	   	   DMA_Config(512,buffer);
//	   	   while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
//	   	   DMA2->LISR=0x00000000;
//		   SRAM_WriteBuffer(buffer,i*512,512);
//	   }

//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
//	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
//	 RCC->APB1ENR |= RCC_APB1ENR_PWREN;
//
//
//	   GPIO_InitTypeDef GPIO_InitStructure;
//	   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
//	   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
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

//	  while(1);

	  GUI_Init();
	  GUI_CURSOR_Show();
	  WM_MOTION_Enable(1);



//		   for(int i=0;i<450;i++)
//			  {
//			   	   SRAM_ReadBuffer(buffer,i*512,512);
//
//			  }
//	  char bu[24]="piotrek";
//	  int a=0;
//	  SRAM_ReadBuffer(&a,0x00000000,2);
//	  GUI_DispDec(a,11);
//	  SRAM_ReadBuffer(bu,0x00000000,9);
//	  GUI_DispString(bu);
//
//	  FRESULT a=f_open(&fsrc,"music/Bbagno.mp3", FA_READ | FA_OPEN_EXISTING );
//	  if(a==0)LED_OFF;
//	  while(1);
//	  {


//		  if(button)
//		  {

//			  USBD_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
//			  while(1);
			  LED_OFF;
//		  }

//	  }
//
//	  int rnd=0;
//	  while(RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);
//	  rnd=RNG_GetRandomNumber();
//	  RNG_ClearFlag(RNG_SR_DRDY);

//	  GUI_DispDec(rnd%14+1,2);

//	  CreateMojeokno();

//	  while(DREQ);


			  f_opendir(&dir,"0:music");
			  f_open(&fsrc,"music/Bbagno.mp3", FA_READ | FA_OPEN_EXISTING );

//	  xTaskCreate(video_player,(char const*)"video_player",8192,NULL,tskIDLE_PRIORITY + 2,&video_Handle);
	  xTaskCreate(MP3_player,(char const*)"MP3_player",2048,NULL,tskIDLE_PRIORITY + 3,&MP3_Handle);
//	  xTaskCreate(Demo_Task,(char const*)"GUI_DEMO",4096,NULL,tskIDLE_PRIORITY  + 3,&Demo_Handle);
	  xTaskCreate(Background_Task,(char const*)"BK_GND",2048,NULL,tskIDLE_PRIORITY + 2,&Task_Handle);

	  /* Launch Touchscreen Timer */
	  TouchScreenTimer = xTimerCreate ("Timer",20, pdTRUE,( void * ) 1, vTimerCallback );
//
	  if( TouchScreenTimer != NULL )
	  {
	     if( xTimerStart( TouchScreenTimer, 0 ) != pdPASS )
	     {

	     }
	  }

	    vTaskStartScheduler();
}


void Demo_Task( void * pvParameters)
{
	portTickType xLastFlashTime;
	xLastFlashTime = xTaskGetTickCount();

	while(1)
	{
		vTaskDelayUntil(&xLastFlashTime, 100/portTICK_RATE_MS);///////////////////////////////////
//		SRAM_WriteBuffer("DZIAKA",0xD0600000,5);
//		GUI_DispStringAt("dziala",10,10);
	}
}
static void video_player(void *pvParameters)
{
	while(1)
	{
//		play_video("bird19.avi");
	}
}

static void MP3_player(void *pvParameters)
{
	portTickType xLastFlashTime;
	xLastFlashTime = xTaskGetTickCount();
	u16 br=0;
	u8 fin=0;
	u8 rd=0;
	FRESULT f;
	int size=0;

//	f_open(&fsrc,"music/fink.mp3", FA_READ | FA_OPEN_EXISTING );

	PROGBAR_Handle hProgBar;
	hProgBar = PROGBAR_Create(40, 200, 200, 10, WM_CF_SHOW);

	name[0]='m';
	name[1]='u';
	name[2]='s';
	name[3]='i';
	name[4]='c';
	name[5]='/';

	while(1)
	{


		SCI_ChipSelect(RESET);

		taskENTER_CRITICAL();
		f=f_read(&fsrc, buffer, sizeof(buffer), &br);
	    taskEXIT_CRITICAL();



		   if(f == 0)
		   {
			   SDI_ChipSelect(SET);
			   for (int i=0;i<br;i++)
			   {
					 SPIPutChar(buffer[i]);
					 while(DREQ);
			   }
			   SDI_ChipSelect(RESET);

			   Mp3SetVolume(0x4a,0x4a);

			   if(br<sizeof(buffer))
			   {
				   fin=1;
			   }
		   }

		 int dec=Mp3ReadRegister(SPI_DECODE_TIME);
		 GUI_DispDecAt(dec,100,100,5);
		 int dec1=size/(VS1003_GetBitrate()*1000/8);
		 GUI_DispDecAt(dec1,150,150,10);
		 PROGBAR_SetBarColor(hProgBar, 0, GUI_GREEN);
		 PROGBAR_SetBarColor(hProgBar, 1, GUI_BLUE);
		 PROGBAR_SetValue(hProgBar, dec*100/dec1 );

//		 GUI_DispString("ok");

//		 vTaskDelayUntil(&xLastFlashTime, 1000/portTICK_RATE_MS);

		if(button)
		{
			while(button){};

			   fin=1;
	    }

		if(fin==1)
		{
//			u8 k=rd;

//			while(k>0)
//			{
//
				fno.lfname = lfname;
				fno.lfsize = _MAX_LFN;

				for(u8 k=6;k<106;k++)
				{
					name[k]=0;
				}

				taskENTER_CRITICAL();
				if(!f_readdir(&dir, &fno) && fno.fname[0])
				{
					taskEXIT_CRITICAL();
					int b=sizeof(fno.fname);
					int h=6;

					if(*fno.lfname!=0)
					{
						while(*fno.lfname != 0)
						{
							name[h]=*fno.lfname;
							fno.lfname++;
							h++;
						}
					}
					else
					{
						for(int j=6;j<b+6;j++)
						{
							name[j]=fno.fname[j-6];
						}
					}
				}
				taskEXIT_CRITICAL();
//				k--;
//			}

			taskENTER_CRITICAL();
			Mp3Reset();
			f_open(&fsrc,name,FA_READ | FA_OPEN_EXISTING);
			size=f_size(&fsrc);
			taskEXIT_CRITICAL();
			GUI_SetFont(GUI_FONT_COMIC18B_ASCII);
			GUI_DispStringAt(name,-5,30);
			fin=0;
		}

		vTaskDelayUntil(&xLastFlashTime, 300/portTICK_RATE_MS);
	}
}

static void vTimerCallback( xTimerHandle pxTimer )
{
	 BSP_Pointer_Update();
}

static void Background_Task(void * pvParameters)
{
	portTickType xLastFlashTime;
	xLastFlashTime = xTaskGetTickCount();

  while(1)
  {
	  GUI_Delay(250);
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
	TIM5->PSC =   1;
	TIM5->ARR =   66;
	TIM5->CCR1 =  33;
//	TIM5->CCR1 =  10;
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

//	GPIOD->MODER|=GPIO_MODER_MODER7_1;
//	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR7;
//	GPIO_PinAFConfig(GPIOD,GPIO_PinSource7, GPIO_AF_FSMC);

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

	GPIOD->MODER|=GPIO_MODER_MODER7_0;
	GPIOD->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR7_1;
	GPIOD->ODR|=GPIO_ODR_ODR_7;
	GPIOD->BSRRH|=GPIO_BSRR_BS_7;

	GPIOB->MODER|=GPIO_MODER_MODER3_0;
	GPIOB->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR3_1;
	GPIOB->ODR|=GPIO_ODR_ODR_3;
	GPIOB->BSRRL|=GPIO_BSRR_BS_3;
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
//void vApplicationTickHook(void)
//{
//	OS_TimeMS++;
//}
void vApplicationMallocFailedHook( void )
{
  while (1)
  {}
}
void vApplicationIdleHook(void)
{
	int i=0;

	while(1)
	{
		i++;

		if(i>90000000)
		{
			LCD_String_lc("in idle",5,10,RED,GREEN,2);
//			GUI_DispStringAt("IN IDLE",50,50);
			i=0;
		}
//		LCD_box_mid_fill(0,0,5,5,RED);
//		GUI_DispStringAt("IN IDLE",50,50);
//		LCD_String_lc("in idle",5,10,RED,GREEN,2);
	}
}
//	  USBD_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
//	  while(1){};





