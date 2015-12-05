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
//#include <stm32f4xx_flash.h>
//#include "jpeglib.h"
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
#include "scroll_list.h"
#include "textbox.h"
////////LE////////////////////
#include "stm32f4xx_pwr.h"
///////FreeRTOS///////////////
#include <FreeRTOS.h>
#include <task.h>

#define digi_w  60
#define digi_h  40

void vStartLEDTasks(unsigned portBASE_TYPE uxPriority);
void vTaskLED1 ( void * pvParameters);
void vTaskLED2 ( void * pvParameters);
void vhToggleLED1(void);

char* keyboard(void);
void button_ini(void);


int main(void)
{
	SystemInit();

//	SysTick_Config(1680);
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

	  NVIC_Config();
	  GPIO_cfg();
	  backlight();
	  FSMC_NAND_Init();
	  LCD_init();
	  LCD_paint(0xfff000);
	  LCD_WRITE_COMMAND=(MADCTR);
	  LCD_WRITE_DATA = (0x66);

	  FILINFO fno;
	  FATFS fs;
	  FIL fsrc;
	  f_mount(&fs,"",0);

	  I2c_init();
	  stmpe811_init();
	  Exti_init();

	  vStartLEDTasks(0);
	  vTaskStartScheduler();

	  while(1){};


}

void vStartLEDTasks(unsigned portBASE_TYPE uxPriority)
{
	xTaskHandle xHandleTaskLED1;

	xTaskCreate(vTaskLED1, (signed portCHAR*) "LED1", 1024, 0, uxPriority, &xHandleTaskLED1 );

}
void vTaskLED1 ( void * pvParameters)
{
	portTickType xLastFlashTime;

//	xLastFlashTime = xTaskGetTickCount();

	while(1)
	{
//		vTaskDelayUntil(&xLastFlashTime, 5000/portTICK_RATE_MS);
		vhToggleLED1();
	}
}

//void vTaskLED2 ( void * pvParameters)
//{
//	portTickType xLastFlashTime;
//
//	xLastFlashTime = xTaskGetTickCount();
//	while(1)
//	{
//	vTaskDelayUntil(&xLastFlashTime, 1000/portTICK_RATE_MS);
//	vhToggleLED2();
//	}
//}

void vhToggleLED1(void)
{

//	LCD_paint(0x0000ff);
	keyboard();
//	if(GPIOD->BSRR & GPIO_BSRR_BS6)GPIOD->BSRR|=GPIO_BSRR_BR6;
//	else GPIOD->BSRR|=GPIO_BSRR_BS6;
}

void backlight(void)
{
	TIM5->PSC =   1;
	TIM5->ARR =   66;
	TIM5->CCR1 =  33;
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1PE;
	TIM5->CCER|=TIM_CCER_CC1NE|TIM_CCER_CC1P;
	TIM5->CCER |= TIM_CCER_CC1E;
	TIM5->CR1 =   TIM_CR1_CEN;
}

void NVIC_Config(void)
{
//	NVIC_EnableIRQ(DMA2_Stream5_IRQn);
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
}



