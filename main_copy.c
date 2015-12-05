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
#include "list.h"
#include "textbox.h"
////////LE////////////////////
#include "stm32f4xx_pwr.h"
////////EXPLORER///////////
#include "explorer.h"
////////graphic////////////
#include "bsp.h"
#include "win.h"
#include "GUIDRV_Template.h"


//extern void BSP_Background (void);
extern __IO int32_t OS_TimeMS;


static TCHAR lfname[_MAX_LFN];

#define digi_w  60
#define digi_h  40
//typedef enum { false, true }bool;

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_Core __ALIGN_END;

volatile uint8_t usb_host_connected_flag = 0;
volatile uint8_t usb_host_disconnected_flag = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define SLAVE_ADDRESS 0x3c
#define BUFF  	18432

#define PLL_Q	7			/* Dzielnik dla USB, SDIO, RND - 48MHz */
#define PLL_M	8
#define PLL_N	336     	/* Glowny mnoznik - 336 MHz */
//#define PLL_N	360
#define PLL_P	2			/* Dzielnik przed VCO - 2 MHz */

#define line_2  6
//#define BUF  	512


#define LED_ON GPIOD->BSRRL|=GPIO_BSRR_BS_6
#define LED_OFF GPIOD->BSRRH|=GPIO_BSRR_BS_6

#define button  (!((GPIOE->IDR) & GPIO_IDR_IDR_4))
//#define

void EXTI9_5_IRQHandler(void);
void RCC_cfg (void);
void RCC_Config (void);
void GPIO_cfg(void);
void NVIC_cfg(void);
void delay(unsigned int ms);
void FSMC_init(void);
void GPIO_Config(void);
void SPI_Config(void);
void DMA_Config(int ele, u8* buf);
u8 open_dir(char *path);
void check_ext(char *f, u8 il);
void dma_start(void);
void bitmap_RGB(char *sc , u16 x, u16 y, u16 lx, u16 ly);
void icon(u8 wh, u16 x, u16 y, u16 lx, u16 ly);
void bitmap_BMP(char *sc , int x, int y);
void bitmap_BMP_mov(char *sc , int x, int y, int lasx, int lasy);
void player (void);
void TIM6_DAC_IRQHandler(void) __attribute__((interrupt));
void wav_interrupt(void);
void IR_config(void);
void exti_init(void);
char* keyboard(char *up);
void icon_trsp(touch *pressed,char *sc, int lax, int lay);
void icon_m(u8 wh, u16 x, u16 y, u16 lx, u16 ly);

void alpha(u16 x, u16 y, u16 x1, u16 y1, float alpha);
UINT tjd_input (JDEC* jd,BYTE* buff,UINT nd);
UINT tjd_output (JDEC* jd,void* bitmap,JRECT* rect);

uint8_t I2C_read_nack(I2C_TypeDef* I2Cx);
BYTE* wsk;

volatile u8 bsp_cfg=0;
volatile int bsp=0;
volatile int bksp=0;

int pixonn=1;
////////////////////////////////////////////////////////////////////
char butstr[9][7]=
{
		{'1',' ','.',',','!'},
		{'2',' ','a','b','c'},
		{'3',' ','d','e','f'},
		{'4',' ','g','h','i'},
		{'5',' ','j','k','l'},
		{'6',' ','m','n','o'},
		{'7',' ','p','q','r','s'},
		{'8',' ','t','u','v'},
		{'9',' ','w','x','y','z'}
};

uint8_t zmienna=0;
volatile int cnt1=0,go=0;
char t[10];
volatile int flag=0, pr;
//int BUF=18432;
int BUF=1;
int indexx=0;
BYTE Buff[4096] __attribute__ ((aligned(4)));
//BYTE Buff[1] __attribute__ ((aligned(4)));
DIR dir;
FILINFO fno;
FATFS fs;
FIL fsrc;
FRESULT fres;
uint8_t  bufor[BUFF]={0};///BUFFem
uint8_t  width[4];
uint8_t  height[4];
uint8_t  off[4];
uint32_t s1;

int i=0,juz=0;
uint8_t dan[5];
void backlight(void);
int framesize;
int movilocation;
int framelocation;
int nobytsread;
//uint8_t pagebuff[23400];       //buffer to store frame
uint8_t pagebuff[1];       //buffer to store frame

//char fil_path[100];

//  MY         MX            MV            ML          RGB        1    1    0
//mirror y   mirror x   x y exchange  	scan order    format

//00001110   0x0E BGR colours vertical
//00000110   0x06 RGB colours vertical
//10001110   0x86 GBR colours vertical upside down
//10000110	 0x8E RGB colours vertical upside down
//01101110   0x6E BGR colours horizontal
//01100110   0x66 RGB colours horizontal
//11101110   0xEE GBR colours horizontal upside down
//11100110	 0xE6 RGB colours horizontal upside down



//char t[10]={0};
volatile u8 check=0;

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
Button spec;
Button upp;



//char fd[200]={0};

