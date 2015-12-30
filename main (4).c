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
/////////////////////////////////////APPS//////////////////////////////////////////////
#include "mp3.h"
#include "bar.h"
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
u8 rand[200];
uint8_t _aucLine[1];
uint8_t pagebuff[1];
FRESULT f=0;
FILINFO fno;
char buer[24]="dziala ko";

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

	  GUI_Init();
//	  GUI_CURSOR_Show();
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
//	  while(1){};



		  if(button)
		  {

			  USBD_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
			  while(1);
//			  LED_OFF;
		  }

//	  }
//


//	  GUI_DispDec(rnd%14+1,2);

//	  CreateMojeokno();

//	  while(1){GUI_Delay(100);};
//	  while(DREQ);




			    GPIO_InitTypeDef gpio;
			  	ADC_InitTypeDef adc;

			  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

//			  	ADC_TempSensorVrefintCmd(ENABLE);

			    GPIO_InitTypeDef GPIO_InitStructure;
			    //for ADC1 on Pb1 using IN9
			    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
			    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
			    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
			    GPIO_Init(GPIOB, &GPIO_InitStructure);

			    ADC1_Configuration();


//			  MP3_player(1);

//	  xTaskCreate(video_player,(char const*)"video_player",4096,NULL,tskIDLE_PRIORITY + 2,&video_Handle);
	  xTaskCreate(MP3_player,(char const*)"MP3_player",4096,NULL,tskIDLE_PRIORITY + 2,&MP3_Handle);
	  xTaskCreate(Demo_Task,(char const*)"GUI_DEMO",2048,NULL,tskIDLE_PRIORITY  + 2,&Demo_Handle);
	  xTaskCreate(Background_Task,(char const*)"BK_GND",2048,NULL,tskIDLE_PRIORITY + 2,&Task_Handle);
	  /* Launch Touchscreen Timer */
	  TouchScreenTimer = xTimerCreate ("Timer",50, pdTRUE,( void * ) 1, vTimerCallback);

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

	WM_HWIN hWin=CreateBar();
	WM_HWIN hItem;

	while(1)
	{
		  vTaskDelayUntil(&xLastFlashTime, 500/portTICK_RATE_MS);///////////////////////////////////
		  ADC_SoftwareStartConv(ADC1);
		  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		  int result= ADC_GetConversionValue(ADC1);
		  result = result * 8059/10000;
		  hItem = WM_GetDialogItem(hWin, ID_PROGBAR_2);
		  PROGBAR_SetValue(hItem,result*100/3100);
//		  GUI_DispDecAt(result,20,0,5);
//		  GUI_DispStringAt("dzial",10,10);
	}
}
static void video_player(void *pvParameters)
{
	WM_HWIN hWin=CreateWindow();
	WM_HWIN hItem;

	while(1)
	{
		 hItem = WM_GetDialogItem(hWin, ID_TEXT_1);
		 TEXT_SetText(hItem,"dziala");
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
	int size=0,co=0,vol=0x4a,on=1;
	int next=0;
	FIL fsrc;
	DIR dir;
	FATFS fs;
//	FILINFO fno;

	taskENTER_CRITICAL();

	  f_mount(&fs,"",0);
	  f_opendir(&dir,"0:music");
	  f_open(&fsrc,"music/fink.mp3", FA_READ | FA_OPEN_EXISTING );

	fno.lfname = lfname;
	fno.lfsize = _MAX_LFN;

	while(!f_readdir(&dir, &fno) && fno.fname[0])
	{
		co++;
	}
	rd=co;

	while(rd--)
	{
		int k=get_random(0,co);

		for(int j=0;j<co-1;j++)
		{
			if(k==rand[j])br=1;
		}
		if(br==0)rand[rd]=k;
		else
		{
			br=0;
			rd++;
		}
	}


	br=0;
	rd=0;

	taskEXIT_CRITICAL();

	GUI_SetFont(GUI_FONT_COMIC18B_ASCII);
	WM_HWIN hWin=CreateWindow();
	WM_HWIN hText,hText1;
	WM_HWIN hSlider;
	WM_HWIN hProgBar;
	WM_HWIN hButton,hButton1,hButton2;

	name[0]='m';
	name[1]='u';
	name[2]='s';
	name[3]='i';
	name[4]='c';
	name[5]='/';

	while(1)
	{

//		hText1 = WM_GetDialogItem(hWin, ID_TEXT_1);
		hText = WM_GetDialogItem(hWin, ID_TEXT_0);
		hSlider = WM_GetDialogItem(hWin, ID_SLIDER_0);
//		hSlider1 = WM_GetDialogItem(hWin, ID_SLIDER_1);
		hProgBar = WM_GetDialogItem(hWin, ID_PROGBAR_0);
		hButton = WM_GetDialogItem(hWin, ID_BUTTON_0);
		hButton1 = WM_GetDialogItem(hWin, ID_BUTTON_1);

//		GUI_DispDecAt(uxTaskGetNumberOfTasks(),150,0,2);

		SCI_ChipSelect(RESET);

		taskENTER_CRITICAL();
		f=f_read(&fsrc, buffer, sizeof(buffer), &br);
		taskEXIT_CRITICAL();


		   if(f == 0)
		   {
			   vol=SLIDER_GetValue(hSlider);
//			   bass=SLIDER_GetValue(hSlider1);
//			   taskENTER_CRITICAL();

			   SDI_ChipSelect(SET);
			   for (int i=0;i<br;i++)
			   {
					 SPIPutChar(buffer[i]);
					 while(DREQ);
			   }
			   SDI_ChipSelect(RESET);
			   Mp3SetVolume(vol, vol);

			   if(br<sizeof(buffer))
			   {
				   fin=1;
			   }
		   }
		   Mp3SoftReset();

		 int dec1=size/(VS1003_GetBitrate()*1000/8);
		 u16 dec=Mp3ReadRegister(SPI_DECODE_TIME);
		 PROGBAR_SetValue(hProgBar, dec*100/dec1 );

		if(button)
		{
			while(button){};
			if(on==1)
			{
				GPIOB->BSRRL|=GPIO_BSRR_BS_9;
//				vTaskSuspend(TouchScreenTimer);
				on=0;
			}
			else
			{
				GPIOB->BSRRH|=GPIO_BSRR_BS_9;
//				vTaskResume(TouchScreenTimer);
				LCD_init();
				on=1;
			}

	    }

		if(BUTTON_IsPressed(hButton))
		{
			while(BUTTON_IsPressed(hButton)){};
			fin=1;
		}
		if(BUTTON_IsPressed(hButton1))
		{
			while(BUTTON_IsPressed(hButton1)){};
			fin=1;
			next-=2;
		}
//

		if(fin==1)
		{

//			GUI_DispDecAt(co,0,0,5);

			taskENTER_CRITICAL();

			next++;

			if(next==co)
			{
				next=0;
			}
//
				fno.lfname = lfname;
				fno.lfsize = _MAX_LFN;

				for(u8 k=6;k<106;k++)
				{
					name[k]=0;
				}


				f_opendir(&dir,"0:music");

				rd=0;

				while(rd!=rand[next])
				{
					f_readdir(&dir, &fno);
					rd++;
				}



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

			Mp3Reset();
			taskENTER_CRITICAL();
			f_open(&fsrc,name,FA_READ | FA_OPEN_EXISTING);
			size=f_size(&fsrc);
			taskEXIT_CRITICAL();
//
			TEXT_SetText(hText,&name[6]);
			fin=0;
		}

		vTaskDelayUntil(&xLastFlashTime, 500/portTICK_RATE_MS);

	}

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

  while(1)
  {
//	  taskENTER_CRITICAL();
	  GUI_Delay(250);
//	  GUI_Clear
//	  taskEXIT_CRITICAL();
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

	GPIOF->MODER|=GPIO_MODER_MODER9_0;
	GPIOF->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR9_1;
	GPIOF->ODR|=GPIO_ODR_ODR_9;
	GPIOF->BSRRH|=GPIO_BSRR_BS_9;

//	GPIO_WriteBit(GPIOF,GPIO_PinSource9,1);

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