void button_ini(void)
{

		u8 xx=35;
		u8 yy=45;

		Button click1;
		Button click2;
		Button click3;
		Button click4;
		Button click5;
		Button click6;
		Button click7;
		Button click8;
		Button click9;
		Button click10;
		Button delete;
		Button enter;

		  enter.color=~GREEN;
		  enter.x=xx-10;
		  enter.y=yy+100;
		  enter.wrap=0;
		  enter.used=1;
		  enter.width=digi_w-10;
		  enter.height=180;
		  enter.text="ENTER";
		  enter.id=button_add(&enter);
		  button_show(enter.id);

		  delete.color=~RED;
		  delete.x=xx+250;
		  delete.y=yy+100;
		  delete.wrap=0;
		  delete.used=1;
		  delete.width=digi_w-10;
		  delete.height=180;
		  delete.text="DELETE";
		  delete.id=button_add(&delete);
		  button_show(delete.id);

		  click1.color=~YELLOW;
		  click1.x=xx+50;
		  click1.y=yy+30;
		  click1.wrap=0;
		  click1.used=1;
		  click1.width=digi_w;
		  click1.height=digi_h;
		  click1.text="1 .,!";
		  click1.id=button_add(&click1);
		  button_show(click1.id);

		  click2.color=~YELLOW;
		  click2.x=xx+120;
		  click2.y=yy+30;
		  click2.wrap=0;
		  click2.used=1;
		  click2.width=digi_w;
		  click2.height=digi_h;
		  click2.text="2 ABC";
		  click2.id=button_add(&click2);
		  button_show(click2.id);

		  click3.color=~YELLOW;
		  click3.x=xx+190;
		  click3.y=yy+30;
		  click3.wrap=0;
		  click3.used=1;
		  click3.width=digi_w;
		  click3.height=digi_h;
		  click3.text="3 DEF";
		  click3.id=button_add(&click3);
		  button_show(click3.id);


		  click4.color=~YELLOW;
		  click4.x=xx+50;
		  click4.y=yy+80;
		  click4.wrap=0;
		  click4.used=1;
		  click4.width=digi_w;
		  click4.height=digi_h;
		  click4.text="4 GHI";
		  click4.id=button_add(&click4);
		  button_show(click4.id);


		  click5.color=~YELLOW;
		  click5.x=xx+120;
		  click5.y=yy+80;
		  click5.wrap=0;
		  click5.used=1;
		  click5.width=digi_w;
		  click5.height=digi_h;
		  click5.text="5 JKL";
		  click5.id=button_add(&click5);
		  button_show(click5.id);


		  click6.color=~YELLOW;
		  click6.x=xx+190;
		  click6.y=yy+80;
		  click6.wrap=0;
		  click6.used=1;
		  click6.width=digi_w;
		  click6.height=digi_h;
		  click6.text="6 MNO";
		  click6.id=button_add(&click6);
		  button_show(click6.id);


		  click7.color=~YELLOW;
		  click7.x=xx+50;
		  click7.y=yy+130;
		  click7.wrap=0;
		  click7.used=1;
		  click7.width=digi_w;
		  click7.height=digi_h;
		  click7.text="7 PQRS";
		  click7.id=button_add(&click7);
		  button_show(click7.id);

		  click8.color=~YELLOW;
		  click8.x=xx+120;
		  click8.y=yy+130;
		  click8.wrap=0;
		  click8.used=1;
		  click8.width=digi_w;
		  click8.height=digi_h;
		  click8.text="8 TUV";
		  click8.id=button_add(&click8);
		  button_show(click8.id);

		  click9.color=~YELLOW;
		  click9.x=xx+190;
		  click9.y=yy+130;
		  click9.wrap=0;
		  click9.used=1;
		  click9.width=digi_w;
		  click9.height=digi_h;
		  click9.text="9 WXYZ";
		  click9.id=button_add(&click9);
		  button_show(click9.id);

		  click10.color=~YELLOW;
		  click10.x=xx+120;
		  click10.y=yy+175;
		  click10.wrap=0;
		  click10.used=1;
		  click10.width=200;
		  click10.height=digi_h-10;
		  click10.text="SPACE";
		  click10.id=button_add(&click10);
		  button_show(click10.id);
}
char* keyboard(void)
{
	touch pressed;

	int cnt1;

	Button click1;
	Button click2;
	Button click3;
	Button click4;
	Button click5;
	Button click6;
	Button click7;
	Button click8;
	Button click9;
	Button click10;
	Button delete;
	Button enter;

	  button_ini();

	  u8 znak=0;
	  int col=-1;
	  u8 last=250;
	  u8 ok=0;
	  u8 coll=0;
	  u8 row=0;
//	  cnt1=0;
	  u8 but=250;
	  char text[40]={0};

	  while(but != enter.id)
	  {
			 but=read_touch_but(&pressed);

//			  coll=col;

			  if(but==click1.id)
			  {
				  	if(last!=click1.id )
					{
						  znak=0;
//						  text[col]='1';
						  col++;
					}

				  if(znak==0)text[col]='.';
				  else if (znak==1)text[col]=',';
				  else if (znak==2)text[col]='!';
				  if(znak<3)cnt1=0;
				  znak++;
				  if(znak==3)znak=0;
				  last=click1.id;
			  }
			  else if(but==click2.id)
			  {
				  	if(last!=click2.id)
					{
						  znak=0;
//						  text[col]='1';
						  col++;
					}

				  if(znak==0)text[col]='a';
				  else if (znak==1)text[col]='b';
				  else if (znak==2)text[col]='c';
				  if(znak<3)cnt1=0;
				  znak++;
				  if(znak==3)znak=0;
				  last=click2.id;
			  }
			  else if(but==click3.id)
			  {
				  	if(last!=click3.id)
					{
						  znak=0;
//						  text[col]='1';
						  col++;
					}

				  if(znak==0)text[col]='d';
				  else if (znak==1)text[col]='e';
				  else if (znak==2)text[col]='f';
				  if(znak<3)cnt1=0;
				  znak++;
				  if(znak==3)znak=0;
				  last=click3.id;
			  }
			  else if(but==click4.id)
			  {
				  	if(last!=click4.id)
					{
						  znak=0;
//						  text[col]='1';
						  col++;
					}

				  if(znak==0)text[col]='g';
				  else if (znak==1)text[col]='h';
				  else if (znak==2)text[col]='i';
				  if(znak<3)cnt1=0;
				  znak++;
				  if(znak==3)znak=0;
				  last=click4.id;

			  }
			  else if(but==click5.id)
			  {
				  	if(last!=click5.id)
					{
						  znak=0;
//						  text[col]='1';
						  col++;
					}

				  if(znak==0)text[col]='j';
				  else if (znak==1)text[col]='k';
				  else if (znak==2)text[col]='l';
				  if(znak<3)cnt1=0;
				  znak++;
				  if(znak==3)znak=0;
				  last=click5.id;
			  }
			  else if(but==click6.id)
			  {
				  	if(last!=click6.id)
					{
						  znak=0;
//						  text[col]='1';
						  col++;
					}
				  if(znak==0)text[col]='m';
				  else if (znak==1)text[col]='n';
				  else if (znak==2)text[col]='o';
				  if(znak<3)cnt1=0;
				  znak++;
				  if(znak==3)znak=0;
				  last=click6.id;
			  }
			  else if(but==click7.id)
			  {
				  	if(last!=click7.id)
					{
						  znak=0;
//						  text[col]='1';
						  col++;
					}

				  if(znak==0)text[col]='p';
				  else if (znak==1)text[col]='q';
				  else if (znak==2)text[col]='r';
				  else if (znak==3)text[col]='s';

				  if(znak<4)cnt1=0;
				  znak++;
				  if(znak==4)znak=0;
				  last=click7.id;
			  }
			  else if(but==click8.id)
			  {
				  	if(last!=click8.id)
					{
						  znak=0;
//						  text[col]='1';
						  col++;
					}

				  if(znak==0)text[col]='t';
				  else if (znak==1)text[col]='u';
				  else if (znak==2)text[col]='v';
				  if(znak<3)cnt1=0;
				  znak++;
				  if(znak==3)znak=0;
				  last=click8.id;
			  }
			  else if(but==click9.id)
			  {
				  	if(last!=click9.id)
					{
						  znak=0;
//						  text[col]='1';
						  col++;
					}
				  if(znak==0)text[col]='w';
				  else if (znak==1)text[col]='x';
				  else if (znak==2)text[col]='y';
				  else if (znak==3)text[col]='z';
				  if(znak<4)cnt1=0;
				  znak++;
				  if(znak==4)znak=0;
				  last=click9.id;
			  }
			  else if(but==click10.id)
			  {
				  if(last!=click10.id)
				  {
//					  text[col]='1';
					   col++;
				  }
				  text[col]='1';
				  last=click10.id;
			  }
			  else if (but==delete.id)
			  {
				  text[col]=0;
				  if(col>-1)col--;
				  last=delete.id;
				  LCD_String_lc("                 ",1,0,BLACK,BLACK,2);
			  }

			  if((last!=250 && cnt1>=100000))
			  {
				  znak=0;
				  last=250;
				  cnt1=0;
			  }
			  cnt1++;

//			  if(but==enter.id)
			  LCD_String_lc(text,1,0,GREEN,~GREEN,2);
	  }

	  button_destroy(&enter);
	  button_destroy(&delete);
	  button_destroy(&click1);
	  button_destroy(&click2);
	  button_destroy(&click3);
	  button_destroy(&click4);
	  button_destroy(&click5);
	  button_destroy(&click6);
	  button_destroy(&click7);
	  button_destroy(&click8);
	  button_destroy(&click9);
	  button_destroy(&click10);

	  return text;

}

void vApplicationTickHook(void)
{
//	cnt1++;
}