int main(void)
{
//	SystemInit();

	RCC_cfg();

	SysTick_Config(1680);

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


//	 GPIO_InitTypeDef GPIO_InitStructure;
//	   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
//	   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//	   GPIO_Init(GPIOA, &GPIO_InitStructure);
//	   GPIO_Init(GPIOB, &GPIO_InitStructure);
//	   GPIO_Init(GPIOC, &GPIO_InitStructure);
//	   GPIO_Init(GPIOD, &GPIO_InitStructure);
//	   GPIO_Init(GPIOE, &GPIO_InitStructure);


//	   PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
//
	 touch pressed;

	  NVIC_Config();
	  GPIO_cfg();
	  backlight();
	  FSMC_NAND_Init();
	  LCD_init();
	  LCD_paint(0xffff00);
	  LCD_WRITE_COMMAND=(MADCTR);
	  LCD_WRITE_DATA = (0x66);

	  LED_OFF;
	  f_mount(&fs,"",0);

	  i2c_ini();
	  stmpe811_init();
//	  exti_init();

	  if(button)
	  {
		  USBD_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
		  while(1);
	  }


	   GUI_Init();
	   bsp_cfg=1;
	   GUI_CURSOR_Show();
////	   bitmap_BMP("moje/jas.bmp",0,0);
//	   GUI_SetTextMode(GUI_TM_TRANS);
//	   GUI_SetFont(GUI_FONT_COMIC24B_ASCII);
//	   BUTTON_Handle hButton;
//	   hButton=BUTTON_Create(50,50,90,70,1,WM_CF_SHOW);
//	   BUTTON_SetText(hButton,"DZIALA");
//	   BUTTON_SetSkin(hButton,BUTTON_SKIN_FLEX);
//
//	   CreateFramewin();

	   WM_Exec();

	   fres=f_open(&fsrc, "0:moje/jas.bmp" , FA_READ| FA_OPEN_EXISTING); // open file
	   GUI_BMP_DrawEx(APP_GetData, &fsrc,0,0); // Draw image
//	   GUI_JPEG_DrawEx(APP_GetData, &fsrc,0,0);

//	   fres=f_open(&fsrc, "0:moje/car1.jpg" , FA_READ| FA_OPEN_EXISTING); // open file
//	   GUI_JPEG_DrawEx(APP_GetData, &fsrc,0,0);
//	   fres=f_close(&fsrc);
//	   delay(10000);
//
	   fres=f_open(&fsrc, "0:test.jpg" , FA_READ| FA_OPEN_EXISTING); // open file



//	   load_jpg(&fsrc,"0:moje/car1.jpg",)
//	   load_jpg(&fsrc,"0:test.jpg",Buff,sizeof(Buff));
//	   GUI_BMP_DrawEx(APP_GetData, &fsrc,0,0);
//	   	   delay(100000);
	   pixonn=0;
	   	go=0;
	   	GUI_JPEG_DrawEx(APP_GetData, &fsrc,0,0);
//	   	LCD_area(100,0,199,240);
//		DMA_Config(8192,bufer);
//		while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
//		DMA2->LISR=0x00000000;
////	   	LCD_area(100,150,199,240);
//		DMA_Config(8192,bufer);
//		while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
//		DMA2->LISR=0x00000000;
////	   	LCD_area(100,200,199,240);
//		DMA_Config(8192,bufer);
//		while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
//		DMA2->LISR=0x00000000;
		pixonn=1;
		GUI_DispDecAt(go,150,150,7);
//		int k=0;

	   while(1)
	   {

		   GUI_Exec();
//		   	LCD_area(100+k,100+k,200+k,200+k);
//			DMA_Config(4095,bufer);
//			while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
//			DMA2->LISR=0x00000000;
//			k+=2;
//			if(k+200==240)k=0;
	   };


//	  play_video(explorer());

//	  while(1);
//	  LCD_box_mid_round(254,200,70,90,RED,20,1);

//	  LCD_circle_but(50,50,30,GREEN,1);


//	  copy_rc5();

	  f_open(&fsrc,"ikony/paint.rgb", FA_READ | FA_OPEN_EXISTING );
	  f_read(&fsrc, bufor, 12288, &s1);
	  f_close(&fsrc);
//

//	  load_jpg(&fsrc,"system/f_ico.jpg",Buff,sizeof(Buff));

//	  bitmap_BMP("moje/jas.bmp",0,0);
//	  LCD_WRITE_COMMAND=(MADCTR);
//	  LCD_WRITE_DATA = (0x0E);
//	  LCD_String_lc("IT WORKS!!!",2,2,GREEN,1,3);
//	  RGBA("system/folder1.bmp",0,0);

//	  while(1)
//	  {
//		  read_touch(&pressed);
//		  NVIC_DisableIRQ(EXTI9_5_IRQn);
//		  if(pressed.x!=400)bitmap_BMP("0:ikony/ic2.bmp",pressed.x,pressed.y);
//		  NVIC_EnableIRQ(EXTI9_5_IRQn);
//	  }



	  int lastty;
	  int lasttx;
	  int lax=0;
	  int lay=0;
//
//	  bitmap_BMP("0:ikony/remote.bmp",50,50);

//	  alpha(0,0,320,240,100);
//	  alpha(0,0,100,240,250);
//	  alpha(0,0,100,30,250);
//	  icon_m(1,50,20,64,64);
//	  alpha(0,0,100,50,250);

	  while(1)
	  {


		  read_touch(&pressed);
//
		if((abs(lasttx-pressed.x)>2 || abs(lastty-pressed.y)>2) && pressed.x!=400 && pressed.y!=400)
		{
//			LCD_paint(0x000000);
			pressed.y=50;
			NVIC_DisableIRQ(EXTI9_5_IRQn);
//			bitmap_BMP("0:ikony/remote.bmp",pressed.x,pressed.y);

//			bitmap_BMP_mov("0:ikony/ic2.bmp",lax,lay,pressed.x,pressed.y);

			icon_m(1,pressed.x,pressed.y,64,64);

//			bitmap_BMP_mov("0:ikony/explorer.bmp",lax-70,lay,pressed.x-70,pressed.y);
//			bitmap_BMP_mov("0:ikony/remote.bmp",lax,lay,pressed.x,pressed.y);
//			bitmap_BMP_mov("0:ikony/paint.bmp",lax+70,lay,pressed.x+70,pressed.y);
			NVIC_EnableIRQ(EXTI9_5_IRQn);

			alpha(0,0,100,50,250);
//			LCD_String("PIOTREK",pressed.x,30,2,GREEN,1,1);

//		  LCD_String_lc(itoa(pressed.x,t,10),5,5,RED,BLUE,1);
//		  LCD_String_lc(itoa(pressed.y,t,10),15,5,RED,BLUE,1);
			lax=pressed.x;
			lay=pressed.y;
		}
//		else if( pressed.y==400) LCD_paint(0x0000ff);
//
		lasttx=pressed.x;
		lastty=pressed.y;



	  }

//	  {
		  LCD_paint(0x0000ff);
		  copy_rc5();
//	  explorer();
//	  }
}

//void icon_trsp(touch *pressed,char *sc, int lax, int lay)
//{
//	LCD_box(lax,lay,lax+50,lay+abs(lay-pressed->y),BLACK,1);
//	LCD_box(lax,lay,lax+abs(lax-pressed->x),lay+50,BLACK,1);
//	LCD_box(lax,lay+50,lax+50,lay+50-abs(lay-pressed->y),BLACK,1);
//	LCD_box(lax+50,lay,lax+50-abs(lax-pressed->x),lay+50,BLACK,1);
//	bitmap_BMP(sc,pressed->x,pressed->y);
//}

void alpha(u16 x, u16 y, u16 x1, u16 y1, float alpha)
{
#define F_INV_255 (1.0f / 255.0f)

	u8 R,B,G;
	u16 x2=x,y2=y;
	float as=alpha;

	while(y2!=y1)
	{
			float alpha_ratio=alpha*F_INV_255;

		  u8 RR=0x00,BB=0x00,GG=0x00;

		  LCD_area(x2,y2,x2,y2);
		  LCD_WRITE_COMMAND=(RAMRD);
//		  int k=LCD_WRITE_DATA;
//		  	  delay(1);
		  RR=LCD_WRITE_DATA;
//			  delay(1);
		  GG=LCD_WRITE_DATA;
//			  delay(1);
		  BB=LCD_WRITE_DATA;
//			  delay(1);

		  u8 R=0x00;
		  u8 G=0x00;
		  u8 B=0x00;

		  R *= alpha_ratio;
		  G *= alpha_ratio;
		  B *= alpha_ratio;
//
		  RR *= (1.0f - alpha_ratio);
		  GG *= (1.0f - alpha_ratio);
		  BB *= (1.0f - alpha_ratio);

		  G += BB;
		  R += RR;
		  B += GG;
////
		  LCD_area(x2,y2,x2,y2);
		  LCD_WRITE_DATA=R;
		  LCD_WRITE_DATA=B;
		  LCD_WRITE_DATA=G;

		  x2++;
		  alpha--;

		  if(x2==x1)
		  {
			  x2=x;
			  alpha=as;
			  y2++;
		  }

	}

}

void bitmap_BMP(char *sc , int x, int y)
{
	BUF=4096;
//	  LCD_WRITE_COMMAND=(MADCTR);
//	  LCD_WRITE_DATA = (0xdf);

	  fres = f_open(&fsrc,sc, FA_READ | FA_OPEN_EXISTING );
	  long int read=f_size(&fsrc) - 54;

	  f_lseek(&fsrc, 18);

	  fres = f_read(&fsrc, width, 4, &s1);
	  fres = f_read(&fsrc, height, 4, &s1);
	  int wi = (width[0]+width[1]*256);
	  int hei = (height[0]+height[1]*256);

//	  if(wi>240 && hei<320)
//	  {
//		  LCD_WRITE_COMMAND=(MADCTR);
//		  LCD_WRITE_DATA = (0x2f);
//		  delay(20);
//		  LCD_area(x,y,x+wi-1,y+hei-1);
//	  }
//	  else
	  LCD_area(x,y,x+wi-1,y+hei-1);

	  fres=f_lseek(&fsrc, 54);

	  while(read>0)
	  {
		  fres = f_read(&fsrc, bufor, BUF, &s1);
		  DMA_Config(BUF,bufor);
		  while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
		  DMA2->LISR=0x00000000;
		  read-=BUF;
		  if(read-BUF<0)BUF=read;
	  }
}

void icon_m(u8 wh, u16 x, u16 y, u16 lx, u16 ly)
{
//	lx/=2;
//	ly/=2;
	int BUF=lx*ly*3;
	LCD_area(x-(lx/2),y-(ly/2),x+(lx/2)-1,y+ly+(ly/2)-1);
	int row=ly,i=0;

	while(row--)
	{
		DMA_Config(ly*3,&bufor[i]);
		while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
		DMA2->LISR=0x00000000;
		i+=lx*3;
	}




}

void bitmap_BMP_mov(char *sc , int x, int y, int lasx, int lasy)
{
	  BUF=4096;

	  fres = f_open(&fsrc,sc, FA_READ | FA_OPEN_EXISTING );
	  long int read=f_size(&fsrc)-54;

	  f_lseek(&fsrc, 18);

	  fres = f_read(&fsrc, width, 4, &s1);
	  fres = f_read(&fsrc, height, 4, &s1);
//
	  int wi = (width[0]+width[1]*256);
	  int hei =(height[0]+height[1]*256);

//		LCD_box(x,y,x+wi,y+abs(y-lasy),RED,1);
//		LCD_box(x,y,x+abs(x-lasx),y+hei,GREEN,1);
//		LCD_box(x,y+hei,x+wi,y+hei-abs(y-lasy),BLUE,1);
//		LCD_box(x+wi,y,x+wi-abs(x-lasx),y+hei,YELLOW,1);
		LCD_box(x,y,x+wi,y+abs(y-lasy),BLACK,1);
		LCD_box(x,y,x+abs(x-lasx),y+hei,BLACK,1);
		LCD_box(x,y+hei,x+wi,y+hei-abs(y-lasy),BLACK,1);
		LCD_box(x+wi,y,x+wi-abs(x-lasx),y+hei,BLACK,1);
//		long int read = wi*hei*3;

		x=lasx;
		y=lasy;

//	  LCD_String_lc(itoa(read,t,10),5,5,RED,BLUE,1);
//	  LCD_String_lc(itoa(f_size(&fsrc),t,10),15,5,RED,BLUE,1);
	  LCD_area(x,y,x+wi-1,y+hei-1);

	  fres=f_lseek(&fsrc, 54);

	  while(read>0)
	  {
		  fres = f_read(&fsrc, bufor, BUF, &s1);

		  DMA_Config(BUF,bufor);
		  while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
		  DMA2->LISR=0x00000000;

		  read-=BUF;
		  if(read-BUF<0)BUF=read;
	  }
}

void bitmap_RGB(char *sc , u16 x, u16 y, u16 lx, u16 ly)
{

//	  unsigned int read= f_size(&fsrc);
//	  BUF=read;
	  BUF=2187;
	  LCD_area(x,y,x+lx-1,y+ly-1);
//	  NVIC_DisableIRQ(EXTI9_5_IRQn);
//	  fres = f_open(&fsrc,sc, FA_READ | FA_OPEN_EXISTING );
//	  while(read>0)
//	  {
//		  fres = f_read(&fsrc, bufor, BUF, &s1);
//		  NVIC_EnableIRQ(EXTI9_5_IRQn);
		  DMA_Config(BUF,bufor);
		  while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
		  DMA2->LISR=0x00000000;
//		  read-=BUF;
//		  if(read-BUF<0)BUF=read;
//	  }
}

void RGBA(char *sc, int x, int y)
{

#define F_INV_255 (1.0f / 256.0f)

	  BUF=2048;
	  u8 buf[2048];
	  fres = f_open(&fsrc,sc, FA_READ | FA_OPEN_EXISTING );
	  long int read=f_size(&fsrc) - 54;
//
	  f_lseek(&fsrc,14);
	  fres = f_read(&fsrc, width, 4, &s1);
	  fres = f_lseek(&fsrc,(width[0]+width[1]*256)+1);//+5

	  int a=(width[0]+width[1]*256)+1;

	  int r=0,gx=x,gy=y;

	  int xk=a+x,xp=x;

	  int f=0;
	  u8 j=10;

	  while(j--)
	  {
		  fres = f_read(&fsrc, buf, BUF, &s1);
		  s1/=4;

		  while(s1>0)
		  {

			  u8 RR=0x00,BB=0x00,GG=0x00;

			  LCD_area(gx,gy,gx,gy);
			  LCD_WRITE_COMMAND=(RAMRD);
			  int k=LCD_WRITE_DATA;
//			  k=LCD_WRITE_DATA;
			  RR=LCD_WRITE_DATA;
//			  delay(1);
			  GG=LCD_WRITE_DATA;
//			  delay(1);
			  BB=LCD_WRITE_DATA;
//			  delay(1);
//
			  u8 R=buf[f+1];
			  u8 G=buf[f+2];
			  u8 B=buf[f+3];
			  float alpha_ratio=buf[f]*F_INV_255;
//
			  R *= alpha_ratio;
			  G *= alpha_ratio;
			  B *= alpha_ratio;

			  RR *= (1.0f - alpha_ratio);
			  GG *= (1.0f - alpha_ratio);
			  BB *= (1.0f - alpha_ratio);

			  R += GG;
			  G += RR;
			  B += BB;
////
			  gx++;

			  if(gx==xk)
			  {
				  gx=xp;
				  gy++;
			  }
////
			  LCD_area(x,y,x,y);
			  LCD_WRITE_DATA=R;
			  delay(1);
			  LCD_WRITE_DATA=B;
			  delay(1);
			  LCD_WRITE_DATA=G;
			  delay(1);
//
			  x++;
			  if(x==xk)
			  {
				  x=xp;
				  y++;
			  }
//
			  f+=4;
			  s1--;
		  }

		  read=read-BUF;
//		  if(read-BUF<0)BUF=read;
	  }
}



void exti_init(void)
{
	EXTI->IMR= EXTI_IMR_MR5;
//	EXTI->FTSR=EXTI_FTSR_TR5;
	EXTI->RTSR = EXTI_RTSR_TR5;
    NVIC_SetPriority(EXTI9_5_IRQn,7 );
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    SYSCFG ->EXTICR[1] = SYSCFG_EXTICR2_EXTI5_PC;
}
void button_ini(void)
{

#define kcol ~0xff0000

		u8 xx=40;
		u8 yy=45;

		  enter.color=~GREEN;
		  enter.x=xx-12;
		  enter.y=yy+50;
		  enter.wrap=0;
		  enter.used=1;
		  enter.width=digi_w-15;
		  enter.height=80;
		  enter.text="ENTER";
		  enter.id=button_add(&enter);

		  delete.color=~RED;
		  delete.x=xx+252;
		  delete.y=yy+50;
		  delete.wrap=0;
		  delete.used=1;
		  delete.width=digi_w-15;
		  delete.height=80;
		  delete.text="DEL";
		  delete.id=button_add(&delete);

		  upp.color=kcol;
		  upp.x=xx-12;
		  upp.y=yy+150;
		  upp.wrap=0;
		  upp.used=1;
		  upp.width=digi_w-15;
		  upp.height=80;
		  upp.text=" ^ ";
		  upp.id=button_add(&upp);

		  spec.color=kcol;
		  spec.x=xx+252;
		  spec.y=yy+150;
		  spec.wrap=0;
		  spec.used=1;
		  spec.width=digi_w-15;
		  spec.height=80;
		  spec.text="@#$%";
		  spec.id=button_add(&spec);

		  click1.color=kcol;
		  click1.x=xx+50;
		  click1.y=yy+30;
		  click1.wrap=0;
		  click1.used=1;
		  click1.width=digi_w;
		  click1.height=digi_h;
		  click1.text=butstr[0];
		  click1.id=button_add(&click1);

		  click2.color=kcol;
		  click2.x=xx+120;
		  click2.y=yy+30;
		  click2.wrap=0;
		  click2.used=1;
		  click2.width=digi_w;
		  click2.height=digi_h;
		  click2.text=butstr[1];
		  click2.id=button_add(&click2);

		  click3.color=kcol;
		  click3.x=xx+190;
		  click3.y=yy+30;
		  click3.wrap=0;
		  click3.used=1;
		  click3.width=digi_w;
		  click3.height=digi_h;
		  click3.text=butstr[2];
		  click3.id=button_add(&click3);

		  click4.color=kcol;
		  click4.x=xx+50;
		  click4.y=yy+80;
		  click4.wrap=0;
		  click4.used=1;
		  click4.width=digi_w;
		  click4.height=digi_h;
		  click4.text=butstr[3];
		  click4.id=button_add(&click4);

		  click5.color=kcol;
		  click5.x=xx+120;
		  click5.y=yy+80;
		  click5.wrap=0;
		  click5.used=1;
		  click5.width=digi_w;
		  click5.height=digi_h;
		  click5.text=butstr[4];
		  click5.id=button_add(&click5);

		  click6.color=kcol;
		  click6.x=xx+190;
		  click6.y=yy+80;
		  click6.wrap=0;
		  click6.used=1;
		  click6.width=digi_w;
		  click6.height=digi_h;
		  click6.text=butstr[5];
		  click6.id=button_add(&click6);

		  click7.color=kcol;
		  click7.x=xx+50;
		  click7.y=yy+130;
		  click7.wrap=0;
		  click7.used=1;
		  click7.width=digi_w;
		  click7.height=digi_h;
		  click7.text=butstr[6];
		  click7.id=button_add(&click7);

		  click8.color=kcol;
		  click8.x=xx+120;
		  click8.y=yy+130;
		  click8.wrap=0;
		  click8.used=1;
		  click8.width=digi_w;
		  click8.height=digi_h;
		  click8.text=butstr[7];
		  click8.id=button_add(&click8);

		  click9.color=kcol;
		  click9.x=xx+190;
		  click9.y=yy+130;
		  click9.wrap=0;
		  click9.used=1;
		  click9.width=digi_w;
		  click9.height=digi_h;
		  click9.text=butstr[8];
		  click9.id=button_add(&click9);

		  click10.color=kcol;
		  click10.x=xx+120;
		  click10.y=yy+175;
		  click10.wrap=0;
		  click10.used=1;
		  click10.width=200;
		  click10.height=digi_h-10;
		  click10.text="SPACE";
		  click10.id=button_add(&click10);


		  button_show(click10.id);
		  button_show(click9.id);
		  button_show(click8.id);
		  button_show(click7.id);
		  button_show(click6.id);
		  button_show(click5.id);
		  button_show(click4.id);
		  button_show(click3.id);
		  button_show(click2.id);
		  button_show(click1.id);
		  button_show(spec.id);
		  button_show(upp.id);
		  button_show(delete.id);
		  button_show(enter.id);

}
void copy_rc5(void)
{

	int tm[251]={0};
	int z=0;
	touch pressed;

	  char *wsk_wr;
	  char *wsk_rd;

	  *(wsk_wr+2)=0;
	  *(wsk_rd+2)=0;

	  Button send;
	  Button sample;
	  send.color=~RED;
	  send.x=80;
	  send.y=140;
	  send.wrap=0;
	  send.used=1;
	  send.on=1;
	  send.width=120;
	  send.height=60;
	  send.text="CLICK TO SEND";
	  send.id=button_add(&send);
	  button_show(send.id);
//
	  sample.color=~YELLOW;
	  sample.x=220;
	  sample.y=140;
	  sample.wrap=0;
	  sample.used=1;
	  sample.width=120;
	  sample.on=1;
	  sample.height=60;
	  sample.text="CLICK TO SAMPLE";
	  sample.id=button_add(&sample);
	  button_show(sample.id);

	  Textbox path_wr;
	  Textbox path_rd;

	  path_wr.color=0x000000;
	  path_wr.colorrev=0xffff00;
	  path_wr.height=30;
	  path_wr.width=220;
	  path_wr.used=1;
	  path_wr.vis=30;
	  path_wr.text="select_path";
	  path_wr.x=200;
	  path_wr.y=60;
	  path_wr.on=1;
	  path_wr.id=textbox_add(&path_wr);
	  textbox_show(path_wr.id);

	  path_rd.color=0x000000;
	  path_rd.colorrev=0xffff00;
	  path_rd.height=30;
	  path_rd.width=220;
	  path_rd.used=1;
	  path_rd.vis=30;
	  path_rd.text="select_path";
	  path_rd.x=200;
	  path_rd.y=30;
	  path_rd.on=1;
	  path_rd.id=textbox_add(&path_rd);
	  textbox_show(path_rd.id);

	  while(1)
	  {
		  u8 idd=read_touch_but(&pressed);
		  u8 itb=read_touch_textbox(&pressed);

			if(idd==sample.id)
			{
				LCD_String_lc("sampling...",3,6,RED,~RED,3);

				i=0;
				cnt1=0;

				while(1)
				{
					if(!((GPIOB->IDR) & GPIO_IDR_IDR_14))
					{
						tm[i]=cnt1;
						cnt1=0;
						i++;
						while(!((GPIOB->IDR) & GPIO_IDR_IDR_14)){};

					}
					else if((GPIOB->IDR) & GPIO_IDR_IDR_14)
					{
						tm[i]=cnt1;
						cnt1=0;
						i++;
						while((GPIOB->IDR) & GPIO_IDR_IDR_14){};
					}

					if(i==250)
					{
						LCD_String_lc("    DONE!   ",3,6,RED,~RED,3);
						break;
					}
				}

				if(*(wsk_wr+2)>0)
				{
					textbox_text(path_wr.id,wsk_wr);
					textbox_text(path_rd.id,wsk_rd);
					button_show(sample.id);
					button_show(send.id);
					fres=f_open(&fsrc,wsk_wr,FA_WRITE);
					f_write(&fsrc,tm,250,&s1);
					f_close(&fsrc);
				}
				else
				{
					while(pressed.hold_but)read_touch_but(&pressed);
					textbox_on(path_rd.id, 0);
					textbox_on(path_wr.id, 0);
					button_on(250, 0);
					wsk_wr=explorer();
					textbox_on(path_rd.id, 1);
					textbox_on(path_wr.id, 1);
					LCD_paint(0x000000);
					textbox_text(path_wr.id,wsk_wr);
					textbox_text(path_rd.id,wsk_rd);
					button_show(sample.id);
					button_show(send.id);
					fres=f_open(&fsrc,wsk_wr,FA_WRITE);
					f_write(&fsrc,tm,250,&s1);
					f_close(&fsrc);
				}
			}
			else if(idd==send.id)
			{
				i=0;
				RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
				IR_config();

				if(*(wsk_rd+2)>0)
				{
					textbox_text(path_rd.id,wsk_rd);
					textbox_text(path_wr.id,wsk_wr);
					button_show(sample.id);
					button_show(send.id);
					fres=f_open(&fsrc,wsk_rd,FA_OPEN_EXISTING|FA_READ);
					f_read(&fsrc,tm,250,&s1);
					f_close(&fsrc);
				}
				else
				{
			  		while(pressed.hold_but)read_touch_but(&pressed);
					textbox_on(path_rd.id, 0);
					textbox_on(path_wr.id, 0);
					button_on(250, 0);
					wsk_rd=explorer();
					textbox_on(path_rd.id, 1);
					textbox_on(path_wr.id, 1);
					LCD_paint(0x000000);
					button_show(sample.id);
					button_show(send.id);
					textbox_text(path_rd.id,wsk_rd);
					textbox_text(path_wr.id,wsk_wr);
					fres=f_open(&fsrc,wsk_rd,FA_OPEN_EXISTING|FA_READ);
					f_read(&fsrc,tm,250,&s1);
					f_close(&fsrc);
				}

				while(i<250)
				{
						TIM4->CR1|= TIM_CR1_CEN;
						TIM4->CNT=1130;
						delay(tm[i++]);
						TIM4->CR1&=~TIM_CR1_CEN;
						TIM4->CNT=1130;
						delay(tm[i++]);
				}

			}
			else if(itb==path_rd.id)
			{
//				while(pressed.hold_tb)read_touch_textbox(&pressed);
				while(read_touch_textbox(&pressed)==path_rd.id){};
				textbox_on(path_rd.id, 0);
				textbox_on(path_wr.id, 0);
				button_on(250, 0);
				wsk_rd=explorer();
				textbox_on(path_rd.id, 1);
				textbox_on(path_wr.id, 1);
				LCD_paint(0x000000);
				button_show(sample.id);
				button_show(send.id);
				textbox_text(path_rd.id,wsk_rd);
				textbox_text(path_wr.id,wsk_wr);
			}
			else if(itb==path_wr.id)
			{
				while(read_touch_textbox(&pressed)==path_wr.id){};
				textbox_on(path_rd.id, 0);
				textbox_on(path_wr.id, 0);
				button_on(250, 0);
				wsk_wr=explorer();
				textbox_on(path_rd.id, 1);
				textbox_on(path_wr.id, 1);
				LCD_paint(0x000000);
				button_show(sample.id);
				button_show(send.id);
				textbox_text(path_rd.id,wsk_rd);
				textbox_text(path_wr.id,wsk_wr);
			}

		 i=0;

		 if(button)
		 {
			  USBD_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
			  while(1){};
		 }
	  }
}


void i2c_ini(void)
{
    I2C_InitTypeDef i2c_init;
    I2C_DeInit(I2C2 );       //Deinit and reset the I2C to avoid it locking up
    I2C_SoftwareResetCmd(I2C2, ENABLE);
    int var;
    for ( var= 0; var < 0xffff; ++var) {};
    I2C_SoftwareResetCmd(I2C2, DISABLE);

    i2c_init.I2C_ClockSpeed = 400000;
    i2c_init.I2C_Mode = I2C_Mode_I2C;
    i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c_init.I2C_OwnAddress1 = 0x0A;
    i2c_init.I2C_Ack = I2C_Ack_Enable;
    i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &i2c_init);

    I2C_StretchClockCmd(I2C2, ENABLE);
    I2C_Cmd(I2C2, ENABLE);
}

UINT tjd_input (
	JDEC* jd,		/* Decompression object */
	BYTE* buff,		/* Pointer to the read buffer (NULL:skip) */
	UINT nd			/* Number of bytes to read/skip from input stream */
){

	UINT rb;
	if (buff)
	{
		/* Read nd bytes from the input strem */
		fres= f_read(&fsrc, buff, nd, &rb);
//		rb=nd;
//		while(nd--)
//		{
//			*buff=*wsk;
//			buff++;
//			wsk++;
//		}
		return rb;	/* Returns number of bytes could be read */

	} else
	{	/* Skip nd bytes on the input stream */
		return (f_lseek(&fsrc, f_tell(&fsrc) + nd) == FR_OK) ? nd : 0;
//		*(wsk+=nd);
//		pagebuff[0]=wsk;
//		return nd;
	}
}

/* User defined call-back function to output RGB bitmap */
UINT tjd_output (
	JDEC* jd,		/* Decompression object of current session */
	void* bitmap,	/* Bitmap data to be output */
	JRECT* rect		/* Rectangular region to output */
)
{
	jd = jd;	/* Suppress warning (device identifier is not needed in this appication) */

	LCD_area( rect->left ,rect->top,rect->right, rect->bottom);

	int yc = rect->bottom - rect->top + 1;			/* Vertical size */
	int xc = rect->right - rect->left + 1;

	DMA_Config(yc*xc*3,(uint8_t*)bitmap);

	while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
	DMA2->LISR=0x00000000;

	return 1;	/* Continue to decompression */  ////////1
}

void load_jpg (
	FIL *fp,	/* File to open */
	char *fn,
	void *work,		/* Pointer to the working buffer (must be 4-byte aligned) */
	UINT sz_work	/* Size of the working buffer (must be power of 2) */
)
{
	JDEC jd;		/* Decompression object (70 bytes) */
	JRESULT rc;
	BYTE scale=0;

	if(*fn!='-')fres = f_open(&fsrc,fn, FA_READ | FA_OPEN_EXISTING);

	rc = jd_prepare(&jd, tjd_input, (uint8_t*)work, sz_work, fp);

	if (rc == JDR_OK)
	{
		rc = jd_decomp(&jd, tjd_output, 0);
	}

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
void backlight(void)
{
	TIM5->PSC =   1;
	TIM5->ARR =   66;
//	TIM5->CCR1 =  33;
	TIM5->CCR1 =  5;
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

//	RCC->PLLCFGR = (PLL_Q << 24) | RCC_PLLCFGR_PLLSRC_HSI |
//			(((PLL_P >> 1) - 1) << 16) | (PLL_N << 6) | PLL_M;
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY))
		;

	/* PLL jak sygnal taktowania rdzenia, psk 2 dla APB2, psk 4 dla APB1 */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_SW_PLL;
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL))
		;

	/* Inicjalizacja koprocesora FPU */
	SCB->CPACR |= ((3 << 10*2)|(3 << 11*2));



//		 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
//		 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
//		 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
//		 RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
//		 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
//		 RCC->AHB3ENR  |= RCC_AHB3ENR_FSMCEN;
//		 RCC->AHB1ENR  |= RCC_AHB1ENR_DMA1EN;
//		 RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
//		 RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;


}


void NVIC_Config(void)
{
//	NVIC_EnableIRQ(DMA2_Stream5_IRQn);
}
void GPIO_cfg(void)
{

//	GPIOA->MODER=0x00000000;
//	GPIOA->OTYPER=0x00000000;
//	GPIOA->OSPEEDR=0x00000000;
//	GPIOA->PUPDR=0x00000000;
//
//	GPIOB->MODER=0x00000000;
//	GPIOB->OTYPER=0x00000000;
//	GPIOB->OSPEEDR=0x00000000;
//	GPIOB->PUPDR=0x00000000;




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

void SysTick_Handler(void)
{
	cnt1++;
	go++;
	pr++;
	stmpe();
	bsp++;
	bksp++;

	if(bsp==100)
	{
		  OS_TimeMS ++;
		  bsp=0;
	}
	if(bsp_cfg && bksp>=1000)
	{
		BSP_Pointer_Update();
		bksp=0;
	}
}
void delay(unsigned int ms)
{
	cnt1=0;
	while(cnt1<=ms){};
}

void play_video(char *sc)
{
	NVIC_DisableIRQ(EXTI9_5_IRQn);
	fres=f_open(&fsrc,sc, FA_READ | FA_OPEN_EXISTING);
	fres=f_read(&fsrc,pagebuff,4,&nobytsread);

	while(pagebuff[0]!='m' || pagebuff[1]!='o'|| pagebuff[2]!='v'|| pagebuff[3]!='i')
	{
	    	indexx++;
	        fres=f_lseek(&fsrc,indexx); // pointer forward
	    	fres=f_read(&fsrc,pagebuff,4,&nobytsread);
	 }
	       indexx+=4;
	       movilocation=indexx;       //movi tag location set
//			       indexx=fno.fsize;
	       indexx=f_size(&fsrc);
	       //point to eof
	while(pagebuff[0]!='i' || pagebuff[1]!='d'|| pagebuff[2]!='x'|| pagebuff[3]!='1')
	{
	       	  indexx--;
	          fres=f_lseek(&fsrc,indexx); // pointer forward
	       	  fres=f_read(&fsrc,pagebuff,4,&nobytsread);
	}
	while(1)
	{
		NVIC_DisableIRQ(EXTI9_5_IRQn);

	    while(pagebuff[0]!='0' || pagebuff[1]!='0'|| pagebuff[2]!='d'|| pagebuff[3]!='c')   ///dc - compressed    db-cecompressed
	    {
	       	  indexx++;
	          fres=f_lseek(&fsrc,indexx); // pointer forward
	       	  fres=f_read(&fsrc,pagebuff,4,&nobytsread);
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

//			          LCD_area(0,0,240,320);

//	          	  i=0;

//				          fres=f_read(&fsrc,pagebuff,framesize,&nobytsread);
//				          wsk=&pagebuff[0];
//						  rc=jd_prepare(&jd,pagebuff);
//						  if(!rc)rc=jd_decomp(&jd);
//          	  	  JDEC jd;
//          	  	  JRESULT rc;

	          	  load_jpg(&fsrc,"-",Buff,sizeof(Buff));


	          f_lseek(&fsrc,indexx);

	     NVIC_EnableIRQ(EXTI9_5_IRQn);
	}
}

void USBH_ConnectEventCallback(void)
{
	usb_host_connected_flag = 1;
}
void USBH_DisconnectEventCallback(void)
{
	usb_host_disconnected_flag = 1;
}

char* keyboard(char *up)
{
	  u8 upper=1;
	  button_ini();
	  touch pressed;
	  Textbox textbox;
	  textbox.color=0x000000;
	  textbox.colorrev=0xffff00;
	  textbox.height=30;
	  textbox.width=300;
	  textbox.used=1;
	  textbox.vis=35;
	  textbox.text=up;
	  textbox.x=160;
	  textbox.y=30;
	  textbox.on=1;
	  textbox.id=textbox_add(&textbox);
	  textbox_show(textbox.id);

	  u8 znak=0;
	  u8 last=250;
	  u8 ok=0;
	  u8 coll=0;
	  u8 row=0;
	  cnt1=0;
	  u8 but=250;
	  char text[40]={0};

	  while(*up!=0)
	  {
		  text[ok++]=*up++;
	  }
	  int col=ok-1;
	  ok=0;

	  while(but != enter.id)
	  {
			 but=read_touch_but(&pressed);
			 read_touch_textbox(&pressed);

//			  coll=col;

			  if(but==click1.id)
			  {
				  	if(last!=click1.id )
					{
						  znak=0;
//						  text[col]='1';
						  col++;
					}

				  if(znak==0)text[col]=*(click1.text+2);
				  else if (znak==1)text[col]=*(click1.text+3);
				  else if (znak==2)text[col]=*(click1.text+4);
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

				  if(znak==0)text[col]=*(click2.text+2);
				  else if (znak==1)text[col]=*(click2.text+3);
				  else if (znak==2)text[col]=*(click2.text+4);
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

				  if(znak==0)text[col]=*(click3.text+2);
				  else if (znak==1)text[col]=*(click3.text+3);
				  else if (znak==2)text[col]=*(click3.text+4);
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

				  if(znak==0)text[col]=*(click4.text+2);
				  else if (znak==1)text[col]=*(click4.text+3);
				  else if (znak==2)text[col]=*(click4.text+4);
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

				  if(znak==0)text[col]=*(click5.text+2);
				  else if (znak==1)text[col]=*(click5.text+3);
				  else if (znak==2)text[col]=*(click5.text+4);
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
				  if(znak==0)text[col]=*(click6.text+2);
				  else if (znak==1)text[col]=*(click6.text+3);
				  else if (znak==2)text[col]=*(click6.text+4);
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

				  if(znak==0)text[col]=*(click7.text+2);
				  else if (znak==1)text[col]=*(click7.text+3);
				  else if (znak==2)text[col]=*(click7.text+4);
				  else if (znak==3)text[col]=*(click7.text+5);

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

				  if(znak==0)text[col]=*(click8.text+2);
				  else if (znak==1)text[col]=*(click8.text+3);
				  else if (znak==2)text[col]=*(click8.text+4);
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
//				  if(znak==0)text[col]='w';
//				  else if (znak==1)text[col]='x';
//				  else if (znak==2)text[col]='y';
//				  else if (znak==3)text[col]='z';
					  if(znak==0)text[col]=*(click9.text+2);
					  else if (znak==1)text[col]=*(click9.text+3);
					  else if (znak==2)text[col]=*(click9.text+4);
					  else if (znak==3)text[col]=*(click9.text+5);
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
//				  LCD_String_lc("                 ",1,0,BLACK,BLACK,2);
			  }
			  else if(but==upp.id)
			  {
				  if(upper==0)
				  {

				  		strcpy(butstr[8],"9 WXYZ");
				  		strcpy(butstr[7],"8 TUV");
				  		strcpy(butstr[6],"7 PQRS");
				  		strcpy(butstr[5],"6 MNO");
				  		strcpy(butstr[4],"5 JKL");
				  		strcpy(butstr[3],"4 GHI");
				  		strcpy(butstr[2],"3 DEF");
				  		strcpy(butstr[1],"2 ABC");
				  		upper=1;
				  }
				  else
				  {
				  		strcpy(butstr[8],"9 wxyz");
				  		strcpy(butstr[7],"8 tuv");
				  		strcpy(butstr[6],"7 pqrs");
				  		strcpy(butstr[5],"6 mno");
				  		strcpy(butstr[4],"5 jkl");
				  		strcpy(butstr[3],"4 ghi");
				  		strcpy(butstr[2],"3 def");
				  		strcpy(butstr[1],"2 abc");
				  		upper=0;
				}

				  button_show(click9.id);
				  button_show(click8.id);
				  button_show(click7.id);
				  button_show(click6.id);
				  button_show(click5.id);
				  button_show(click4.id);
				  button_show(click3.id);
				  button_show(click2.id);

			  }

			  if((last!=250 && cnt1>=100000))
			  {
				  znak=0;
				  last=250;
				  cnt1=0;
			  }

//			  if(but==enter.id)
//			  LCD_String_lc(text,1,0,GREEN,~GREEN,2);
			  textbox_text(textbox.id,text);
	  }


	  button_destroy(&upp);
	  button_destroy(&spec);
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
	  textbox_destroy(&textbox);

	  while(pressed.hold_but)
	  {
		 read_touch_but(&pressed);
	  }

	  LCD_paint(0x000000);

	  return text;
}

//	  USBD_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
//	  while(1){};




