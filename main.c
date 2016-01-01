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
xTaskHandle                   Menu_Handle;
xTimerHandle                  TouchScreenTimer;

static void Background_Task(void * pvParameters);
static void Demo_Task(void * pvParameters);
static void vTimerCallback( xTimerHandle pxTimer );
static void MP3_player(void *pvParameters);
static void Menu(void *pvParameters);




#define RECOMMENDED_MEMORY (1024L * 200)


/*********************************************************************
*
*       _bmBatteryEmpty_27x14
*/
static GUI_CONST_STORAGE GUI_COLOR ColorsBatteryEmpty_27x14[] = {
     0x0000FF,0xFFFFFF,0xD7A50F,0xF0D26C,0x000000
};

static GUI_CONST_STORAGE GUI_LOGPALETTE PalBatteryEmpty_27x14 = {
  5,	// number of entries
  1, 	// Has transparency
  &ColorsBatteryEmpty_27x14[0]
};

static GUI_CONST_STORAGE unsigned char acBatteryEmpty_27x14[] = {
  0x00, 0x04, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x00,
  0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40,
  0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
  0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
  0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
  0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
  0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
  0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
  0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
  0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
  0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
  0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40,
  0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40,
  0x00, 0x04, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x00
};

static GUI_CONST_STORAGE GUI_BITMAP _bmBatteryEmpty_27x14 = {
  27, // XSize
  14, // YSize
  14, // BytesPerLine
  4, // BitsPerPixel
  acBatteryEmpty_27x14,  // Pointer to picture data (indices)
  &PalBatteryEmpty_27x14   // Pointer to palette
};

/*********************************************************************
*
*       bmChargeX_27x14
*/
static GUI_CONST_STORAGE GUI_COLOR ColorsCharge_27x14[] = {
     0x0000FF,0x000000,0xD7A50F,0xF0D26C
    ,0xFFFFFF
};

static GUI_CONST_STORAGE GUI_LOGPALETTE PalCharge_27x14 = {
  5,	// number of entries
  1, 	// Has transparency
  &ColorsCharge_27x14[0]
};

static GUI_CONST_STORAGE unsigned char acCharge1_27x14[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char acCharge2_27x14[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char acCharge3_27x14[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char acCharge4_27x14[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char acCharge5_27x14[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x03, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x02, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static GUI_CONST_STORAGE GUI_BITMAP _bmCharge1_27x14 = {
  27, // XSize
  14, // YSize
  14, // BytesPerLine
  4, // BitsPerPixel
  acCharge1_27x14,  // Pointer to picture data (indices)
  &PalCharge_27x14   // Pointer to palette
};

static GUI_CONST_STORAGE GUI_BITMAP _bmCharge2_27x14 = {
  27, // XSize
  14, // YSize
  14, // BytesPerLine
  4, // BitsPerPixel
  acCharge2_27x14,  // Pointer to picture data (indices)
  &PalCharge_27x14   // Pointer to palette
};

static GUI_CONST_STORAGE GUI_BITMAP _bmCharge3_27x14 = {
  27, // XSize
  14, // YSize
  14, // BytesPerLine
  4, // BitsPerPixel
  acCharge3_27x14,  // Pointer to picture data (indices)
  &PalCharge_27x14   // Pointer to palette
};

static GUI_CONST_STORAGE GUI_BITMAP _bmCharge4_27x14 = {
  27, // XSize
  14, // YSize
  14, // BytesPerLine
  4, // BitsPerPixel
  acCharge4_27x14,  // Pointer to picture data (indices)
  &PalCharge_27x14   // Pointer to palette
};

static GUI_CONST_STORAGE GUI_BITMAP _bmCharge5_27x14 = {
  27, // XSize
  14, // YSize
  14, // BytesPerLine
  4, // BitsPerPixel
  acCharge5_27x14,  // Pointer to picture data (indices)
  &PalCharge_27x14   // Pointer to palette
};

static GUI_CONST_STORAGE GUI_BITMAP * _apbmCharge[] = {
  &_bmCharge1_27x14,
  &_bmCharge2_27x14,
  &_bmCharge3_27x14,
  &_bmCharge4_27x14,
  &_bmCharge5_27x14,
};

/*********************************************************************
*
*       _acClock_16x16_black
*/
static GUI_CONST_STORAGE unsigned long _acClock_16x16_black[] = {
  0xFC000000, 0x62000000, 0x14000000, 0xAF000000, 0x7D000000, 0x24000000, 0x0B000000, 0x08000000, 0x04000000, 0x14000000, 0x3A000000, 0x8B000000, 0xC6000000, 0x1D000000, 0x60000000, 0xFC000000,
  0x60000000, 0x00000000, 0x33000000, 0x1D000000, 0x00000000, 0x07000000, 0x39000000, 0x5C000000, 0x51000000, 0x30000000, 0x04000000, 0x00000000, 0x37000000, 0x60000000, 0x01000000, 0x5E000000,
  0x16000000, 0x3C000000, 0x11000000, 0x00000000, 0x56000000, 0xC6000000, 0xF6000000, 0xFF000000, 0xFF000000, 0xE8000000, 0xAE000000, 0x37000000, 0x00000000, 0x31000000, 0x6F000000, 0x16000000,
  0xC4000000, 0x28000000, 0x00000000, 0x71000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xE5000000, 0x4A000000, 0x00000000, 0x4F000000, 0xD9000000,
  0x99000000, 0x00000000, 0x40000000, 0xFA000000, 0xE6000000, 0xC5000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xB1000000, 0x46000000, 0xCA000000, 0x20000000, 0x06000000, 0xCA000000,
  0x3B000000, 0x00000000, 0xAD000000, 0xFF000000, 0xC7000000, 0x17000000, 0xAC000000, 0xFF000000, 0xFF000000, 0xB4000000, 0x14000000, 0xA9000000, 0xFF000000, 0x80000000, 0x00000000, 0x6D000000,
  0x0E000000, 0x14000000, 0xE5000000, 0xFF000000, 0xFF000000, 0xB2000000, 0x13000000, 0xB1000000, 0xB8000000, 0x14000000, 0xAC000000, 0xFF000000, 0xFF000000, 0xBD000000, 0x00000000, 0x32000000,
  0x06000000, 0x28000000, 0xF9000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xB5000000, 0x19000000, 0x17000000, 0xAC000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xD5000000, 0x00000000, 0x24000000,
  0x0D000000, 0x1C000000, 0xEE000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xD3000000, 0xD0000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xC9000000, 0x00000000, 0x31000000,
  0x34000000, 0x03000000, 0xC1000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0x94000000, 0x00000000, 0x5B000000,
  0x71000000, 0x00000000, 0x5F000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xF8000000, 0x35000000, 0x00000000, 0xA5000000,
  0xCF000000, 0x0F000000, 0x07000000, 0xA0000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0x76000000, 0x00000000, 0x30000000, 0xF7000000,
  0xFF000000, 0x8B000000, 0x00000000, 0x06000000, 0x92000000, 0xF2000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xE5000000, 0x71000000, 0x00000000, 0x06000000, 0xB6000000, 0xFF000000,
  0xFF000000, 0xF9000000, 0x7C000000, 0x00000000, 0x00000000, 0x36000000, 0x81000000, 0x98000000, 0x9A000000, 0x6C000000, 0x26000000, 0x00000000, 0x11000000, 0x9F000000, 0xFF000000, 0xFF000000,
  0xFF000000, 0xFF000000, 0xFD000000, 0xA0000000, 0x2B000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x01000000, 0x4F000000, 0xC0000000, 0xFF000000, 0xFF000000, 0xFF000000,
  0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xEF000000, 0xAE000000, 0x72000000, 0x67000000, 0x64000000, 0x89000000, 0xC1000000, 0xFC000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
};

GUI_CONST_STORAGE GUI_BITMAP bmClock_16x16_black = {
  16, // xSize
  16, // ySize
  64, // BytesPerLine
  32, // BitsPerPixel
  (unsigned char *)_acClock_16x16_black,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_BMP8888
};

/*********************************************************************
*
*       _bmLogoSegger_40x20
*/
static GUI_CONST_STORAGE GUI_COLOR ColorsLogoSegger_40x20[] = {
     0x0000FF,0x201F23,0xC7C7C8,0xA02020
    ,0xD09090,0x58575A,0xB24A4A,0xCA8282
    ,0xA62E2E,0x908F91,0xD5D5D6,0xEDD5D5
    ,0x828183,0x4A494C,0x666568,0x9D9D9F
    ,0xE1B9B9,0xF9F1F1,0xF1F1F1,0xF3E3E3
    ,0xABABAD,0xE3E3E4,0x747376,0xFFFFFF
    ,0xD59D9D,0xE7C7C7,0x3C3B3F,0x2E2D31
    ,0xB85858,0xBE6666,0xC47474,0xB9B9BA
    ,0xDBABAB
};

static GUI_CONST_STORAGE GUI_LOGPALETTE PalLogoSegger_40x20 = {
  33,	// number of entries
  1, 	// Has transparency
  &ColorsLogoSegger_40x20[0]
};

static GUI_CONST_STORAGE unsigned char acLogoSegger_40x20[] = {
  0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
  0x01, 0x09, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x09, 0x01,
  0x01, 0x02, 0x0B, 0x11, 0x17, 0x17, 0x0B, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x02, 0x01,
  0x01, 0x02, 0x06, 0x06, 0x17, 0x19, 0x03, 0x20, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x02, 0x01,
  0x01, 0x02, 0x10, 0x03, 0x07, 0x17, 0x1C, 0x08, 0x0B, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x02, 0x01,
  0x01, 0x02, 0x17, 0x07, 0x03, 0x10, 0x11, 0x08, 0x06, 0x11, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x02, 0x01,
  0x01, 0x02, 0x19, 0x17, 0x06, 0x08, 0x13, 0x0B, 0x03, 0x1D, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x02, 0x01,
  0x01, 0x02, 0x08, 0x13, 0x13, 0x08, 0x06, 0x17, 0x18, 0x03, 0x18, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x02, 0x01,
  0x01, 0x02, 0x03, 0x06, 0x17, 0x10, 0x03, 0x07, 0x17, 0x1E, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07, 0x02, 0x01,
  0x01, 0x02, 0x03, 0x03, 0x07, 0x17, 0x07, 0x03, 0x10, 0x11, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0B, 0x02, 0x01,
  0x01, 0x02, 0x03, 0x03, 0x07, 0x17, 0x07, 0x03, 0x10, 0x11, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0B, 0x02, 0x01,
  0x01, 0x02, 0x03, 0x06, 0x17, 0x10, 0x03, 0x07, 0x17, 0x1E, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07, 0x02, 0x01,
  0x01, 0x02, 0x08, 0x13, 0x13, 0x08, 0x06, 0x17, 0x18, 0x03, 0x18, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x02, 0x01,
  0x01, 0x02, 0x19, 0x17, 0x06, 0x08, 0x13, 0x0B, 0x03, 0x1D, 0x12, 0x0E, 0x05, 0x0E, 0x12, 0x0E, 0x05, 0x05, 0x09, 0x0A, 0x05, 0x05, 0x16, 0x12, 0x1F, 0x05, 0x05, 0x0C, 0x17, 0x0E, 0x05, 0x05, 0x14, 0x0C, 0x05, 0x05, 0x0C, 0x17, 0x02, 0x01,
  0x01, 0x02, 0x17, 0x07, 0x03, 0x10, 0x11, 0x08, 0x06, 0x11, 0x02, 0x01, 0x0F, 0x0C, 0x15, 0x01, 0x16, 0x09, 0x0A, 0x1A, 0x0C, 0x15, 0x0E, 0x15, 0x1B, 0x09, 0x0A, 0x0E, 0x12, 0x01, 0x16, 0x09, 0x15, 0x05, 0x0F, 0x0C, 0x01, 0x17, 0x02, 0x01,
  0x01, 0x02, 0x10, 0x03, 0x07, 0x17, 0x1C, 0x08, 0x0B, 0x17, 0x17, 0x16, 0x05, 0x01, 0x14, 0x01, 0x0D, 0x05, 0x02, 0x01, 0x1F, 0x0E, 0x01, 0x0F, 0x01, 0x02, 0x0E, 0x01, 0x02, 0x01, 0x0D, 0x05, 0x02, 0x05, 0x0D, 0x1B, 0x0D, 0x17, 0x02, 0x01,
  0x01, 0x02, 0x06, 0x06, 0x17, 0x19, 0x03, 0x20, 0x17, 0x17, 0x0A, 0x01, 0x0C, 0x1A, 0x0F, 0x01, 0x0D, 0x05, 0x0F, 0x05, 0x1A, 0x0C, 0x01, 0x09, 0x0D, 0x0D, 0x0C, 0x01, 0x02, 0x01, 0x0D, 0x05, 0x0F, 0x05, 0x02, 0x02, 0x01, 0x17, 0x02, 0x01,
  0x01, 0x02, 0x0B, 0x11, 0x17, 0x17, 0x0B, 0x17, 0x17, 0x17, 0x17, 0x0A, 0x09, 0x14, 0x17, 0x0A, 0x02, 0x02, 0x0A, 0x17, 0x14, 0x0F, 0x02, 0x0A, 0x12, 0x14, 0x0F, 0x02, 0x15, 0x02, 0x02, 0x02, 0x0A, 0x0A, 0x12, 0x12, 0x02, 0x17, 0x02, 0x01,
  0x01, 0x09, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x09, 0x01,
  0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00
};

static GUI_CONST_STORAGE GUI_BITMAP _bmLogoSegger_40x20 = {
  40, // XSize
  20, // YSize
  40, // BytesPerLine
  8, // BitsPerPixel
  acLogoSegger_40x20,  // Pointer to picture data (indices)
  &PalLogoSegger_40x20   // Pointer to palette
};





static GUI_CONST_STORAGE unsigned char _ac0[] = {
  /* RLE: 011 Pixels @ 000,000 */ 11, 0xFF,
  /* ABS: 010 Pixels @ 011,000 */ 0, 10, 0xF6, 0xA8, 0x48, 0x1D, 0x11, 0x0E, 0x1E, 0x47, 0x8E, 0xE7,
  /* RLE: 021 Pixels @ 021,000 */ 21, 0xFF,
  /* ABS: 002 Pixels @ 010,001 */ 0, 2, 0xBF, 0x30,
  /* RLE: 008 Pixels @ 012,001 */ 8, 0x00,
  /* ABS: 003 Pixels @ 020,001 */ 0, 3, 0x17, 0x97, 0xFE,
  /* RLE: 017 Pixels @ 023,001 */ 17, 0xFF,
  /* ABS: 003 Pixels @ 008,002 */ 0, 3, 0xFE, 0x91, 0x08,
  /* RLE: 011 Pixels @ 011,002 */ 11, 0x00,
  /* ABS: 002 Pixels @ 022,002 */ 0, 2, 0x69, 0xF8,
  /* RLE: 016 Pixels @ 024,002 */ 16, 0xFF,
  /* RLE: 001 Pixels @ 008,003 */ 1, 0x7B,
  /* RLE: 005 Pixels @ 009,003 */ 5, 0x00,
  /* ABS: 004 Pixels @ 014,003 */ 0, 4, 0x03, 0x0E, 0x0F, 0x06,
  /* RLE: 005 Pixels @ 018,003 */ 5, 0x00,
  /* ABS: 002 Pixels @ 023,003 */ 0, 2, 0x49, 0xF6,
  /* RLE: 014 Pixels @ 025,003 */ 14, 0xFF,
  /* ABS: 013 Pixels @ 007,004 */ 0, 13, 0xBE, 0x04, 0x00, 0x00, 0x00, 0x03, 0x60, 0xC2, 0xED, 0xF1, 0xD3, 0x7F, 0x13,
  /* RLE: 004 Pixels @ 020,004 */ 4, 0x00,
  /* RLE: 001 Pixels @ 024,004 */ 1, 0x85,
  /* RLE: 014 Pixels @ 025,004 */ 14, 0xFF,
  /* ABS: 006 Pixels @ 007,005 */ 0, 6, 0x4A, 0x00, 0x00, 0x00, 0x06, 0xA2,
  /* RLE: 006 Pixels @ 013,005 */ 6, 0xFF,
  /* ABS: 007 Pixels @ 019,005 */ 0, 7, 0xCE, 0x21, 0x00, 0x00, 0x00, 0x19, 0xE4,
  /* RLE: 012 Pixels @ 026,005 */ 12, 0xFF,
  /* ABS: 006 Pixels @ 006,006 */ 0, 6, 0xD4, 0x08, 0x00, 0x00, 0x00, 0x7D,
  /* RLE: 008 Pixels @ 012,006 */ 8, 0xFF,
  /* ABS: 006 Pixels @ 020,006 */ 0, 6, 0xB8, 0x01, 0x00, 0x00, 0x00, 0x99,
  /* RLE: 012 Pixels @ 026,006 */ 12, 0xFF,
  /* ABS: 006 Pixels @ 006,007 */ 0, 6, 0x9B, 0x00, 0x00, 0x00, 0x15, 0xE5,
  /* RLE: 009 Pixels @ 012,007 */ 9, 0xFF,
  /* ABS: 005 Pixels @ 021,007 */ 0, 5, 0x48, 0x00, 0x00, 0x00, 0x5F,
  /* RLE: 012 Pixels @ 026,007 */ 12, 0xFF,
  /* ABS: 005 Pixels @ 006,008 */ 0, 5, 0x7A, 0x00, 0x00, 0x00, 0x46,
  /* RLE: 010 Pixels @ 011,008 */ 10, 0xFF,
  /* ABS: 006 Pixels @ 021,008 */ 0, 6, 0x82, 0x00, 0x00, 0x00, 0x3E, 0xFE,
  /* RLE: 011 Pixels @ 027,008 */ 11, 0xFF,
  /* ABS: 005 Pixels @ 006,009 */ 0, 5, 0x71, 0x00, 0x00, 0x00, 0x56,
  /* RLE: 010 Pixels @ 011,009 */ 10, 0xFF,
  /* ABS: 006 Pixels @ 021,009 */ 0, 6, 0x93, 0x00, 0x00, 0x00, 0x34, 0xFA,
  /* RLE: 011 Pixels @ 027,009 */ 11, 0xFF,
  /* ABS: 005 Pixels @ 006,010 */ 0, 5, 0x70, 0x00, 0x00, 0x00, 0x57,
  /* RLE: 010 Pixels @ 011,010 */ 10, 0xFF,
  /* ABS: 006 Pixels @ 021,010 */ 0, 6, 0x94, 0x00, 0x00, 0x00, 0x33, 0xFA,
  /* RLE: 011 Pixels @ 027,010 */ 11, 0xFF,
  /* ABS: 005 Pixels @ 006,011 */ 0, 5, 0x71, 0x00, 0x00, 0x00, 0x57,
  /* RLE: 010 Pixels @ 011,011 */ 10, 0xFF,
  /* ABS: 006 Pixels @ 021,011 */ 0, 6, 0x94, 0x00, 0x00, 0x00, 0x34, 0xFA,
  /* RLE: 011 Pixels @ 027,011 */ 11, 0xFF,
  /* ABS: 005 Pixels @ 006,012 */ 0, 5, 0x71, 0x00, 0x00, 0x00, 0x57,
  /* RLE: 010 Pixels @ 011,012 */ 10, 0xFF,
  /* ABS: 006 Pixels @ 021,012 */ 0, 6, 0x94, 0x00, 0x00, 0x00, 0x35, 0xFB,
  /* RLE: 008 Pixels @ 027,012 */ 8, 0xFF,
  /* ABS: 010 Pixels @ 003,013 */ 0, 10, 0xCF, 0x69, 0x59, 0x26, 0x00, 0x00, 0x00, 0x1D, 0x58, 0x57,
  /* RLE: 007 Pixels @ 013,013 */ 7, 0x56,
  /* ABS: 009 Pixels @ 020,013 */ 0, 9, 0x5B, 0x32, 0x00, 0x00, 0x00, 0x11, 0x53, 0x5E, 0xAF,
  /* RLE: 005 Pixels @ 029,013 */ 5, 0xFF,
  /* ABS: 002 Pixels @ 002,014 */ 0, 2, 0xD6, 0x18,
  /* RLE: 025 Pixels @ 004,014 */ 25, 0x00,
  /* RLE: 001 Pixels @ 029,014 */ 1, 0xA4,
  /* RLE: 004 Pixels @ 030,014 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,015 */ 1, 0x7E,
  /* RLE: 026 Pixels @ 003,015 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,015 */ 1, 0x46,
  /* RLE: 004 Pixels @ 030,015 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,016 */ 1, 0x63,
  /* RLE: 026 Pixels @ 003,016 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,016 */ 1, 0x34,
  /* RLE: 004 Pixels @ 030,016 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,017 */ 1, 0x64,
  /* RLE: 026 Pixels @ 003,017 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,017 */ 1, 0x35,
  /* RLE: 004 Pixels @ 030,017 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,018 */ 1, 0x64,
  /* RLE: 026 Pixels @ 003,018 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,018 */ 1, 0x35,
  /* RLE: 004 Pixels @ 030,018 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,019 */ 1, 0x64,
  /* RLE: 026 Pixels @ 003,019 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,019 */ 1, 0x35,
  /* RLE: 004 Pixels @ 030,019 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,020 */ 1, 0x64,
  /* RLE: 026 Pixels @ 003,020 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,020 */ 1, 0x35,
  /* RLE: 004 Pixels @ 030,020 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,021 */ 1, 0x64,
  /* RLE: 026 Pixels @ 003,021 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,021 */ 1, 0x35,
  /* RLE: 004 Pixels @ 030,021 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,022 */ 1, 0x64,
  /* RLE: 026 Pixels @ 003,022 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,022 */ 1, 0x35,
  /* RLE: 004 Pixels @ 030,022 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,023 */ 1, 0x64,
  /* RLE: 026 Pixels @ 003,023 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,023 */ 1, 0x35,
  /* RLE: 004 Pixels @ 030,023 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,024 */ 1, 0x64,
  /* RLE: 026 Pixels @ 003,024 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,024 */ 1, 0x35,
  /* RLE: 004 Pixels @ 030,024 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,025 */ 1, 0x64,
  /* RLE: 026 Pixels @ 003,025 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,025 */ 1, 0x35,
  /* RLE: 004 Pixels @ 030,025 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,026 */ 1, 0x64,
  /* RLE: 026 Pixels @ 003,026 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,026 */ 1, 0x35,
  /* RLE: 004 Pixels @ 030,026 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,027 */ 1, 0x64,
  /* RLE: 026 Pixels @ 003,027 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,027 */ 1, 0x35,
  /* RLE: 004 Pixels @ 030,027 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,028 */ 1, 0x62,
  /* RLE: 026 Pixels @ 003,028 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,028 */ 1, 0x34,
  /* RLE: 004 Pixels @ 030,028 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,029 */ 1, 0x6E,
  /* RLE: 026 Pixels @ 003,029 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,029 */ 1, 0x3B,
  /* RLE: 004 Pixels @ 030,029 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,030 */ 1, 0xB5,
  /* RLE: 026 Pixels @ 003,030 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,030 */ 1, 0x7B,
  /* RLE: 005 Pixels @ 030,030 */ 5, 0xFF,
  /* ABS: 003 Pixels @ 003,031 */ 0, 3, 0x94, 0x1F, 0x0D,
  /* RLE: 020 Pixels @ 006,031 */ 20, 0x0F,
  /* ABS: 006 Pixels @ 026,031 */ 0, 6, 0x0D, 0x15, 0x6B, 0xF3, 0xFF, 0xFF,
  0
};  // 416 bytes for 1024 pixels

GUI_CONST_STORAGE GUI_BITMAP bm0 = {
  32, // xSize
  32, // ySize
  32, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_ac0,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*********************************************************************
*
*       bm1
*/
static GUI_CONST_STORAGE unsigned char _ac1[] = {
  /* RLE: 065 Pixels @ 000,000 */ 65, 0xFF,
  /* ABS: 003 Pixels @ 001,002 */ 0, 3, 0xA1, 0x66, 0x59,
  /* RLE: 024 Pixels @ 004,002 */ 24, 0x5A,
  /* ABS: 005 Pixels @ 028,002 */ 0, 5, 0x59, 0x67, 0xA6, 0xFF, 0x85,
  /* RLE: 030 Pixels @ 001,003 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,003 */ 0, 2, 0x8F, 0x17,
  /* RLE: 030 Pixels @ 001,004 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,004 */ 0, 2, 0x1F, 0x03,
  /* RLE: 030 Pixels @ 001,005 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,005 */ 0, 2, 0x06, 0x5A,
  /* RLE: 030 Pixels @ 001,006 */ 30, 0x00,
  /* ABS: 004 Pixels @ 031,006 */ 0, 4, 0x4B, 0xFD, 0x84, 0x03,
  /* RLE: 027 Pixels @ 003,007 */ 27, 0x00,
  /* ABS: 006 Pixels @ 030,007 */ 0, 6, 0x70, 0xF6, 0xFF, 0xFF, 0xB1, 0x19,
  /* RLE: 024 Pixels @ 004,008 */ 24, 0x00,
  /* ABS: 002 Pixels @ 028,008 */ 0, 2, 0x0F, 0xA1,
  /* RLE: 005 Pixels @ 030,008 */ 5, 0xFF,
  /* ABS: 002 Pixels @ 003,009 */ 0, 2, 0xC3, 0x19,
  /* RLE: 022 Pixels @ 005,009 */ 22, 0x00,
  /* ABS: 011 Pixels @ 027,009 */ 0, 11, 0x0F, 0xB2, 0xFF, 0xFF, 0xFF, 0xED, 0xFF, 0xFF, 0xFF, 0xD6, 0x41,
  /* RLE: 020 Pixels @ 006,010 */ 20, 0x00,
  /* ABS: 014 Pixels @ 026,010 */ 0, 14, 0x34, 0xC9, 0xFF, 0xFF, 0xFF, 0xEB, 0x34, 0xBB, 0xFF, 0xFF, 0xFF, 0xF7, 0x75, 0x01,
  /* RLE: 017 Pixels @ 008,011 */ 17, 0x00,
  /* ABS: 016 Pixels @ 025,011 */ 0, 16, 0x67, 0xF0, 0xFF, 0xFF, 0xFF, 0xB8, 0x34, 0x05, 0x07, 0x8A, 0xFE, 0xFF, 0xFF, 0xFF, 0xAE, 0x21,
  /* RLE: 014 Pixels @ 009,012 */ 14, 0x00,
  /* ABS: 019 Pixels @ 023,012 */ 0, 19, 0x19, 0xA3, 0xFF, 0xFF, 0xFF, 0xFE, 0x8A, 0x06, 0x09, 0x0F, 0x00, 0x00, 0x53, 0xE4, 0xFF, 0xFF, 0xFF, 0xE0, 0x52,
  /* RLE: 012 Pixels @ 010,013 */ 12, 0x00,
  /* ABS: 016 Pixels @ 022,013 */ 0, 16, 0x47, 0xD8, 0xFF, 0xFF, 0xFF, 0xE6, 0x54, 0x00, 0x00, 0x13, 0x0F, 0x00, 0x00, 0x00, 0x24, 0xB4,
  /* RLE: 004 Pixels @ 006,014 */ 4, 0xFF,
  /* ABS: 002 Pixels @ 010,014 */ 0, 2, 0x96, 0x14,
  /* RLE: 008 Pixels @ 012,014 */ 8, 0x00,
  /* ABS: 013 Pixels @ 020,014 */ 0, 13, 0x10, 0x8D, 0xFB, 0xFF, 0xFF, 0xFF, 0xB8, 0x27, 0x00, 0x00, 0x00, 0x13, 0x0F,
  /* RLE: 004 Pixels @ 001,015 */ 4, 0x00,
  /* ABS: 008 Pixels @ 005,015 */ 0, 8, 0x03, 0x76, 0xF8, 0xFF, 0xFF, 0xFF, 0xD6, 0x51,
  /* RLE: 006 Pixels @ 013,015 */ 6, 0x00,
  /* ABS: 008 Pixels @ 019,015 */ 0, 8, 0x4D, 0xD2, 0xFF, 0xFF, 0xFF, 0xFB, 0x7D, 0x05,
  /* RLE: 004 Pixels @ 027,015 */ 4, 0x00,
  /* ABS: 002 Pixels @ 031,015 */ 0, 2, 0x13, 0x0F,
  /* RLE: 006 Pixels @ 001,016 */ 6, 0x00,
  /* ABS: 002 Pixels @ 007,016 */ 0, 2, 0x57, 0xED,
  /* RLE: 004 Pixels @ 009,016 */ 4, 0xFF,
  /* ABS: 006 Pixels @ 013,016 */ 0, 6, 0x80, 0x11, 0x00, 0x00, 0x11, 0x7E,
  /* RLE: 004 Pixels @ 019,016 */ 4, 0xFF,
  /* ABS: 002 Pixels @ 023,016 */ 0, 2, 0xF1, 0x5F,
  /* RLE: 006 Pixels @ 025,016 */ 6, 0x00,
  /* ABS: 002 Pixels @ 031,016 */ 0, 2, 0x13, 0x0F,
  /* RLE: 007 Pixels @ 001,017 */ 7, 0x00,
  /* ABS: 002 Pixels @ 008,017 */ 0, 2, 0x2F, 0xBC,
  /* RLE: 004 Pixels @ 010,017 */ 4, 0xFF,
  /* ABS: 004 Pixels @ 014,017 */ 0, 4, 0xD8, 0x83, 0x84, 0xD8,
  /* RLE: 004 Pixels @ 018,017 */ 4, 0xFF,
  /* ABS: 002 Pixels @ 022,017 */ 0, 2, 0xC4, 0x35,
  /* RLE: 007 Pixels @ 024,017 */ 7, 0x00,
  /* ABS: 002 Pixels @ 031,017 */ 0, 2, 0x12, 0x0F,
  /* RLE: 008 Pixels @ 001,018 */ 8, 0x00,
  /* ABS: 003 Pixels @ 009,018 */ 0, 3, 0x05, 0x72, 0xEE,
  /* RLE: 008 Pixels @ 012,018 */ 8, 0xFF,
  /* ABS: 003 Pixels @ 020,018 */ 0, 3, 0xF2, 0x7C, 0x09,
  /* RLE: 008 Pixels @ 023,018 */ 8, 0x00,
  /* ABS: 002 Pixels @ 031,018 */ 0, 2, 0x12, 0x0F,
  /* RLE: 010 Pixels @ 001,019 */ 10, 0x00,
  /* ABS: 002 Pixels @ 011,019 */ 0, 2, 0x2A, 0xAA,
  /* RLE: 006 Pixels @ 013,019 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 019,019 */ 0, 2, 0xB2, 0x31,
  /* RLE: 010 Pixels @ 021,019 */ 10, 0x00,
  /* ABS: 002 Pixels @ 031,019 */ 0, 2, 0x12, 0x0F,
  /* RLE: 012 Pixels @ 001,020 */ 12, 0x00,
  /* ABS: 006 Pixels @ 013,020 */ 0, 6, 0x4A, 0xBB, 0xFA, 0xFA, 0xBD, 0x4F,
  /* RLE: 012 Pixels @ 019,020 */ 12, 0x00,
  /* ABS: 002 Pixels @ 031,020 */ 0, 2, 0x11, 0x0F,
  /* RLE: 014 Pixels @ 001,021 */ 14, 0x00,
  /* ABS: 002 Pixels @ 015,021 */ 0, 2, 0x33, 0x32,
  /* RLE: 014 Pixels @ 017,021 */ 14, 0x00,
  /* ABS: 002 Pixels @ 031,021 */ 0, 2, 0x11, 0x0F,
  /* RLE: 030 Pixels @ 001,022 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,022 */ 0, 2, 0x11, 0x0F,
  /* RLE: 030 Pixels @ 001,023 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,023 */ 0, 2, 0x10, 0x0F,
  /* RLE: 030 Pixels @ 001,024 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,024 */ 0, 2, 0x10, 0x0F,
  /* RLE: 030 Pixels @ 001,025 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,025 */ 0, 2, 0x10, 0x0E,
  /* RLE: 030 Pixels @ 001,026 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,026 */ 0, 2, 0x0E, 0x0F,
  /* RLE: 030 Pixels @ 001,027 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,027 */ 0, 2, 0x15, 0x60,
  /* RLE: 030 Pixels @ 001,028 */ 30, 0x00,
  /* ABS: 004 Pixels @ 031,028 */ 0, 4, 0x6A, 0xEF, 0x76, 0x3C,
  /* RLE: 026 Pixels @ 003,029 */ 26, 0x38,
  /* ABS: 003 Pixels @ 029,029 */ 0, 3, 0x3D, 0x7B, 0xF3,
  /* RLE: 064 Pixels @ 000,030 */ 64, 0xFF,
  0
};  // 386 bytes for 1024 pixels

GUI_CONST_STORAGE GUI_BITMAP bm1 = {
  32, // xSize
  32, // ySize
  32, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_ac1,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*********************************************************************
*
*       bm2
*/
static GUI_CONST_STORAGE unsigned char _ac2[] = {
  /* RLE: 013 Pixels @ 000,000 */ 13, 0xFF,
  /* ABS: 007 Pixels @ 013,000 */ 0, 7, 0xD5, 0x5D, 0x1C, 0x11, 0x1D, 0x53, 0xB7,
  /* RLE: 024 Pixels @ 020,000 */ 24, 0xFF,
  /* ABS: 002 Pixels @ 012,001 */ 0, 2, 0xA3, 0x0D,
  /* RLE: 006 Pixels @ 014,001 */ 6, 0x00,
  /* ABS: 002 Pixels @ 020,001 */ 0, 2, 0x70, 0xF9,
  /* RLE: 021 Pixels @ 022,001 */ 21, 0xFF,
  /* ABS: 002 Pixels @ 011,002 */ 0, 2, 0x95, 0x03,
  /* RLE: 008 Pixels @ 013,002 */ 8, 0x00,
  /* ABS: 002 Pixels @ 021,002 */ 0, 2, 0x50, 0xFC,
  /* RLE: 019 Pixels @ 023,002 */ 19, 0xFF,
  /* ABS: 002 Pixels @ 010,003 */ 0, 2, 0xDC, 0x12,
  /* RLE: 010 Pixels @ 012,003 */ 10, 0x00,
  /* RLE: 001 Pixels @ 022,003 */ 1, 0xA7,
  /* RLE: 019 Pixels @ 023,003 */ 19, 0xFF,
  /* RLE: 001 Pixels @ 010,004 */ 1, 0x92,
  /* RLE: 011 Pixels @ 011,004 */ 11, 0x00,
  /* RLE: 001 Pixels @ 022,004 */ 1, 0x57,
  /* RLE: 019 Pixels @ 023,004 */ 19, 0xFF,
  /* RLE: 001 Pixels @ 010,005 */ 1, 0x6D,
  /* RLE: 011 Pixels @ 011,005 */ 11, 0x00,
  /* ABS: 002 Pixels @ 022,005 */ 0, 2, 0x30, 0xF9,
  /* RLE: 018 Pixels @ 024,005 */ 18, 0xFF,
  /* RLE: 001 Pixels @ 010,006 */ 1, 0x6D,
  /* RLE: 011 Pixels @ 011,006 */ 11, 0x00,
  /* ABS: 002 Pixels @ 022,006 */ 0, 2, 0x30, 0xF9,
  /* RLE: 018 Pixels @ 024,006 */ 18, 0xFF,
  /* RLE: 001 Pixels @ 010,007 */ 1, 0x93,
  /* RLE: 011 Pixels @ 011,007 */ 11, 0x00,
  /* RLE: 001 Pixels @ 022,007 */ 1, 0x58,
  /* RLE: 019 Pixels @ 023,007 */ 19, 0xFF,
  /* ABS: 002 Pixels @ 010,008 */ 0, 2, 0xDD, 0x13,
  /* RLE: 010 Pixels @ 012,008 */ 10, 0x00,
  /* RLE: 001 Pixels @ 022,008 */ 1, 0xA9,
  /* RLE: 020 Pixels @ 023,008 */ 20, 0xFF,
  /* ABS: 002 Pixels @ 011,009 */ 0, 2, 0x98, 0x04,
  /* RLE: 008 Pixels @ 013,009 */ 8, 0x00,
  /* ABS: 002 Pixels @ 021,009 */ 0, 2, 0x53, 0xFD,
  /* RLE: 021 Pixels @ 023,009 */ 21, 0xFF,
  /* ABS: 002 Pixels @ 012,010 */ 0, 2, 0xA5, 0x0E,
  /* RLE: 006 Pixels @ 014,010 */ 6, 0x00,
  /* ABS: 002 Pixels @ 020,010 */ 0, 2, 0x72, 0xFA,
  /* RLE: 023 Pixels @ 022,010 */ 23, 0xFF,
  /* ABS: 007 Pixels @ 013,011 */ 0, 7, 0xD6, 0x69, 0x28, 0x0D, 0x1C, 0x53, 0xB9,
  /* RLE: 021 Pixels @ 020,011 */ 21, 0xFF,
  /* ABS: 015 Pixels @ 009,012 */ 0, 15, 0xFC, 0xCF, 0x96, 0xD7, 0xFF, 0xFF, 0xFC, 0xF3, 0xF9, 0xFF, 0xFF, 0xF3, 0xA7, 0xB6, 0xF3,
  /* RLE: 015 Pixels @ 024,012 */ 15, 0xFF,
  /* ABS: 019 Pixels @ 007,013 */ 0, 19, 0xF0, 0x96, 0x37, 0x04, 0x00, 0x13, 0x6F, 0xBB, 0xE6, 0xF6, 0xEE, 0xCC, 0x8D, 0x2C, 0x00, 0x00, 0x20, 0x74, 0xD8,
  /* RLE: 012 Pixels @ 026,013 */ 12, 0xFF,
  /* ABS: 002 Pixels @ 006,014 */ 0, 2, 0xA5, 0x23,
  /* RLE: 006 Pixels @ 008,014 */ 6, 0x00,
  /* ABS: 005 Pixels @ 014,014 */ 0, 5, 0x02, 0x0B, 0x11, 0x0E, 0x04,
  /* RLE: 006 Pixels @ 019,014 */ 6, 0x00,
  /* ABS: 003 Pixels @ 025,014 */ 0, 3, 0x0B, 0x76, 0xF0,
  /* RLE: 008 Pixels @ 028,014 */ 8, 0xFF,
  /* ABS: 002 Pixels @ 004,015 */ 0, 2, 0xF6, 0x64,
  /* RLE: 021 Pixels @ 006,015 */ 21, 0x00,
  /* ABS: 002 Pixels @ 027,015 */ 0, 2, 0x30, 0xD4,
  /* RLE: 006 Pixels @ 029,015 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 003,016 */ 0, 2, 0xFB, 0x57,
  /* RLE: 023 Pixels @ 005,016 */ 23, 0x00,
  /* ABS: 002 Pixels @ 028,016 */ 0, 2, 0x21, 0xD8,
  /* RLE: 005 Pixels @ 030,016 */ 5, 0xFF,
  /* RLE: 001 Pixels @ 003,017 */ 1, 0x86,
  /* RLE: 025 Pixels @ 004,017 */ 25, 0x00,
  /* ABS: 007 Pixels @ 029,017 */ 0, 7, 0x3F, 0xF9, 0xFF, 0xFF, 0xFF, 0xE2, 0x16,
  /* RLE: 026 Pixels @ 004,018 */ 26, 0x00,
  /* ABS: 005 Pixels @ 030,018 */ 0, 5, 0xA1, 0xFF, 0xFF, 0xFF, 0x9B,
  /* RLE: 027 Pixels @ 003,019 */ 27, 0x00,
  /* ABS: 005 Pixels @ 030,019 */ 0, 5, 0x52, 0xFF, 0xFF, 0xFF, 0x6A,
  /* RLE: 027 Pixels @ 003,020 */ 27, 0x00,
  /* ABS: 005 Pixels @ 030,020 */ 0, 5, 0x2B, 0xFF, 0xFF, 0xFF, 0x59,
  /* RLE: 027 Pixels @ 003,021 */ 27, 0x00,
  /* ABS: 010 Pixels @ 030,021 */ 0, 10, 0x21, 0xFF, 0xFF, 0xFF, 0x5A, 0x00, 0x00, 0x00, 0x25, 0x2F,
  /* RLE: 017 Pixels @ 008,022 */ 17, 0x00,
  /* ABS: 015 Pixels @ 025,022 */ 0, 15, 0x18, 0x38, 0x00, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0xFF, 0x5A, 0x00, 0x00, 0x00, 0x98, 0xC2,
  /* RLE: 017 Pixels @ 008,023 */ 17, 0x00,
  /* ABS: 015 Pixels @ 025,023 */ 0, 15, 0x70, 0xE3, 0x09, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0xFF, 0x5A, 0x00, 0x00, 0x00, 0x9E, 0xC7,
  /* RLE: 017 Pixels @ 008,024 */ 17, 0x00,
  /* ABS: 015 Pixels @ 025,024 */ 0, 15, 0x75, 0xEA, 0x0B, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0xFF, 0x59, 0x00, 0x00, 0x00, 0x9D, 0xC8,
  /* RLE: 017 Pixels @ 008,025 */ 17, 0x00,
  /* ABS: 015 Pixels @ 025,025 */ 0, 15, 0x74, 0xE9, 0x0A, 0x00, 0x00, 0x21, 0xFF, 0xFF, 0xFF, 0x5F, 0x00, 0x00, 0x00, 0xA0, 0xC8,
  /* RLE: 017 Pixels @ 008,026 */ 17, 0x00,
  /* ABS: 015 Pixels @ 025,026 */ 0, 15, 0x74, 0xEC, 0x0B, 0x00, 0x00, 0x25, 0xFF, 0xFF, 0xFF, 0xB3, 0x19, 0x0C, 0x28, 0xDC, 0xC0,
  /* RLE: 017 Pixels @ 008,027 */ 17, 0x00,
  /* ABS: 006 Pixels @ 025,027 */ 0, 6, 0x71, 0xFF, 0x55, 0x0C, 0x0D, 0x76,
  /* RLE: 004 Pixels @ 031,027 */ 4, 0xFF,
  /* ABS: 005 Pixels @ 003,028 */ 0, 5, 0xF4, 0xF0, 0xF6, 0xFF, 0xB7,
  /* RLE: 017 Pixels @ 008,028 */ 17, 0x00,
  /* ABS: 006 Pixels @ 025,028 */ 0, 6, 0x6F, 0xFF, 0xFB, 0xF1, 0xF1, 0xFC,
  /* RLE: 008 Pixels @ 031,028 */ 8, 0xFF,
  /* ABS: 002 Pixels @ 007,029 */ 0, 2, 0xE1, 0x23,
  /* RLE: 016 Pixels @ 009,029 */ 16, 0x00,
  /* RLE: 001 Pixels @ 025,029 */ 1, 0xA6,
  /* RLE: 014 Pixels @ 026,029 */ 14, 0xFF,
  /* ABS: 003 Pixels @ 008,030 */ 0, 3, 0xDE, 0x5D, 0x09,
  /* RLE: 012 Pixels @ 011,030 */ 12, 0x00,
  /* ABS: 002 Pixels @ 023,030 */ 0, 2, 0x36, 0xB9,
  /* RLE: 017 Pixels @ 025,030 */ 17, 0xFF,
  /* ABS: 014 Pixels @ 010,031 */ 0, 14, 0xDB, 0x9A, 0x6B, 0x42, 0x26, 0x16, 0x0F, 0x12, 0x20, 0x38, 0x5B, 0x85, 0xC3, 0xFA,
  /* RLE: 008 Pixels @ 024,031 */ 8, 0xFF,
  0
};  // 437 bytes for 1024 pixels

GUI_CONST_STORAGE GUI_BITMAP bm2 = {
  32, // xSize
  32, // ySize
  32, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_ac2,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*********************************************************************
*
*       bm3
*/
static GUI_CONST_STORAGE unsigned char _ac3[] = {
  /* RLE: 032 Pixels @ 000,000 */ 32, 0xFF,
  /* ABS: 003 Pixels @ 000,001 */ 0, 3, 0x80, 0x33, 0x34,
  /* RLE: 026 Pixels @ 003,001 */ 26, 0x35,
  /* ABS: 004 Pixels @ 029,001 */ 0, 4, 0x34, 0x33, 0x81, 0x09,
  /* RLE: 030 Pixels @ 001,002 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,002 */ 0, 2, 0x0B, 0x0E,
  /* RLE: 030 Pixels @ 001,003 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,003 */ 0, 2, 0x10, 0x0F,
  /* RLE: 030 Pixels @ 001,004 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,004 */ 0, 2, 0x11, 0x0F,
  /* RLE: 030 Pixels @ 001,005 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,005 */ 0, 2, 0x11, 0x07,
  /* RLE: 030 Pixels @ 001,006 */ 30, 0x00,
  /* ABS: 003 Pixels @ 031,006 */ 0, 3, 0x0A, 0x4B, 0x11,
  /* RLE: 028 Pixels @ 002,007 */ 28, 0x13,
  /* ABS: 004 Pixels @ 030,007 */ 0, 4, 0x11, 0x4D, 0xFC, 0xF0,
  /* RLE: 028 Pixels @ 002,008 */ 28, 0xF1,
  /* ABS: 002 Pixels @ 030,008 */ 0, 2, 0xF0, 0xFC,
  /* RLE: 096 Pixels @ 000,009 */ 96, 0xFF,
  /* RLE: 001 Pixels @ 000,012 */ 1, 0x81,
  /* RLE: 030 Pixels @ 001,012 */ 30, 0x47,
  /* ABS: 002 Pixels @ 031,012 */ 0, 2, 0x83, 0x07,
  /* RLE: 030 Pixels @ 001,013 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,013 */ 0, 2, 0x09, 0x0E,
  /* RLE: 030 Pixels @ 001,014 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,014 */ 0, 2, 0x10, 0x0F,
  /* RLE: 030 Pixels @ 001,015 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,015 */ 0, 2, 0x11, 0x0F,
  /* RLE: 030 Pixels @ 001,016 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,016 */ 0, 2, 0x11, 0x07,
  /* RLE: 030 Pixels @ 001,017 */ 30, 0x00,
  /* ABS: 003 Pixels @ 031,017 */ 0, 3, 0x09, 0x5B, 0x21,
  /* RLE: 028 Pixels @ 002,018 */ 28, 0x22,
  /* ABS: 003 Pixels @ 030,018 */ 0, 3, 0x21, 0x5D, 0xFF,
  /* RLE: 030 Pixels @ 001,019 */ 30, 0xF5,
  /* RLE: 066 Pixels @ 031,019 */ 66, 0xFF,
  /* RLE: 030 Pixels @ 001,022 */ 30, 0xFB,
  /* ABS: 004 Pixels @ 031,022 */ 0, 4, 0xFF, 0x70, 0x35, 0x34,
  /* RLE: 026 Pixels @ 003,023 */ 26, 0x35,
  /* ABS: 004 Pixels @ 029,023 */ 0, 4, 0x34, 0x35, 0x71, 0x07,
  /* RLE: 030 Pixels @ 001,024 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,024 */ 0, 2, 0x09, 0x0E,
  /* RLE: 030 Pixels @ 001,025 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,025 */ 0, 2, 0x10, 0x0F,
  /* RLE: 030 Pixels @ 001,026 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,026 */ 0, 2, 0x11, 0x0E,
  /* RLE: 030 Pixels @ 001,027 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,027 */ 0, 2, 0x10, 0x06,
  /* RLE: 030 Pixels @ 001,028 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,028 */ 0, 2, 0x09, 0x6D,
  /* RLE: 030 Pixels @ 001,029 */ 30, 0x38,
  /* RLE: 001 Pixels @ 031,029 */ 1, 0x70,
  /* RLE: 064 Pixels @ 000,030 */ 64, 0xFF,
  0
};  // 164 bytes for 1024 pixels

GUI_CONST_STORAGE GUI_BITMAP bm3 = {
  32, // xSize
  32, // ySize
  32, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_ac3,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*********************************************************************
*
*       bm4
*/
static GUI_CONST_STORAGE unsigned char _ac4[] = {
  /* RLE: 034 Pixels @ 000,000 */ 34, 0xFF,
  /* ABS: 003 Pixels @ 002,001 */ 0, 3, 0xC5, 0x56, 0x2B,
  /* RLE: 012 Pixels @ 005,001 */ 12, 0x2D,
  /* ABS: 003 Pixels @ 017,001 */ 0, 3, 0x23, 0x81, 0xFD,
  /* RLE: 004 Pixels @ 020,001 */ 4, 0xFF,
  /* ABS: 005 Pixels @ 024,001 */ 0, 5, 0xFD, 0x6B, 0x54, 0x8B, 0xD6,
  /* RLE: 004 Pixels @ 029,001 */ 4, 0xFF,
  /* ABS: 002 Pixels @ 001,002 */ 0, 2, 0xC5, 0x0C,
  /* RLE: 014 Pixels @ 003,002 */ 14, 0x00,
  /* ABS: 002 Pixels @ 017,002 */ 0, 2, 0x46, 0xEB,
  /* RLE: 004 Pixels @ 019,002 */ 4, 0xFF,
  /* ABS: 011 Pixels @ 023,002 */ 0, 11, 0xF7, 0x62, 0x00, 0x00, 0x00, 0x12, 0x9A, 0xFF, 0xFF, 0xFF, 0x52,
  /* RLE: 014 Pixels @ 002,003 */ 14, 0x00,
  /* ABS: 002 Pixels @ 016,003 */ 0, 2, 0x57, 0xF3,
  /* RLE: 005 Pixels @ 018,003 */ 5, 0xFF,
  /* ABS: 002 Pixels @ 023,003 */ 0, 2, 0xEF, 0x54,
  /* RLE: 004 Pixels @ 025,003 */ 4, 0x00,
  /* ABS: 005 Pixels @ 029,003 */ 0, 5, 0x04, 0xCB, 0xFF, 0xFF, 0x32,
  /* RLE: 013 Pixels @ 002,004 */ 13, 0x00,
  /* RLE: 001 Pixels @ 015,004 */ 1, 0x7F,
  /* RLE: 005 Pixels @ 016,004 */ 5, 0xFF,
  /* ABS: 018 Pixels @ 021,004 */ 0, 18, 0xFB, 0xD5, 0xFF, 0xFF, 0x94, 0x07, 0x00, 0x00, 0x00, 0x6D, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0x6D, 0xAB,
  /* RLE: 007 Pixels @ 007,005 */ 7, 0xA8,
  /* RLE: 001 Pixels @ 014,005 */ 1, 0xB7,
  /* RLE: 006 Pixels @ 015,005 */ 6, 0xFF,
  /* ABS: 017 Pixels @ 021,005 */ 0, 17, 0x6C, 0x0B, 0x6B, 0xF1, 0xFF, 0xAC, 0x0A, 0x00, 0x00, 0x36, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xAD,
  /* RLE: 014 Pixels @ 006,006 */ 14, 0xFF,
  /* ABS: 018 Pixels @ 020,006 */ 0, 18, 0x6D, 0x00, 0x00, 0x00, 0x37, 0xE0, 0xFF, 0xA2, 0x02, 0x00, 0x30, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5,
  /* RLE: 012 Pixels @ 006,007 */ 12, 0xFF,
  /* ABS: 002 Pixels @ 018,007 */ 0, 2, 0xFE, 0x6C,
  /* RLE: 005 Pixels @ 020,007 */ 5, 0x00,
  /* ABS: 013 Pixels @ 025,007 */ 0, 13, 0x31, 0xEA, 0xFF, 0x6B, 0x19, 0xCA, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5,
  /* RLE: 011 Pixels @ 006,008 */ 11, 0xFF,
  /* ABS: 002 Pixels @ 017,008 */ 0, 2, 0xFE, 0x6C,
  /* RLE: 007 Pixels @ 019,008 */ 7, 0x00,
  /* ABS: 012 Pixels @ 026,008 */ 0, 12, 0x58, 0xFE, 0xEE, 0xD4, 0xFF, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5,
  /* RLE: 011 Pixels @ 006,009 */ 11, 0xFF,
  /* RLE: 001 Pixels @ 017,009 */ 1, 0x6C,
  /* RLE: 009 Pixels @ 018,009 */ 9, 0x00,
  /* RLE: 001 Pixels @ 027,009 */ 1, 0xAD,
  /* RLE: 005 Pixels @ 028,009 */ 5, 0xFF,
  /* ABS: 005 Pixels @ 001,010 */ 0, 5, 0x35, 0x00, 0x00, 0x00, 0xA5,
  /* RLE: 009 Pixels @ 006,010 */ 9, 0xFF,
  /* ABS: 002 Pixels @ 015,010 */ 0, 2, 0xEB, 0x5D,
  /* RLE: 009 Pixels @ 017,010 */ 9, 0x00,
  /* ABS: 002 Pixels @ 026,010 */ 0, 2, 0x1C, 0xCA,
  /* RLE: 005 Pixels @ 028,010 */ 5, 0xFF,
  /* ABS: 005 Pixels @ 001,011 */ 0, 5, 0x35, 0x00, 0x00, 0x00, 0xA5,
  /* RLE: 008 Pixels @ 006,011 */ 8, 0xFF,
  /* ABS: 002 Pixels @ 014,011 */ 0, 2, 0xEB, 0x3E,
  /* RLE: 009 Pixels @ 016,011 */ 9, 0x00,
  /* ABS: 002 Pixels @ 025,011 */ 0, 2, 0x1D, 0xCE,
  /* RLE: 006 Pixels @ 027,011 */ 6, 0xFF,
  /* ABS: 005 Pixels @ 001,012 */ 0, 5, 0x35, 0x00, 0x00, 0x00, 0xA5,
  /* RLE: 007 Pixels @ 006,012 */ 7, 0xFF,
  /* ABS: 002 Pixels @ 013,012 */ 0, 2, 0xEB, 0x3E,
  /* RLE: 009 Pixels @ 015,012 */ 9, 0x00,
  /* ABS: 002 Pixels @ 024,012 */ 0, 2, 0x1D, 0xCE,
  /* RLE: 007 Pixels @ 026,012 */ 7, 0xFF,
  /* ABS: 005 Pixels @ 001,013 */ 0, 5, 0x35, 0x00, 0x00, 0x00, 0xA5,
  /* RLE: 006 Pixels @ 006,013 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 012,013 */ 0, 2, 0xEB, 0x3F,
  /* RLE: 009 Pixels @ 014,013 */ 9, 0x00,
  /* ABS: 002 Pixels @ 023,013 */ 0, 2, 0x1D, 0xCD,
  /* RLE: 005 Pixels @ 025,013 */ 5, 0xFF,
  /* ABS: 008 Pixels @ 030,013 */ 0, 8, 0xC0, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5,
  /* RLE: 005 Pixels @ 006,014 */ 5, 0xFF,
  /* ABS: 002 Pixels @ 011,014 */ 0, 2, 0xEB, 0x3E,
  /* RLE: 009 Pixels @ 013,014 */ 9, 0x00,
  /* ABS: 002 Pixels @ 022,014 */ 0, 2, 0x1D, 0xCE,
  /* RLE: 005 Pixels @ 024,014 */ 5, 0xFF,
  /* ABS: 009 Pixels @ 029,014 */ 0, 9, 0x9F, 0x23, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5,
  /* RLE: 004 Pixels @ 006,015 */ 4, 0xFF,
  /* ABS: 002 Pixels @ 010,015 */ 0, 2, 0xE7, 0x3E,
  /* RLE: 009 Pixels @ 012,015 */ 9, 0x00,
  /* ABS: 002 Pixels @ 021,015 */ 0, 2, 0x1B, 0xCD,
  /* RLE: 005 Pixels @ 023,015 */ 5, 0xFF,
  /* ABS: 010 Pixels @ 028,015 */ 0, 10, 0xA9, 0x04, 0x1B, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5,
  /* RLE: 004 Pixels @ 006,016 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 010,016 */ 1, 0x74,
  /* RLE: 009 Pixels @ 011,016 */ 9, 0x00,
  /* ABS: 002 Pixels @ 020,016 */ 0, 2, 0x05, 0xB4,
  /* RLE: 005 Pixels @ 022,016 */ 5, 0xFF,
  /* ABS: 011 Pixels @ 027,016 */ 0, 11, 0xD1, 0x10, 0x00, 0x22, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5,
  /* RLE: 004 Pixels @ 006,017 */ 4, 0xFF,
  /* ABS: 002 Pixels @ 010,017 */ 0, 2, 0xDF, 0x5B,
  /* RLE: 007 Pixels @ 012,017 */ 7, 0x00,
  /* ABS: 002 Pixels @ 019,017 */ 0, 2, 0x05, 0xA1,
  /* RLE: 005 Pixels @ 021,017 */ 5, 0xFF,
  /* ABS: 019 Pixels @ 026,017 */ 0, 19, 0xED, 0x28, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5, 0xFF, 0xFB, 0x86, 0xAF, 0xFF, 0xFF, 0x85,
  /* RLE: 005 Pixels @ 013,018 */ 5, 0x00,
  /* ABS: 002 Pixels @ 018,018 */ 0, 2, 0x04, 0xA1,
  /* RLE: 006 Pixels @ 020,018 */ 6, 0xFF,
  /* ABS: 025 Pixels @ 026,018 */ 0, 25, 0xDB, 0x04, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5, 0xFF, 0xF0, 0x03, 0x00, 0x68, 0xF6, 0xFF, 0x8D, 0x00, 0x00, 0x00, 0x05, 0xA1,
  /* RLE: 007 Pixels @ 019,019 */ 7, 0xFF,
  /* ABS: 024 Pixels @ 026,019 */ 0, 24, 0xDD, 0x0A, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5, 0xFF, 0xF0, 0x0F, 0x00, 0x00, 0x4A, 0xF3, 0xFF, 0x6D, 0x00, 0x04, 0xA1,
  /* RLE: 008 Pixels @ 018,020 */ 8, 0xFF,
  /* ABS: 023 Pixels @ 026,020 */ 0, 23, 0xDD, 0x0A, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5, 0xFF, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x5C, 0xFF, 0xF4, 0x51, 0xA9,
  /* RLE: 009 Pixels @ 017,021 */ 9, 0xFF,
  /* ABS: 015 Pixels @ 026,021 */ 0, 15, 0xDD, 0x0A, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5, 0xFF, 0xF0, 0x0D,
  /* RLE: 004 Pixels @ 009,022 */ 4, 0x00,
  /* RLE: 001 Pixels @ 013,022 */ 1, 0x9B,
  /* RLE: 012 Pixels @ 014,022 */ 12, 0xFF,
  /* ABS: 015 Pixels @ 026,022 */ 0, 15, 0xDD, 0x0A, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5, 0xFF, 0xF0, 0x0E,
  /* RLE: 004 Pixels @ 009,023 */ 4, 0x00,
  /* ABS: 002 Pixels @ 013,023 */ 0, 2, 0x3E, 0xFC,
  /* RLE: 011 Pixels @ 015,023 */ 11, 0xFF,
  /* ABS: 015 Pixels @ 026,023 */ 0, 15, 0xDD, 0x0A, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA5, 0xFF, 0xFB, 0xBC,
  /* RLE: 004 Pixels @ 009,024 */ 4, 0xB7,
  /* RLE: 001 Pixels @ 013,024 */ 1, 0xD8,
  /* RLE: 012 Pixels @ 014,024 */ 12, 0xFF,
  /* ABS: 012 Pixels @ 026,024 */ 0, 12, 0xDD, 0x0A, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0xA9,
  /* RLE: 020 Pixels @ 006,025 */ 20, 0xFF,
  /* ABS: 013 Pixels @ 026,025 */ 0, 13, 0xE3, 0x0A, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0x35, 0x00, 0x00, 0x00, 0x8F, 0xDF,
  /* RLE: 018 Pixels @ 007,026 */ 18, 0xDD,
  /* ABS: 013 Pixels @ 025,026 */ 0, 13, 0xDE, 0xBF, 0x09, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0x33, 0x00, 0x00, 0x00, 0x06,
  /* RLE: 020 Pixels @ 006,027 */ 20, 0x0A,
  /* ABS: 008 Pixels @ 026,027 */ 0, 8, 0x09, 0x00, 0x00, 0x00, 0x21, 0xFF, 0xFF, 0x40,
  /* RLE: 028 Pixels @ 002,028 */ 28, 0x00,
  /* ABS: 004 Pixels @ 030,028 */ 0, 4, 0x25, 0xFF, 0xFF, 0xA8,
  /* RLE: 028 Pixels @ 002,029 */ 28, 0x00,
  /* ABS: 007 Pixels @ 030,029 */ 0, 7, 0x71, 0xFF, 0xFF, 0xFF, 0x93, 0x33, 0x21,
  /* RLE: 022 Pixels @ 005,030 */ 22, 0x22,
  /* ABS: 004 Pixels @ 027,030 */ 0, 4, 0x21, 0x2B, 0x72, 0xF2,
  /* RLE: 033 Pixels @ 031,030 */ 33, 0xFF,
  0
};  // 656 bytes for 1024 pixels

GUI_CONST_STORAGE GUI_BITMAP bm4 = {
  32, // xSize
  32, // ySize
  32, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_ac4,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*********************************************************************
*
*       bm5
*/
static GUI_CONST_STORAGE unsigned char _ac5[] = {
  /* RLE: 041 Pixels @ 000,000 */ 41, 0xFF,
  /* ABS: 004 Pixels @ 009,001 */ 0, 4, 0xF7, 0x8E, 0x3E, 0x2B,
  /* RLE: 006 Pixels @ 013,001 */ 6, 0x2D,
  /* ABS: 004 Pixels @ 019,001 */ 0, 4, 0x2C, 0x35, 0x7B, 0xEA,
  /* RLE: 018 Pixels @ 023,001 */ 18, 0xFF,
  /* RLE: 001 Pixels @ 009,002 */ 1, 0x67,
  /* RLE: 012 Pixels @ 010,002 */ 12, 0x00,
  /* ABS: 002 Pixels @ 022,002 */ 0, 2, 0x41, 0xF8,
  /* RLE: 016 Pixels @ 024,002 */ 16, 0xFF,
  /* ABS: 002 Pixels @ 008,003 */ 0, 2, 0xD0, 0x04,
  /* RLE: 013 Pixels @ 010,003 */ 13, 0x00,
  /* RLE: 001 Pixels @ 023,003 */ 1, 0xA9,
  /* RLE: 016 Pixels @ 024,003 */ 16, 0xFF,
  /* RLE: 001 Pixels @ 008,004 */ 1, 0x62,
  /* RLE: 014 Pixels @ 009,004 */ 14, 0x00,
  /* ABS: 002 Pixels @ 023,004 */ 0, 2, 0x3A, 0xFD,
  /* RLE: 008 Pixels @ 025,004 */ 8, 0xFF,
  /* ABS: 008 Pixels @ 001,005 */ 0, 8, 0xEC, 0x91, 0x61, 0x56, 0x56, 0x57, 0x53, 0x09,
  /* RLE: 015 Pixels @ 009,005 */ 15, 0x00,
  /* ABS: 010 Pixels @ 024,005 */ 0, 10, 0x4A, 0x57, 0x56, 0x56, 0x61, 0x8D, 0xE7, 0xFF, 0xD7, 0x2C,
  /* RLE: 028 Pixels @ 002,006 */ 28, 0x00,
  /* ABS: 003 Pixels @ 030,006 */ 0, 3, 0x26, 0xD0, 0x43,
  /* RLE: 030 Pixels @ 001,007 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,007 */ 0, 2, 0x3E, 0x0B,
  /* RLE: 030 Pixels @ 001,008 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,008 */ 0, 2, 0x0B, 0x0F,
  /* RLE: 011 Pixels @ 001,009 */ 11, 0x00,
  /* ABS: 008 Pixels @ 012,009 */ 0, 8, 0x07, 0x48, 0x83, 0xA0, 0xA0, 0x83, 0x48, 0x07,
  /* RLE: 011 Pixels @ 020,009 */ 11, 0x00,
  /* ABS: 002 Pixels @ 031,009 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 010 Pixels @ 001,010 */ 10, 0x00,
  /* ABS: 002 Pixels @ 011,010 */ 0, 2, 0x38, 0xCD,
  /* RLE: 006 Pixels @ 013,010 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 019,010 */ 0, 2, 0xCD, 0x37,
  /* RLE: 010 Pixels @ 021,010 */ 10, 0x00,
  /* ABS: 002 Pixels @ 031,010 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 009 Pixels @ 001,011 */ 9, 0x00,
  /* ABS: 002 Pixels @ 010,011 */ 0, 2, 0x47, 0xE8,
  /* RLE: 008 Pixels @ 012,011 */ 8, 0xFF,
  /* ABS: 002 Pixels @ 020,011 */ 0, 2, 0xE7, 0x46,
  /* RLE: 009 Pixels @ 022,011 */ 9, 0x00,
  /* ABS: 002 Pixels @ 031,011 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 008 Pixels @ 001,012 */ 8, 0x00,
  /* ABS: 014 Pixels @ 009,012 */ 0, 14, 0x39, 0xF1, 0xFF, 0xFF, 0xD1, 0x7C, 0x4E, 0x4E, 0x7C, 0xD2, 0xFF, 0xFF, 0xF1, 0x38,
  /* RLE: 008 Pixels @ 023,012 */ 8, 0x00,
  /* ABS: 002 Pixels @ 031,012 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 007 Pixels @ 001,013 */ 7, 0x00,
  /* ABS: 006 Pixels @ 008,013 */ 0, 6, 0x0B, 0xD0, 0xFF, 0xFF, 0x8F, 0x0D,
  /* RLE: 004 Pixels @ 014,013 */ 4, 0x00,
  /* ABS: 006 Pixels @ 018,013 */ 0, 6, 0x0E, 0x90, 0xFF, 0xFF, 0xD0, 0x0A,
  /* RLE: 007 Pixels @ 024,013 */ 7, 0x00,
  /* ABS: 002 Pixels @ 031,013 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 007 Pixels @ 001,014 */ 7, 0x00,
  /* ABS: 004 Pixels @ 008,014 */ 0, 4, 0x60, 0xFF, 0xFF, 0xA6,
  /* RLE: 008 Pixels @ 012,014 */ 8, 0x00,
  /* ABS: 004 Pixels @ 020,014 */ 0, 4, 0xA6, 0xFF, 0xFF, 0x60,
  /* RLE: 007 Pixels @ 024,014 */ 7, 0x00,
  /* ABS: 002 Pixels @ 031,014 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 007 Pixels @ 001,015 */ 7, 0x00,
  /* ABS: 004 Pixels @ 008,015 */ 0, 4, 0xB1, 0xFF, 0xF4, 0x25,
  /* RLE: 008 Pixels @ 012,015 */ 8, 0x00,
  /* ABS: 004 Pixels @ 020,015 */ 0, 4, 0x25, 0xF4, 0xFF, 0xB0,
  /* RLE: 007 Pixels @ 024,015 */ 7, 0x00,
  /* ABS: 002 Pixels @ 031,015 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 006 Pixels @ 001,016 */ 6, 0x00,
  /* ABS: 004 Pixels @ 007,016 */ 0, 4, 0x09, 0xDF, 0xFF, 0xBF,
  /* RLE: 010 Pixels @ 011,016 */ 10, 0x00,
  /* ABS: 004 Pixels @ 021,016 */ 0, 4, 0xC0, 0xFF, 0xDE, 0x09,
  /* RLE: 006 Pixels @ 025,016 */ 6, 0x00,
  /* ABS: 002 Pixels @ 031,016 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 006 Pixels @ 001,017 */ 6, 0x00,
  /* ABS: 004 Pixels @ 007,017 */ 0, 4, 0x0F, 0xEF, 0xFF, 0xA4,
  /* RLE: 010 Pixels @ 011,017 */ 10, 0x00,
  /* ABS: 004 Pixels @ 021,017 */ 0, 4, 0xA5, 0xFF, 0xEE, 0x0F,
  /* RLE: 006 Pixels @ 025,017 */ 6, 0x00,
  /* ABS: 002 Pixels @ 031,017 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 006 Pixels @ 001,018 */ 6, 0x00,
  /* ABS: 004 Pixels @ 007,018 */ 0, 4, 0x0B, 0xE2, 0xFF, 0xB8,
  /* RLE: 010 Pixels @ 011,018 */ 10, 0x00,
  /* ABS: 004 Pixels @ 021,018 */ 0, 4, 0xB9, 0xFF, 0xE2, 0x0B,
  /* RLE: 006 Pixels @ 025,018 */ 6, 0x00,
  /* ABS: 002 Pixels @ 031,018 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 006 Pixels @ 001,019 */ 6, 0x00,
  /* ABS: 005 Pixels @ 007,019 */ 0, 5, 0x02, 0xBA, 0xFF, 0xED, 0x19,
  /* RLE: 008 Pixels @ 012,019 */ 8, 0x00,
  /* ABS: 005 Pixels @ 020,019 */ 0, 5, 0x1A, 0xED, 0xFF, 0xBA, 0x02,
  /* RLE: 006 Pixels @ 025,019 */ 6, 0x00,
  /* ABS: 002 Pixels @ 031,019 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 007 Pixels @ 001,020 */ 7, 0x00,
  /* ABS: 004 Pixels @ 008,020 */ 0, 4, 0x82, 0xFF, 0xFF, 0x72,
  /* RLE: 008 Pixels @ 012,020 */ 8, 0x00,
  /* ABS: 004 Pixels @ 020,020 */ 0, 4, 0x73, 0xFF, 0xFF, 0x81,
  /* RLE: 007 Pixels @ 024,020 */ 7, 0x00,
  /* ABS: 002 Pixels @ 031,020 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 007 Pixels @ 001,021 */ 7, 0x00,
  /* ABS: 005 Pixels @ 008,021 */ 0, 5, 0x28, 0xF4, 0xFF, 0xEC, 0x3F,
  /* RLE: 006 Pixels @ 013,021 */ 6, 0x00,
  /* ABS: 005 Pixels @ 019,021 */ 0, 5, 0x40, 0xED, 0xFF, 0xF3, 0x27,
  /* RLE: 007 Pixels @ 024,021 */ 7, 0x00,
  /* ABS: 002 Pixels @ 031,021 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 008 Pixels @ 001,022 */ 8, 0x00,
  /* ABS: 014 Pixels @ 009,022 */ 0, 14, 0x79, 0xFF, 0xFF, 0xF2, 0x7B, 0x20, 0x06, 0x06, 0x21, 0x7B, 0xF2, 0xFF, 0xFF, 0x79,
  /* RLE: 008 Pixels @ 023,022 */ 8, 0x00,
  /* ABS: 002 Pixels @ 031,022 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 008 Pixels @ 001,023 */ 8, 0x00,
  /* ABS: 014 Pixels @ 009,023 */ 0, 14, 0x02, 0x97, 0xFF, 0xFF, 0xFF, 0xF2, 0xD1, 0xD1, 0xF2, 0xFF, 0xFF, 0xFF, 0x96, 0x02,
  /* RLE: 008 Pixels @ 023,023 */ 8, 0x00,
  /* ABS: 002 Pixels @ 031,023 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 010 Pixels @ 001,024 */ 10, 0x00,
  /* ABS: 002 Pixels @ 011,024 */ 0, 2, 0x6D, 0xE5,
  /* RLE: 006 Pixels @ 013,024 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 019,024 */ 0, 2, 0xE5, 0x6C,
  /* RLE: 010 Pixels @ 021,024 */ 10, 0x00,
  /* ABS: 002 Pixels @ 031,024 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 011 Pixels @ 001,025 */ 11, 0x00,
  /* ABS: 008 Pixels @ 012,025 */ 0, 8, 0x1C, 0x6D, 0xA7, 0xC5, 0xC5, 0xA7, 0x6D, 0x1B,
  /* RLE: 011 Pixels @ 020,025 */ 11, 0x00,
  /* ABS: 002 Pixels @ 031,025 */ 0, 2, 0x0F, 0x08,
  /* RLE: 014 Pixels @ 001,026 */ 14, 0x00,
  /* ABS: 002 Pixels @ 015,026 */ 0, 2, 0x03, 0x03,
  /* RLE: 014 Pixels @ 017,026 */ 14, 0x00,
  /* ABS: 002 Pixels @ 031,026 */ 0, 2, 0x08, 0x55,
  /* RLE: 030 Pixels @ 001,027 */ 30, 0x00,
  /* ABS: 003 Pixels @ 031,027 */ 0, 3, 0x52, 0xF2, 0x58,
  /* RLE: 028 Pixels @ 002,028 */ 28, 0x00,
  /* ABS: 007 Pixels @ 030,028 */ 0, 7, 0x51, 0xEF, 0xFF, 0xFE, 0x9D, 0x46, 0x37,
  /* RLE: 022 Pixels @ 005,029 */ 22, 0x38,
  /* ABS: 004 Pixels @ 027,029 */ 0, 4, 0x37, 0x45, 0x96, 0xFC,
  /* RLE: 065 Pixels @ 031,029 */ 65, 0xFF,
  0
};  // 499 bytes for 1024 pixels

GUI_CONST_STORAGE GUI_BITMAP bm5 = {
  32, // xSize
  32, // ySize
  32, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_ac5,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*********************************************************************
*
*       bm6
*/
static GUI_CONST_STORAGE unsigned char _ac6[] = {
  /* RLE: 013 Pixels @ 000,000 */ 13, 0xFF,
  /* ABS: 006 Pixels @ 013,000 */ 0, 6, 0xF3, 0x46, 0x11, 0x11, 0x46, 0xF3,
  /* RLE: 026 Pixels @ 019,000 */ 26, 0xFF,
  /* RLE: 001 Pixels @ 013,001 */ 1, 0xC0,
  /* RLE: 004 Pixels @ 014,001 */ 4, 0x00,
  /* RLE: 001 Pixels @ 018,001 */ 1, 0xC1,
  /* RLE: 026 Pixels @ 019,001 */ 26, 0xFF,
  /* RLE: 001 Pixels @ 013,002 */ 1, 0xA1,
  /* RLE: 004 Pixels @ 014,002 */ 4, 0x00,
  /* RLE: 001 Pixels @ 018,002 */ 1, 0xA2,
  /* RLE: 018 Pixels @ 019,002 */ 18, 0xFF,
  /* ABS: 002 Pixels @ 005,003 */ 0, 2, 0xE5, 0xC1,
  /* RLE: 006 Pixels @ 007,003 */ 6, 0xFF,
  /* RLE: 001 Pixels @ 013,003 */ 1, 0x6F,
  /* RLE: 004 Pixels @ 014,003 */ 4, 0x00,
  /* RLE: 001 Pixels @ 018,003 */ 1, 0x6F,
  /* RLE: 006 Pixels @ 019,003 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 025,003 */ 0, 2, 0xC1, 0xE5,
  /* RLE: 009 Pixels @ 027,003 */ 9, 0xFF,
  /* ABS: 010 Pixels @ 004,004 */ 0, 10, 0xD6, 0x29, 0x02, 0x58, 0xE6, 0xFF, 0xFF, 0xE0, 0x86, 0x0C,
  /* RLE: 004 Pixels @ 014,004 */ 4, 0x00,
  /* ABS: 010 Pixels @ 018,004 */ 0, 10, 0x0D, 0x87, 0xE1, 0xFF, 0xFF, 0xE6, 0x57, 0x01, 0x29, 0xD7,
  /* RLE: 007 Pixels @ 028,004 */ 7, 0xFF,
  /* ABS: 009 Pixels @ 003,005 */ 0, 9, 0xE2, 0x26, 0x00, 0x00, 0x00, 0x27, 0x83, 0x66, 0x14,
  /* RLE: 008 Pixels @ 012,005 */ 8, 0x00,
  /* ABS: 009 Pixels @ 020,005 */ 0, 9, 0x14, 0x66, 0x83, 0x27, 0x00, 0x00, 0x00, 0x26, 0xE2,
  /* RLE: 006 Pixels @ 029,005 */ 6, 0xFF,
  /* RLE: 001 Pixels @ 003,006 */ 1, 0xC1,
  /* RLE: 024 Pixels @ 004,006 */ 24, 0x00,
  /* RLE: 001 Pixels @ 028,006 */ 1, 0xC2,
  /* RLE: 006 Pixels @ 029,006 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 003,007 */ 0, 2, 0xF9, 0x5A,
  /* RLE: 022 Pixels @ 005,007 */ 22, 0x00,
  /* ABS: 002 Pixels @ 027,007 */ 0, 2, 0x59, 0xF9,
  /* RLE: 007 Pixels @ 029,007 */ 7, 0xFF,
  /* ABS: 002 Pixels @ 004,008 */ 0, 2, 0xEB, 0x2A,
  /* RLE: 020 Pixels @ 006,008 */ 20, 0x00,
  /* ABS: 002 Pixels @ 026,008 */ 0, 2, 0x2A, 0xEB,
  /* RLE: 009 Pixels @ 028,008 */ 9, 0xFF,
  /* RLE: 001 Pixels @ 005,009 */ 1, 0x88,
  /* RLE: 020 Pixels @ 006,009 */ 20, 0x00,
  /* RLE: 001 Pixels @ 026,009 */ 1, 0x89,
  /* RLE: 010 Pixels @ 027,009 */ 10, 0xFF,
  /* RLE: 001 Pixels @ 005,010 */ 1, 0x68,
  /* RLE: 020 Pixels @ 006,010 */ 20, 0x00,
  /* RLE: 001 Pixels @ 026,010 */ 1, 0x68,
  /* RLE: 009 Pixels @ 027,010 */ 9, 0xFF,
  /* ABS: 002 Pixels @ 004,011 */ 0, 2, 0xE8, 0x14,
  /* RLE: 009 Pixels @ 006,011 */ 9, 0x00,
  /* ABS: 002 Pixels @ 015,011 */ 0, 2, 0x0C, 0x0C,
  /* RLE: 009 Pixels @ 017,011 */ 9, 0x00,
  /* ABS: 002 Pixels @ 026,011 */ 0, 2, 0x14, 0xE9,
  /* RLE: 008 Pixels @ 028,011 */ 8, 0xFF,
  /* RLE: 001 Pixels @ 004,012 */ 1, 0x86,
  /* RLE: 008 Pixels @ 005,012 */ 8, 0x00,
  /* ABS: 006 Pixels @ 013,012 */ 0, 6, 0x31, 0xAB, 0xE5, 0xE5, 0xAA, 0x2F,
  /* RLE: 008 Pixels @ 019,012 */ 8, 0x00,
  /* RLE: 001 Pixels @ 027,012 */ 1, 0x87,
  /* RLE: 004 Pixels @ 028,012 */ 4, 0xFF,
  /* ABS: 005 Pixels @ 000,013 */ 0, 5, 0xF6, 0xC1, 0x9F, 0x6E, 0x0A,
  /* RLE: 007 Pixels @ 005,013 */ 7, 0x00,
  /* ABS: 002 Pixels @ 012,013 */ 0, 2, 0x2F, 0xE8,
  /* RLE: 004 Pixels @ 014,013 */ 4, 0xFF,
  /* ABS: 002 Pixels @ 018,013 */ 0, 2, 0xE7, 0x2C,
  /* RLE: 007 Pixels @ 020,013 */ 7, 0x00,
  /* ABS: 007 Pixels @ 027,013 */ 0, 7, 0x0A, 0x6E, 0xA0, 0xC2, 0xF6, 0x50, 0x02,
  /* RLE: 009 Pixels @ 002,014 */ 9, 0x00,
  /* ABS: 002 Pixels @ 011,014 */ 0, 2, 0x01, 0xAD,
  /* RLE: 006 Pixels @ 013,014 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 019,014 */ 0, 2, 0xAB, 0x01,
  /* RLE: 009 Pixels @ 021,014 */ 9, 0x00,
  /* ABS: 003 Pixels @ 030,014 */ 0, 3, 0x02, 0x51, 0x0D,
  /* RLE: 010 Pixels @ 001,015 */ 10, 0x00,
  /* ABS: 002 Pixels @ 011,015 */ 0, 2, 0x0C, 0xE9,
  /* RLE: 006 Pixels @ 013,015 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 019,015 */ 0, 2, 0xE9, 0x0C,
  /* RLE: 010 Pixels @ 021,015 */ 10, 0x00,
  /* ABS: 002 Pixels @ 031,015 */ 0, 2, 0x0D, 0x0D,
  /* RLE: 010 Pixels @ 001,016 */ 10, 0x00,
  /* ABS: 002 Pixels @ 011,016 */ 0, 2, 0x0C, 0xE9,
  /* RLE: 006 Pixels @ 013,016 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 019,016 */ 0, 2, 0xE9, 0x0C,
  /* RLE: 010 Pixels @ 021,016 */ 10, 0x00,
  /* ABS: 003 Pixels @ 031,016 */ 0, 3, 0x0D, 0x50, 0x02,
  /* RLE: 009 Pixels @ 002,017 */ 9, 0x00,
  /* ABS: 002 Pixels @ 011,017 */ 0, 2, 0x01, 0xAD,
  /* RLE: 006 Pixels @ 013,017 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 019,017 */ 0, 2, 0xAC, 0x01,
  /* RLE: 009 Pixels @ 021,017 */ 9, 0x00,
  /* ABS: 007 Pixels @ 030,017 */ 0, 7, 0x02, 0x51, 0xF6, 0xC1, 0x9F, 0x6E, 0x0A,
  /* RLE: 007 Pixels @ 005,018 */ 7, 0x00,
  /* ABS: 002 Pixels @ 012,018 */ 0, 2, 0x2D, 0xE6,
  /* RLE: 004 Pixels @ 014,018 */ 4, 0xFF,
  /* ABS: 002 Pixels @ 018,018 */ 0, 2, 0xE6, 0x2D,
  /* RLE: 007 Pixels @ 020,018 */ 7, 0x00,
  /* ABS: 005 Pixels @ 027,018 */ 0, 5, 0x0A, 0x6E, 0xA0, 0xC2, 0xF6,
  /* RLE: 004 Pixels @ 000,019 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 004,019 */ 1, 0x86,
  /* RLE: 008 Pixels @ 005,019 */ 8, 0x00,
  /* ABS: 006 Pixels @ 013,019 */ 0, 6, 0x2D, 0xA8, 0xE6, 0xE6, 0xA8, 0x2D,
  /* RLE: 008 Pixels @ 019,019 */ 8, 0x00,
  /* RLE: 001 Pixels @ 027,019 */ 1, 0x87,
  /* RLE: 008 Pixels @ 028,019 */ 8, 0xFF,
  /* ABS: 002 Pixels @ 004,020 */ 0, 2, 0xE9, 0x15,
  /* RLE: 009 Pixels @ 006,020 */ 9, 0x00,
  /* ABS: 002 Pixels @ 015,020 */ 0, 2, 0x0D, 0x0D,
  /* RLE: 009 Pixels @ 017,020 */ 9, 0x00,
  /* ABS: 002 Pixels @ 026,020 */ 0, 2, 0x16, 0xEA,
  /* RLE: 009 Pixels @ 028,020 */ 9, 0xFF,
  /* RLE: 001 Pixels @ 005,021 */ 1, 0x69,
  /* RLE: 020 Pixels @ 006,021 */ 20, 0x00,
  /* RLE: 001 Pixels @ 026,021 */ 1, 0x6A,
  /* RLE: 010 Pixels @ 027,021 */ 10, 0xFF,
  /* RLE: 001 Pixels @ 005,022 */ 1, 0x88,
  /* RLE: 020 Pixels @ 006,022 */ 20, 0x00,
  /* RLE: 001 Pixels @ 026,022 */ 1, 0x89,
  /* RLE: 009 Pixels @ 027,022 */ 9, 0xFF,
  /* ABS: 002 Pixels @ 004,023 */ 0, 2, 0xE9, 0x29,
  /* RLE: 020 Pixels @ 006,023 */ 20, 0x00,
  /* ABS: 002 Pixels @ 026,023 */ 0, 2, 0x29, 0xE9,
  /* RLE: 007 Pixels @ 028,023 */ 7, 0xFF,
  /* ABS: 002 Pixels @ 003,024 */ 0, 2, 0xFA, 0x55,
  /* RLE: 022 Pixels @ 005,024 */ 22, 0x00,
  /* ABS: 002 Pixels @ 027,024 */ 0, 2, 0x56, 0xF9,
  /* RLE: 006 Pixels @ 029,024 */ 6, 0xFF,
  /* RLE: 001 Pixels @ 003,025 */ 1, 0xB5,
  /* RLE: 024 Pixels @ 004,025 */ 24, 0x00,
  /* RLE: 001 Pixels @ 028,025 */ 1, 0xC2,
  /* RLE: 006 Pixels @ 029,025 */ 6, 0xFF,
  /* ABS: 009 Pixels @ 003,026 */ 0, 9, 0xD7, 0x1D, 0x00, 0x00, 0x00, 0x2A, 0x83, 0x67, 0x18,
  /* RLE: 008 Pixels @ 012,026 */ 8, 0x00,
  /* ABS: 009 Pixels @ 020,026 */ 0, 9, 0x18, 0x67, 0x83, 0x2A, 0x00, 0x00, 0x00, 0x28, 0xE3,
  /* RLE: 007 Pixels @ 029,026 */ 7, 0xFF,
  /* ABS: 010 Pixels @ 004,027 */ 0, 10, 0xC9, 0x28, 0x07, 0x58, 0xE8, 0xFF, 0xFF, 0xE5, 0x87, 0x0C,
  /* RLE: 004 Pixels @ 014,027 */ 4, 0x00,
  /* ABS: 010 Pixels @ 018,027 */ 0, 10, 0x0C, 0x87, 0xE6, 0xFF, 0xFF, 0xE8, 0x57, 0x02, 0x29, 0xD8,
  /* RLE: 009 Pixels @ 028,027 */ 9, 0xFF,
  /* ABS: 002 Pixels @ 005,028 */ 0, 2, 0xF1, 0xD3,
  /* RLE: 006 Pixels @ 007,028 */ 6, 0xFF,
  /* RLE: 001 Pixels @ 013,028 */ 1, 0x6F,
  /* RLE: 004 Pixels @ 014,028 */ 4, 0x00,
  /* RLE: 001 Pixels @ 018,028 */ 1, 0x70,
  /* RLE: 006 Pixels @ 019,028 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 025,028 */ 0, 2, 0xC1, 0xE5,
  /* RLE: 018 Pixels @ 027,028 */ 18, 0xFF,
  /* RLE: 001 Pixels @ 013,029 */ 1, 0xA1,
  /* RLE: 004 Pixels @ 014,029 */ 4, 0x00,
  /* RLE: 001 Pixels @ 018,029 */ 1, 0xA2,
  /* RLE: 026 Pixels @ 019,029 */ 26, 0xFF,
  /* RLE: 001 Pixels @ 013,030 */ 1, 0xC0,
  /* RLE: 004 Pixels @ 014,030 */ 4, 0x00,
  /* RLE: 001 Pixels @ 018,030 */ 1, 0xC2,
  /* RLE: 026 Pixels @ 019,030 */ 26, 0xFF,
  /* ABS: 006 Pixels @ 013,031 */ 0, 6, 0xF3, 0x50, 0x13, 0x13, 0x51, 0xF3,
  /* RLE: 013 Pixels @ 019,031 */ 13, 0xFF,
  0
};  // 502 bytes for 1024 pixels

GUI_CONST_STORAGE GUI_BITMAP bm6 = {
  32, // xSize
  32, // ySize
  32, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_ac6,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*********************************************************************
*
*       bm7
*/
static GUI_CONST_STORAGE unsigned char _ac7[] = {
  /* RLE: 010 Pixels @ 000,000 */ 10, 0xFF,
  /* ABS: 012 Pixels @ 010,000 */ 0, 12, 0xE4, 0xA1, 0x5A, 0x25, 0x17, 0x11, 0x11, 0x17, 0x25, 0x5B, 0xA1, 0xE4,
  /* RLE: 018 Pixels @ 022,000 */ 18, 0xFF,
  /* ABS: 003 Pixels @ 008,001 */ 0, 3, 0xCD, 0x64, 0x14,
  /* RLE: 010 Pixels @ 011,001 */ 10, 0x00,
  /* ABS: 003 Pixels @ 021,001 */ 0, 3, 0x14, 0x65, 0xCD,
  /* RLE: 014 Pixels @ 024,001 */ 14, 0xFF,
  /* ABS: 003 Pixels @ 006,002 */ 0, 3, 0xEF, 0x71, 0x0A,
  /* RLE: 014 Pixels @ 009,002 */ 14, 0x00,
  /* ABS: 003 Pixels @ 023,002 */ 0, 3, 0x0A, 0x72, 0xEF,
  /* RLE: 011 Pixels @ 026,002 */ 11, 0xFF,
  /* ABS: 002 Pixels @ 005,003 */ 0, 2, 0xD2, 0x34,
  /* RLE: 018 Pixels @ 007,003 */ 18, 0x00,
  /* ABS: 002 Pixels @ 025,003 */ 0, 2, 0x35, 0xD3,
  /* RLE: 009 Pixels @ 027,003 */ 9, 0xFF,
  /* ABS: 002 Pixels @ 004,004 */ 0, 2, 0xC8, 0x1C,
  /* RLE: 020 Pixels @ 006,004 */ 20, 0x00,
  /* ABS: 002 Pixels @ 026,004 */ 0, 2, 0x1D, 0xC8,
  /* RLE: 007 Pixels @ 028,004 */ 7, 0xFF,
  /* ABS: 002 Pixels @ 003,005 */ 0, 2, 0xD6, 0x1C,
  /* RLE: 008 Pixels @ 005,005 */ 8, 0x00,
  /* ABS: 004 Pixels @ 013,005 */ 0, 4, 0x0A, 0x87, 0xA2, 0x2E,
  /* RLE: 010 Pixels @ 017,005 */ 10, 0x00,
  /* ABS: 002 Pixels @ 027,005 */ 0, 2, 0x1C, 0xD6,
  /* RLE: 005 Pixels @ 029,005 */ 5, 0xFF,
  /* ABS: 002 Pixels @ 002,006 */ 0, 2, 0xF0, 0x33,
  /* RLE: 009 Pixels @ 004,006 */ 9, 0x00,
  /* ABS: 004 Pixels @ 013,006 */ 0, 4, 0x3B, 0xFF, 0xFF, 0x91,
  /* RLE: 011 Pixels @ 017,006 */ 11, 0x00,
  /* ABS: 002 Pixels @ 028,006 */ 0, 2, 0x33, 0xF1,
  /* RLE: 004 Pixels @ 030,006 */ 4, 0xFF,
  /* RLE: 001 Pixels @ 002,007 */ 1, 0x73,
  /* RLE: 010 Pixels @ 003,007 */ 10, 0x00,
  /* ABS: 004 Pixels @ 013,007 */ 0, 4, 0x4B, 0xFF, 0xFF, 0x9D,
  /* RLE: 012 Pixels @ 017,007 */ 12, 0x00,
  /* ABS: 006 Pixels @ 029,007 */ 0, 6, 0x77, 0xFF, 0xFF, 0xFF, 0xD1, 0x08,
  /* RLE: 010 Pixels @ 003,008 */ 10, 0x00,
  /* ABS: 004 Pixels @ 013,008 */ 0, 4, 0x58, 0xFF, 0xFF, 0xA9,
  /* RLE: 012 Pixels @ 017,008 */ 12, 0x00,
  /* ABS: 005 Pixels @ 029,008 */ 0, 5, 0x09, 0xD4, 0xFF, 0xFF, 0x63,
  /* RLE: 011 Pixels @ 002,009 */ 11, 0x00,
  /* ABS: 004 Pixels @ 013,009 */ 0, 4, 0x65, 0xFF, 0xFF, 0xB6,
  /* RLE: 013 Pixels @ 017,009 */ 13, 0x00,
  /* ABS: 004 Pixels @ 030,009 */ 0, 4, 0x66, 0xFF, 0xE5, 0x10,
  /* RLE: 011 Pixels @ 002,010 */ 11, 0x00,
  /* ABS: 005 Pixels @ 013,010 */ 0, 5, 0x72, 0xFF, 0xFF, 0xC3, 0x02,
  /* RLE: 012 Pixels @ 018,010 */ 12, 0x00,
  /* ABS: 003 Pixels @ 030,010 */ 0, 3, 0x11, 0xE6, 0xA1,
  /* RLE: 012 Pixels @ 001,011 */ 12, 0x00,
  /* ABS: 005 Pixels @ 013,011 */ 0, 5, 0x7F, 0xFF, 0xFF, 0xD0, 0x06,
  /* RLE: 013 Pixels @ 018,011 */ 13, 0x00,
  /* ABS: 002 Pixels @ 031,011 */ 0, 2, 0xA1, 0x66,
  /* RLE: 012 Pixels @ 001,012 */ 12, 0x00,
  /* ABS: 005 Pixels @ 013,012 */ 0, 5, 0x8D, 0xFF, 0xFF, 0xDD, 0x09,
  /* RLE: 013 Pixels @ 018,012 */ 13, 0x00,
  /* ABS: 002 Pixels @ 031,012 */ 0, 2, 0x67, 0x3B,
  /* RLE: 012 Pixels @ 001,013 */ 12, 0x00,
  /* ABS: 005 Pixels @ 013,013 */ 0, 5, 0x9A, 0xFF, 0xFF, 0xEA, 0x0B,
  /* RLE: 013 Pixels @ 018,013 */ 13, 0x00,
  /* ABS: 002 Pixels @ 031,013 */ 0, 2, 0x3C, 0x1F,
  /* RLE: 012 Pixels @ 001,014 */ 12, 0x00,
  /* ABS: 005 Pixels @ 013,014 */ 0, 5, 0xA7, 0xFF, 0xFF, 0xF5, 0x1B,
  /* RLE: 013 Pixels @ 018,014 */ 13, 0x00,
  /* ABS: 002 Pixels @ 031,014 */ 0, 2, 0x20, 0x12,
  /* RLE: 012 Pixels @ 001,015 */ 12, 0x00,
  /* ABS: 005 Pixels @ 013,015 */ 0, 5, 0xB7, 0xFF, 0xFF, 0xFF, 0x71,
  /* RLE: 013 Pixels @ 018,015 */ 13, 0x00,
  /* ABS: 002 Pixels @ 031,015 */ 0, 2, 0x13, 0x12,
  /* RLE: 012 Pixels @ 001,016 */ 12, 0x00,
  /* ABS: 006 Pixels @ 013,016 */ 0, 6, 0x9F, 0xFF, 0xFF, 0xFF, 0xEF, 0x3D,
  /* RLE: 012 Pixels @ 019,016 */ 12, 0x00,
  /* ABS: 002 Pixels @ 031,016 */ 0, 2, 0x13, 0x1F,
  /* RLE: 012 Pixels @ 001,017 */ 12, 0x00,
  /* ABS: 007 Pixels @ 013,017 */ 0, 7, 0x2A, 0xD9, 0xFF, 0xFF, 0xFF, 0xE3, 0x28,
  /* RLE: 011 Pixels @ 020,017 */ 11, 0x00,
  /* ABS: 002 Pixels @ 031,017 */ 0, 2, 0x20, 0x3B,
  /* RLE: 013 Pixels @ 001,018 */ 13, 0x00,
  /* ABS: 007 Pixels @ 014,018 */ 0, 7, 0x22, 0xC6, 0xFF, 0xFF, 0xFF, 0xD0, 0x17,
  /* RLE: 010 Pixels @ 021,018 */ 10, 0x00,
  /* ABS: 002 Pixels @ 031,018 */ 0, 2, 0x3C, 0x67,
  /* RLE: 014 Pixels @ 001,019 */ 14, 0x00,
  /* ABS: 007 Pixels @ 015,019 */ 0, 7, 0x13, 0xB1, 0xFF, 0xFF, 0xFF, 0xB9, 0x0A,
  /* RLE: 009 Pixels @ 022,019 */ 9, 0x00,
  /* ABS: 002 Pixels @ 031,019 */ 0, 2, 0x67, 0xA1,
  /* RLE: 015 Pixels @ 001,020 */ 15, 0x00,
  /* ABS: 007 Pixels @ 016,020 */ 0, 7, 0x07, 0x99, 0xFF, 0xFF, 0xFF, 0xA0, 0x06,
  /* RLE: 008 Pixels @ 023,020 */ 8, 0x00,
  /* ABS: 003 Pixels @ 031,020 */ 0, 3, 0xA2, 0xE6, 0x11,
  /* RLE: 016 Pixels @ 002,021 */ 16, 0x00,
  /* ABS: 005 Pixels @ 018,021 */ 0, 5, 0x7F, 0xFF, 0xFF, 0xFF, 0x2A,
  /* RLE: 007 Pixels @ 023,021 */ 7, 0x00,
  /* ABS: 004 Pixels @ 030,021 */ 0, 4, 0x12, 0xE7, 0xFF, 0x63,
  /* RLE: 017 Pixels @ 002,022 */ 17, 0x00,
  /* ABS: 004 Pixels @ 019,022 */ 0, 4, 0x67, 0xE7, 0x9B, 0x08,
  /* RLE: 007 Pixels @ 023,022 */ 7, 0x00,
  /* ABS: 005 Pixels @ 030,022 */ 0, 5, 0x64, 0xFF, 0xFF, 0xD2, 0x09,
  /* RLE: 017 Pixels @ 003,023 */ 17, 0x00,
  /* RLE: 001 Pixels @ 020,023 */ 1, 0x0E,
  /* RLE: 008 Pixels @ 021,023 */ 8, 0x00,
  /* ABS: 006 Pixels @ 029,023 */ 0, 6, 0x0A, 0xD3, 0xFF, 0xFF, 0xFF, 0x76,
  /* RLE: 026 Pixels @ 003,024 */ 26, 0x00,
  /* RLE: 001 Pixels @ 029,024 */ 1, 0x79,
  /* RLE: 004 Pixels @ 030,024 */ 4, 0xFF,
  /* ABS: 002 Pixels @ 002,025 */ 0, 2, 0xF2, 0x34,
  /* RLE: 024 Pixels @ 004,025 */ 24, 0x00,
  /* ABS: 002 Pixels @ 028,025 */ 0, 2, 0x34, 0xF2,
  /* RLE: 005 Pixels @ 030,025 */ 5, 0xFF,
  /* ABS: 002 Pixels @ 003,026 */ 0, 2, 0xD6, 0x1E,
  /* RLE: 022 Pixels @ 005,026 */ 22, 0x00,
  /* ABS: 002 Pixels @ 027,026 */ 0, 2, 0x1E, 0xD6,
  /* RLE: 007 Pixels @ 029,026 */ 7, 0xFF,
  /* ABS: 002 Pixels @ 004,027 */ 0, 2, 0xCA, 0x1F,
  /* RLE: 020 Pixels @ 006,027 */ 20, 0x00,
  /* ABS: 002 Pixels @ 026,027 */ 0, 2, 0x20, 0xCB,
  /* RLE: 009 Pixels @ 028,027 */ 9, 0xFF,
  /* ABS: 002 Pixels @ 005,028 */ 0, 2, 0xD6, 0x3B,
  /* RLE: 018 Pixels @ 007,028 */ 18, 0x00,
  /* ABS: 002 Pixels @ 025,028 */ 0, 2, 0x3B, 0xD6,
  /* RLE: 011 Pixels @ 027,028 */ 11, 0xFF,
  /* ABS: 003 Pixels @ 006,029 */ 0, 3, 0xF4, 0x7A, 0x0D,
  /* RLE: 014 Pixels @ 009,029 */ 14, 0x00,
  /* ABS: 003 Pixels @ 023,029 */ 0, 3, 0x0D, 0x7B, 0xF4,
  /* RLE: 014 Pixels @ 026,029 */ 14, 0xFF,
  /* ABS: 003 Pixels @ 008,030 */ 0, 3, 0xD0, 0x68, 0x16,
  /* RLE: 010 Pixels @ 011,030 */ 10, 0x00,
  /* ABS: 003 Pixels @ 021,030 */ 0, 3, 0x17, 0x66, 0xCE,
  /* RLE: 018 Pixels @ 024,030 */ 18, 0xFF,
  /* ABS: 012 Pixels @ 010,031 */ 0, 12, 0xE5, 0xA1, 0x67, 0x3D, 0x21, 0x11, 0x11, 0x21, 0x3D, 0x67, 0xA1, 0xE5,
  /* RLE: 010 Pixels @ 022,031 */ 10, 0xFF,
  0
};  // 485 bytes for 1024 pixels

GUI_CONST_STORAGE GUI_BITMAP bm7 = {
  32, // xSize
  32, // ySize
  32, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_ac7,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*********************************************************************
*
*       bm8
*/
static GUI_CONST_STORAGE unsigned char _ac8[] = {
  /* RLE: 128 Pixels @ 000,000 */ 128, 0xFF,
  /* ABS: 003 Pixels @ 000,004 */ 0, 3, 0xF5, 0x7C, 0x1D,
  /* RLE: 017 Pixels @ 003,004 */ 17, 0x0F,
  /* ABS: 002 Pixels @ 020,004 */ 0, 2, 0x34, 0xAD,
  /* RLE: 007 Pixels @ 022,004 */ 7, 0xFF,
  /* ABS: 004 Pixels @ 029,004 */ 0, 4, 0xA2, 0x40, 0xCD, 0x73,
  /* RLE: 020 Pixels @ 001,005 */ 20, 0x00,
  /* ABS: 002 Pixels @ 021,005 */ 0, 2, 0x02, 0xBC,
  /* RLE: 005 Pixels @ 023,005 */ 5, 0xFF,
  /* ABS: 005 Pixels @ 028,005 */ 0, 5, 0xB3, 0x0D, 0x00, 0x34, 0x17,
  /* RLE: 021 Pixels @ 001,006 */ 21, 0x00,
  /* RLE: 001 Pixels @ 022,006 */ 1, 0x63,
  /* RLE: 004 Pixels @ 023,006 */ 4, 0xFF,
  /* ABS: 006 Pixels @ 027,006 */ 0, 6, 0xB2, 0x0C, 0x00, 0x00, 0x0C, 0x0D,
  /* RLE: 021 Pixels @ 001,007 */ 21, 0x00,
  /* ABS: 011 Pixels @ 022,007 */ 0, 11, 0x58, 0xFF, 0xFF, 0xFF, 0xB1, 0x0C, 0x00, 0x00, 0x00, 0x0F, 0x0F,
  /* RLE: 021 Pixels @ 001,008 */ 21, 0x00,
  /* ABS: 005 Pixels @ 022,008 */ 0, 5, 0x5A, 0xFF, 0xFF, 0xB1, 0x0C,
  /* RLE: 004 Pixels @ 027,008 */ 4, 0x00,
  /* ABS: 002 Pixels @ 031,008 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 021 Pixels @ 001,009 */ 21, 0x00,
  /* ABS: 004 Pixels @ 022,009 */ 0, 4, 0x5A, 0xFF, 0xB3, 0x0C,
  /* RLE: 005 Pixels @ 026,009 */ 5, 0x00,
  /* ABS: 002 Pixels @ 031,009 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 021 Pixels @ 001,010 */ 21, 0x00,
  /* ABS: 003 Pixels @ 022,010 */ 0, 3, 0x65, 0xB8, 0x0D,
  /* RLE: 006 Pixels @ 025,010 */ 6, 0x00,
  /* ABS: 002 Pixels @ 031,010 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 021 Pixels @ 001,011 */ 21, 0x00,
  /* ABS: 002 Pixels @ 022,011 */ 0, 2, 0x25, 0x10,
  /* RLE: 007 Pixels @ 024,011 */ 7, 0x00,
  /* ABS: 002 Pixels @ 031,011 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 030 Pixels @ 001,012 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,012 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 030 Pixels @ 001,013 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,013 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 030 Pixels @ 001,014 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,014 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 030 Pixels @ 001,015 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,015 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 030 Pixels @ 001,016 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,016 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 030 Pixels @ 001,017 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,017 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 030 Pixels @ 001,018 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,018 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 030 Pixels @ 001,019 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,019 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 030 Pixels @ 001,020 */ 30, 0x00,
  /* ABS: 002 Pixels @ 031,020 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 021 Pixels @ 001,021 */ 21, 0x00,
  /* ABS: 002 Pixels @ 022,021 */ 0, 2, 0x24, 0x0F,
  /* RLE: 007 Pixels @ 024,021 */ 7, 0x00,
  /* ABS: 002 Pixels @ 031,021 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 021 Pixels @ 001,022 */ 21, 0x00,
  /* ABS: 003 Pixels @ 022,022 */ 0, 3, 0x65, 0xB6, 0x0C,
  /* RLE: 006 Pixels @ 025,022 */ 6, 0x00,
  /* ABS: 002 Pixels @ 031,022 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 021 Pixels @ 001,023 */ 21, 0x00,
  /* ABS: 004 Pixels @ 022,023 */ 0, 4, 0x5B, 0xFF, 0xB0, 0x0B,
  /* RLE: 005 Pixels @ 026,023 */ 5, 0x00,
  /* ABS: 002 Pixels @ 031,023 */ 0, 2, 0x0F, 0x0F,
  /* RLE: 021 Pixels @ 001,024 */ 21, 0x00,
  /* ABS: 005 Pixels @ 022,024 */ 0, 5, 0x5A, 0xFF, 0xFF, 0xAF, 0x0B,
  /* RLE: 004 Pixels @ 027,024 */ 4, 0x00,
  /* ABS: 002 Pixels @ 031,024 */ 0, 2, 0x0F, 0x0D,
  /* RLE: 021 Pixels @ 001,025 */ 21, 0x00,
  /* ABS: 011 Pixels @ 022,025 */ 0, 11, 0x58, 0xFF, 0xFF, 0xFF, 0xAF, 0x0A, 0x00, 0x00, 0x00, 0x0F, 0x18,
  /* RLE: 021 Pixels @ 001,026 */ 21, 0x00,
  /* RLE: 001 Pixels @ 022,026 */ 1, 0x64,
  /* RLE: 004 Pixels @ 023,026 */ 4, 0xFF,
  /* ABS: 006 Pixels @ 027,026 */ 0, 6, 0xAD, 0x0A, 0x00, 0x00, 0x0C, 0x75,
  /* RLE: 020 Pixels @ 001,027 */ 20, 0x00,
  /* ABS: 002 Pixels @ 021,027 */ 0, 2, 0x05, 0xBC,
  /* RLE: 005 Pixels @ 023,027 */ 5, 0xFF,
  /* ABS: 007 Pixels @ 028,027 */ 0, 7, 0xAD, 0x09, 0x00, 0x37, 0xF8, 0x8D, 0x4D,
  /* RLE: 017 Pixels @ 003,028 */ 17, 0x47,
  /* ABS: 002 Pixels @ 020,028 */ 0, 2, 0x59, 0xB7,
  /* RLE: 007 Pixels @ 022,028 */ 7, 0xFF,
  /* ABS: 003 Pixels @ 029,028 */ 0, 3, 0xAB, 0x5E, 0xD4,
  /* RLE: 096 Pixels @ 000,029 */ 96, 0xFF,
  0
};  // 288 bytes for 1024 pixels

GUI_CONST_STORAGE GUI_BITMAP bm8 = {
  32, // xSize
  32, // ySize
  32, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_ac8,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*********************************************************************
*
*       bm9
*/
static GUI_CONST_STORAGE unsigned char _ac9[] = {
  /* RLE: 046 Pixels @ 000,000 */ 46, 0xFF,
  /* ABS: 004 Pixels @ 014,001 */ 0, 4, 0xAF, 0x4B, 0x53, 0xBB,
  /* RLE: 027 Pixels @ 018,001 */ 27, 0xFF,
  /* ABS: 006 Pixels @ 013,002 */ 0, 6, 0xA9, 0x02, 0x00, 0x00, 0x09, 0xC5,
  /* RLE: 025 Pixels @ 019,002 */ 25, 0xFF,
  /* ABS: 002 Pixels @ 012,003 */ 0, 2, 0xB1, 0x0B,
  /* RLE: 004 Pixels @ 014,003 */ 4, 0x00,
  /* RLE: 001 Pixels @ 018,003 */ 1, 0x67,
  /* RLE: 024 Pixels @ 019,003 */ 24, 0xFF,
  /* ABS: 002 Pixels @ 011,004 */ 0, 2, 0xB8, 0x0D,
  /* RLE: 005 Pixels @ 013,004 */ 5, 0x00,
  /* RLE: 001 Pixels @ 018,004 */ 1, 0x58,
  /* RLE: 006 Pixels @ 019,004 */ 6, 0xFF,
  /* ABS: 002 Pixels @ 025,004 */ 0, 2, 0xFB, 0xBB,
  /* RLE: 015 Pixels @ 027,004 */ 15, 0xFF,
  /* ABS: 002 Pixels @ 010,005 */ 0, 2, 0xB2, 0x0E,
  /* RLE: 006 Pixels @ 012,005 */ 6, 0x00,
  /* RLE: 001 Pixels @ 018,005 */ 1, 0x5A,
  /* RLE: 006 Pixels @ 019,005 */ 6, 0xFF,
  /* ABS: 003 Pixels @ 025,005 */ 0, 3, 0xA3, 0x03, 0x74,
  /* RLE: 013 Pixels @ 028,005 */ 13, 0xFF,
  /* ABS: 002 Pixels @ 009,006 */ 0, 2, 0xA5, 0x0B,
  /* RLE: 007 Pixels @ 011,006 */ 7, 0x00,
  /* RLE: 001 Pixels @ 018,006 */ 1, 0x5A,
  /* RLE: 005 Pixels @ 019,006 */ 5, 0xFF,
  /* ABS: 005 Pixels @ 024,006 */ 0, 5, 0xF4, 0x26, 0x00, 0x00, 0x83,
  /* RLE: 011 Pixels @ 029,006 */ 11, 0xFF,
  /* ABS: 002 Pixels @ 008,007 */ 0, 2, 0x8D, 0x03,
  /* RLE: 008 Pixels @ 010,007 */ 8, 0x00,
  /* RLE: 001 Pixels @ 018,007 */ 1, 0x5A,
  /* RLE: 005 Pixels @ 019,007 */ 5, 0xFF,
  /* ABS: 006 Pixels @ 024,007 */ 0, 6, 0xFD, 0xAE, 0x0D, 0x00, 0x02, 0xB5,
  /* RLE: 008 Pixels @ 030,007 */ 8, 0xFF,
  /* ABS: 002 Pixels @ 006,008 */ 0, 2, 0xE4, 0x61,
  /* RLE: 010 Pixels @ 008,008 */ 10, 0x00,
  /* RLE: 001 Pixels @ 018,008 */ 1, 0x5A,
  /* RLE: 004 Pixels @ 019,008 */ 4, 0xFF,
  /* ABS: 016 Pixels @ 023,008 */ 0, 16, 0xFE, 0xFF, 0xFF, 0xA4, 0x01, 0x00, 0x25, 0xEE, 0xFF, 0xFD, 0x9F, 0x5F, 0x55, 0x57, 0x4D, 0x19,
  /* RLE: 011 Pixels @ 007,009 */ 11, 0x00,
  /* ABS: 015 Pixels @ 018,009 */ 0, 15, 0x5A, 0xFF, 0xFF, 0xFF, 0xDF, 0x49, 0xD6, 0xFF, 0xFF, 0x66, 0x00, 0x00, 0x87, 0xFF, 0x81,
  /* RLE: 017 Pixels @ 001,010 */ 17, 0x00,
  /* ABS: 015 Pixels @ 018,010 */ 0, 15, 0x5A, 0xFF, 0xFF, 0xFB, 0x53, 0x00, 0x35, 0xF2, 0xFF, 0xE4, 0x16, 0x00, 0x21, 0xF3, 0x1B,
  /* RLE: 017 Pixels @ 001,011 */ 17, 0x00,
  /* ABS: 015 Pixels @ 018,011 */ 0, 15, 0x5A, 0xFF, 0xFF, 0xF7, 0x43, 0x00, 0x00, 0x83, 0xFF, 0xFF, 0x71, 0x00, 0x00, 0xB1, 0x0D,
  /* RLE: 017 Pixels @ 001,012 */ 17, 0x00,
  /* ABS: 015 Pixels @ 018,012 */ 0, 15, 0x5A, 0xFF, 0xFF, 0xFF, 0xE2, 0x19, 0x00, 0x19, 0xEC, 0xFF, 0xC7, 0x02, 0x00, 0x6F, 0x0F,
  /* RLE: 017 Pixels @ 001,013 */ 17, 0x00,
  /* RLE: 001 Pixels @ 018,013 */ 1, 0x5A,
  /* RLE: 004 Pixels @ 019,013 */ 4, 0xFF,
  /* ABS: 010 Pixels @ 023,013 */ 0, 10, 0x80, 0x00, 0x00, 0xAA, 0xFF, 0xF5, 0x1F, 0x00, 0x40, 0x0F,
  /* RLE: 017 Pixels @ 001,014 */ 17, 0x00,
  /* RLE: 001 Pixels @ 018,014 */ 1, 0x5A,
  /* RLE: 004 Pixels @ 019,014 */ 4, 0xFF,
  /* ABS: 010 Pixels @ 023,014 */ 0, 10, 0xCB, 0x04, 0x00, 0x77, 0xFF, 0xFF, 0x44, 0x00, 0x21, 0x0F,
  /* RLE: 017 Pixels @ 001,015 */ 17, 0x00,
  /* RLE: 001 Pixels @ 018,015 */ 1, 0x5A,
  /* RLE: 004 Pixels @ 019,015 */ 4, 0xFF,
  /* ABS: 010 Pixels @ 023,015 */ 0, 10, 0xEC, 0x0E, 0x00, 0x5E, 0xFF, 0xFF, 0x57, 0x00, 0x12, 0x0F,
  /* RLE: 017 Pixels @ 001,016 */ 17, 0x00,
  /* RLE: 001 Pixels @ 018,016 */ 1, 0x5A,
  /* RLE: 004 Pixels @ 019,016 */ 4, 0xFF,
  /* ABS: 010 Pixels @ 023,016 */ 0, 10, 0xEF, 0x0F, 0x00, 0x5C, 0xFF, 0xFF, 0x59, 0x00, 0x11, 0x0F,
  /* RLE: 017 Pixels @ 001,017 */ 17, 0x00,
  /* RLE: 001 Pixels @ 018,017 */ 1, 0x5A,
  /* RLE: 004 Pixels @ 019,017 */ 4, 0xFF,
  /* ABS: 010 Pixels @ 023,017 */ 0, 10, 0xD6, 0x07, 0x00, 0x6E, 0xFF, 0xFF, 0x4B, 0x00, 0x1C, 0x0F,
  /* RLE: 017 Pixels @ 001,018 */ 17, 0x00,
  /* RLE: 001 Pixels @ 018,018 */ 1, 0x5A,
  /* RLE: 004 Pixels @ 019,018 */ 4, 0xFF,
  /* ABS: 010 Pixels @ 023,018 */ 0, 10, 0x97, 0x00, 0x00, 0x9A, 0xFF, 0xF9, 0x28, 0x00, 0x37, 0x0E,
  /* RLE: 017 Pixels @ 001,019 */ 17, 0x00,
  /* ABS: 015 Pixels @ 018,019 */ 0, 15, 0x5A, 0xFF, 0xFF, 0xFF, 0xF6, 0x2F, 0x00, 0x0B, 0xDE, 0xFF, 0xD6, 0x06, 0x00, 0x61, 0x12,
  /* RLE: 017 Pixels @ 001,020 */ 17, 0x00,
  /* ABS: 015 Pixels @ 018,020 */ 0, 15, 0x5A, 0xFF, 0xFF, 0xFB, 0x6C, 0x00, 0x00, 0x62, 0xFF, 0xFF, 0x89, 0x00, 0x00, 0x9E, 0x5D,
  /* RLE: 017 Pixels @ 001,021 */ 17, 0x00,
  /* ABS: 020 Pixels @ 018,021 */ 0, 20, 0x5A, 0xFF, 0xFF, 0xF7, 0x34, 0x00, 0x18, 0xDE, 0xFF, 0xF5, 0x28, 0x00, 0x12, 0xE6, 0xEA, 0x67, 0x28, 0x21, 0x22, 0x16,
  /* RLE: 012 Pixels @ 006,022 */ 12, 0x00,
  /* ABS: 022 Pixels @ 018,022 */ 0, 22, 0x5A, 0xFF, 0xFF, 0xFF, 0xC5, 0x1E, 0xB2, 0xFF, 0xFF, 0x8B, 0x00, 0x00, 0x69, 0xFF, 0xFF, 0xFF, 0xF7, 0xF5, 0xF5, 0xF0, 0xB6, 0x33,
  /* RLE: 010 Pixels @ 008,023 */ 10, 0x00,
  /* RLE: 001 Pixels @ 018,023 */ 1, 0x5A,
  /* RLE: 004 Pixels @ 019,023 */ 4, 0xFF,
  /* ABS: 008 Pixels @ 023,023 */ 0, 8, 0xE2, 0xFF, 0xFF, 0xCB, 0x0E, 0x00, 0x10, 0xDB,
  /* RLE: 008 Pixels @ 031,023 */ 8, 0xFF,
  /* ABS: 002 Pixels @ 007,024 */ 0, 2, 0xED, 0x59,
  /* RLE: 009 Pixels @ 009,024 */ 9, 0x00,
  /* RLE: 001 Pixels @ 018,024 */ 1, 0x5A,
  /* RLE: 006 Pixels @ 019,024 */ 6, 0xFF,
  /* ABS: 005 Pixels @ 025,024 */ 0, 5, 0xD8, 0x26, 0x00, 0x00, 0x8F,
  /* RLE: 010 Pixels @ 030,024 */ 10, 0xFF,
  /* ABS: 002 Pixels @ 008,025 */ 0, 2, 0xFD, 0x74,
  /* RLE: 008 Pixels @ 010,025 */ 8, 0x00,
  /* RLE: 001 Pixels @ 018,025 */ 1, 0x5A,
  /* RLE: 005 Pixels @ 019,025 */ 5, 0xFF,
  /* ABS: 006 Pixels @ 024,025 */ 0, 6, 0xF4, 0x2F, 0x00, 0x00, 0x58, 0xFD,
  /* RLE: 012 Pixels @ 030,025 */ 12, 0xFF,
  /* RLE: 001 Pixels @ 010,026 */ 1, 0x82,
  /* RLE: 007 Pixels @ 011,026 */ 7, 0x00,
  /* RLE: 001 Pixels @ 018,026 */ 1, 0x5A,
  /* RLE: 005 Pixels @ 019,026 */ 5, 0xFF,
  /* ABS: 005 Pixels @ 024,026 */ 0, 5, 0xFD, 0x7A, 0x00, 0x47, 0xF1,
  /* RLE: 014 Pixels @ 029,026 */ 14, 0xFF,
  /* RLE: 001 Pixels @ 011,027 */ 1, 0x89,
  /* RLE: 006 Pixels @ 012,027 */ 6, 0x00,
  /* RLE: 001 Pixels @ 018,027 */ 1, 0x59,
  /* RLE: 006 Pixels @ 019,027 */ 6, 0xFF,
  /* ABS: 003 Pixels @ 025,027 */ 0, 3, 0xF2, 0x86, 0xF0,
  /* RLE: 016 Pixels @ 028,027 */ 16, 0xFF,
  /* RLE: 001 Pixels @ 012,028 */ 1, 0x85,
  /* RLE: 005 Pixels @ 013,028 */ 5, 0x00,
  /* RLE: 001 Pixels @ 018,028 */ 1, 0x5E,
  /* RLE: 026 Pixels @ 019,028 */ 26, 0xFF,
  /* RLE: 001 Pixels @ 013,029 */ 1, 0x7C,
  /* RLE: 004 Pixels @ 014,029 */ 4, 0x00,
  /* RLE: 001 Pixels @ 018,029 */ 1, 0xA9,
  /* RLE: 027 Pixels @ 019,029 */ 27, 0xFF,
  /* ABS: 004 Pixels @ 014,030 */ 0, 4, 0x85, 0x2D, 0x32, 0x93,
  /* RLE: 046 Pixels @ 018,030 */ 46, 0xFF,
  0
};  // 515 bytes for 1024 pixels

GUI_CONST_STORAGE GUI_BITMAP bm9 = {
  32, // xSize
  32, // ySize
  32, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (unsigned char *)_ac9,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_RLEALPHA
};

/*********************************************************************
*
*       bmArrowRigth_15x24
*/
static GUI_CONST_STORAGE GUI_COLOR _ColorsArrowRigth_15x24[] = {
  0xFFFFFF, 0xccc7c7
};

static GUI_CONST_STORAGE GUI_LOGPALETTE _PalArrowRigth_15x24 = {
  2,  // Number of entries
  1,  // Has transparency
  &_ColorsArrowRigth_15x24[0]
};

static GUI_CONST_STORAGE unsigned char _acArrowRigth_15x24[] = {
  ___X____, ________,
  __XXX___, ________,
  _XXXXX__, ________,
  XXXXXXX_, ________,
  _XXXXXXX, ________,
  __XXXXXX, X_______,
  ___XXXXX, XX______,
  ____XXXX, XXX_____,
  _____XXX, XXXX____,
  ______XX, XXXXX___,
  _______X, XXXXXX__,
  ________, XXXXXXX_,
  ________, XXXXXXX_,
  _______X, XXXXXX__,
  ______XX, XXXXX___,
  _____XXX, XXXX____,
  ____XXXX, XXX_____,
  ___XXXXX, XX______,
  __XXXXXX, X_______,
  _XXXXXXX, ________,
  XXXXXXX_, ________,
  _XXXXX__, ________,
  __XXX___, ________,
  ___X____, ________
};

GUI_CONST_STORAGE GUI_BITMAP bmArrowRigth_15x24 = {
  15, // xSize
  24, // ySize
  2, // BytesPerLine
  1, // BitsPerPixel
  _acArrowRigth_15x24,  // Pointer to picture data (indices)
  &_PalArrowRigth_15x24   // Pointer to palette
};


const struct MENU_ITEM {
  GUI_COLOR Color;
  const GUI_BITMAP * pBm;
  const char * pText;
} _aMenu[] = {
  { 0x552dff, &bm0, "Passcode" },
  { 0x552dff, &bm1, "Mail" },
  { 0x552dff, &bm2, "Contacts" },
  { 0x938e8e, &bm3, "Menu" },
  { 0x938e8e, &bm4, "Notepad" },
  { 0x938e8e, &bm5, "Photos" },
  { 0xf0661d, &bm6, "Setup" },
  { 0xf0661d, &bm7, "Time" },
  { 0xf0661d, &bm8, "Videos" },
  { 0x30d5ff, &bm9, "Music" },
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _ButtonSkin
*
* Purpose:
*   Skinning function of a single button. Responsible for custom drawing only.
*/
static int _ButtonSkin(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
  int Index, xSize, ySize, IsPressed;
  WM_HWIN hWin;
  const GUI_BITMAP * pBm;
  GUI_COLOR Color;

  hWin = pDrawItemInfo->hWin;
  switch (pDrawItemInfo->Cmd) {
  case WIDGET_ITEM_DRAW_BACKGROUND:
    //
    // Get state of button
    //
    IsPressed = BUTTON_IsPressed(pDrawItemInfo->hWin);
    //
    // Get window dimension
    //
    xSize = WM_GetWindowSizeX(hWin);
    ySize = WM_GetWindowSizeY(hWin);
    //
    // Get button index
    //
    BUTTON_GetUserData(hWin, &Index, sizeof(Index));
    //
    // Draw background color depending on state
    //
    if (IsPressed) {
      Color = 0xCCCCCC;
    } else {
      Color = GUI_WHITE;
    }
    GUI_SetBkColor(Color);
    GUI_Clear();
    //
    // Draw antialiased rounded rectangle as background for image
    //
    GUI_SetColor(_aMenu[Index].Color);
    GUI_AA_FillRoundedRect(10, 6, 47 + 10, 47 + 6, 12);
    //
    // Draw compressed alpha bitmap
    //
    GUI_SetColor(GUI_WHITE);
    GUI_DrawBitmap(_aMenu[Index].pBm, 18, 14);
    //
    // Draw button text
    //
    GUI_SetColor(GUI_BLACK);
    GUI_SetFont(&GUI_FontComic24B_ASCII);
    GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringAt(_aMenu[Index].pText, 80, 30);
    //
    // Draw small separator line
    //
    GUI_SetColor(0xccc7c8);
    GUI_DrawHLine(ySize - 1, 80, xSize - 1);
    //
    // Draw arrow at the right
    //
    pBm = &bmArrowRigth_15x24;
    GUI_DrawBitmap(&bmArrowRigth_15x24, xSize - 40, (ySize - pBm->YSize) / 2);
    break;
  }
  return 0;
}

/*********************************************************************
*
*       _cbHeading
*
* Purpose:
*   Callback function of heading window containing logo and battery symbol.
*/
//static void _cbHeading(WM_MESSAGE * pMsg) {
//  int xSize, xPos;
//  const GUI_BITMAP * pBm;
//  WM_HWIN hWin;
//  static int Index;
//
//  hWin = pMsg->hWin;
//  switch (pMsg->MsgId) {
//  case WM_CREATE:
//    //
//    // Create timer to be used to modify the battery symbol
//    //
//    WM_CreateTimer(hWin, 0, 1000, 0);
//    break;
//  case WM_TIMER:
//    //
//    // Modify battery symbol on timer message
//    //
//    Index++;
//    if (Index == GUI_COUNTOF(_apbmCharge)) {
//      Index = 0;
//    }
//    WM_InvalidateWindow(hWin);
//    WM_RestartTimer(pMsg->Data.v, 0);
//    break;
//  case WM_PAINT:
//    //
//    // Get window dimension
//    //
//    xSize = WM_GetWindowSizeX(hWin);
//    //
//    // Draw logo, battery and clock
//    //
//    xPos = xSize;
//    pBm = &_bmLogoSegger_40x20;
//    GUI_SetColor(0xf8f8f8);
//    GUI_FillRect(0, 0, xSize - 1, 57);
//    GUI_SetColor(0xb2b2b2);
//    GUI_FillRect(0, 58, xSize - 1, 59);
//    GUI_DrawBitmap(pBm, 0, 0);
//    pBm = &_bmBatteryEmpty_27x14;
//    xPos -= pBm->XSize + 3;
//    GUI_DrawBitmap(pBm, xPos, 3);
//    pBm = _apbmCharge[Index];
//    GUI_DrawBitmap(pBm, xPos, 3);
//    pBm = &bmClock_16x16_black;
//    xPos -= pBm->XSize + 10;
//    GUI_DrawBitmap(pBm, xPos, 2);
//    //
//    // Draw 'Settings'
//    //
//    GUI_SetFont(&GUI_FontComic24B_ASCII);
//    GUI_SetColor(GUI_BLACK);
//    GUI_SetTextMode(GUI_TM_TRANS);
//    GUI_DispStringHCenterAt("Settings", xSize / 2, 20);
//    break;
//  default:
//    WM_DefaultProc(pMsg);
//  }
//}

/*********************************************************************
*
*       _cbSelected
*
* Purpose:
*   Callback function of window created in reaction of pressing a button.
*   Responsible for drawing only.
*/
static void _cbSelected(WM_MESSAGE * pMsg)
{
  WM_HWIN hWin;
  int Index, xSize, yPos, ySizeFont;
  const GUI_BITMAP * pBm;

  hWin = pMsg->hWin;
  switch (pMsg->MsgId)
  {
	  case WM_PAINT:
	  {
		WM_GetUserData(hWin, &Index, sizeof(Index));
//		xSize = WM_GetXSize(hWin);
//		GUI_SetBkColor(_aMenu[Index].Color);
//		GUI_Clear();

		if(Index==2)
		{
			 xTaskCreate(MP3_player,(char const*)"MP3_player",2048,NULL,tskIDLE_PRIORITY + 2,&MP3_Handle);

		}

		break;
	  }

  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _CreateSelected
*
* Purpose:
*   This function is called when a button is pressed. It creates a new
*   window and shows a small screen animation.
*/
static void _CreateSelected(int Index, WM_HWIN hWin)
{
  WM_HWIN hWinBase;     // Window to be shifted out
  WM_HWIN hWinSelected; // New window to become visible behind hWinBase
  int xSize, ySize;

  hWinBase = WM_GetParent(WM_GetParent(hWin));
  xSize = WM_GetXSize(hWinBase);
  ySize = WM_GetYSize(hWinBase);
  hWinSelected = WM_CreateWindow(0, 0, xSize, ySize, WM_CF_SHOW, _cbSelected, sizeof(Index));
  //
  // Pass menu index to window
  //
  WM_SetUserData(hWinSelected, &Index, sizeof(Index));
  //
  // Window animation
//
//  GUI_MEMDEV_ShiftOutWindow(hWinBase, 500, GUI_MEMDEV_EDGE_LEFT);
//  GUI_Delay(500);
//  GUI_MEMDEV_ShiftInWindow(hWinBase, 500, GUI_MEMDEV_EDGE_LEFT);
  //
  // Remove the new window
  //
//  WM_DeleteWindow(hWinSelected);
}

/*********************************************************************
*
*       _cbMenu
*
* Purpose:
*   Callback function of menu window.
*/
static void _cbMenu(WM_MESSAGE * pMsg) {
  int xSize, i, Id, NCode;
  WM_MOTION_INFO * pInfo;
  WM_HWIN hWin, hButton;
  static int IsPressed;

  hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);      // Id of widget
    NCode = pMsg->Data.v;                 // Notification code
    switch (NCode) {
    case WM_NOTIFICATION_CLICKED:
      IsPressed = 1;
      break;
    case WM_NOTIFICATION_RELEASED:
      if (IsPressed)
      {
        //
        // React only if window is not moving
        //
        _CreateSelected(Id - GUI_ID_BUTTON0, pMsg->hWin);
        IsPressed = 0;
      }
      break;
    case WM_NOTIFICATION_MOVED_OUT:
        IsPressed = 0;
      break;
    }
    break;
  case WM_MOTION:
    pInfo = (WM_MOTION_INFO *)pMsg->Data.p;
    switch (pInfo->Cmd) {
    case WM_MOTION_MOVE:
      //
      // Button should not react if window is moving
      //
      IsPressed = 0;
      break;
    case WM_MOTION_INIT:
      //
      // Activate snapping
      //
      pInfo->SnapY = 60;
      break;
    }
    break;
  case WM_CREATE:
    //
    // Create all buttons
    //
    xSize = WM_GetWindowSizeX(hWin);
    for (i = 0; (unsigned)i < GUI_COUNTOF(_aMenu); i++) {
      hButton = BUTTON_CreateUser(0, i * 60, xSize, 60, hWin, WM_CF_SHOW, 0, GUI_ID_BUTTON0 + i, sizeof(i));
      BUTTON_SetSkin(hButton, _ButtonSkin);
      BUTTON_SetUserData(hButton, &i, sizeof(i));
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _cbDummy
*
* Purpose:
*   Required for base window and viewport window to make sure WM_DefaultProc() is called.
*/
static void _cbDummy(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  default:
    WM_DefaultProc(pMsg);
  }
}


















uint8_t buffer[512]__attribute((section(".ExRam")));
uint8_t pagebuff[2340]__attribute((section(".ExRam")));
char name[107]__attribute((section(".ExRam")));
u8 rand[250]__attribute((section(".ExRam")));
uint8_t _aucLine[8192]__attribute((section(".ExRam")));
FRESULT f=0;
FILINFO fno;
char t[10];

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

//
	  WM_SetCreateFlags(WM_CF_MEMDEV);
	  GUI_Init();
	  WM_MOTION_Enable(1);
	  BUTTON_SetReactOnLevel();

//
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


//	  xTaskCreate(MP3_player,(char const*)"MP3_player",1024,NULL,tskIDLE_PRIORITY + 2,&MP3_Handle);
	  xTaskCreate(Demo_Task,(char const*)"GUI_DEMO",2048,NULL,tskIDLE_PRIORITY  + 3,&Demo_Handle);
	  xTaskCreate(Menu,(char const*)"Menu",2048,NULL,tskIDLE_PRIORITY + 2,&Menu_Handle);
//	  xTaskCreate(Background_Task,(char const*)"BK_GND",1024,NULL,tskIDLE_PRIORITY + 2,&Task_Handle);
	  TouchScreenTimer = xTimerCreate ("Timer",60, pdTRUE,( void * ) 1, vTimerCallback);

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
	WM_HWIN hItem,hText;
	WM_HWIN hButton3;

	while(1)
	{

		vTaskDelayUntil(&xLastFlashTime, 200/portTICK_RATE_MS);///////////////////////////////////
//		GUI_Delay(250);
		WM_Paint(hWin);
//		taskENTER_CRITICAL();
	    hButton3 = WM_GetDialogItem(hWin, ID_BACK);

		if(BUTTON_IsPressed(hButton3))
		{
			while(BUTTON_IsPressed(hButton3)){};
			xTaskCreate(Menu,(char const*)"Menu",2048,NULL,tskIDLE_PRIORITY + 2,&Menu_Handle);
			vTaskDelete(MP3_Handle);
		}
//
	    ADC_SoftwareStartConv(ADC1);
	    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	    int result= ADC_GetConversionValue(ADC1);
	    result = result * 8059/10000;
	    hItem = WM_GetDialogItem(hWin, ID_PROGBAR_2);
	    hText = WM_GetDialogItem(hWin, ID_TEXT_2);
	    PROGBAR_SetValue(hItem,result*100/3100);
	    TEXT_SetText(hText, itoa(uxTaskGetNumberOfTasks(),t,10));
	    GUI_Exec();

	}
}
static void Menu(void *pvParameters)
{
	  int xSize, ySize;
	  WM_HWIN hWinBase;     // Parent window for heading and viewport
	  WM_HWIN hWinHeading;  // Heading window
	  WM_HWIN hWinViewport; // Viewport window
	  WM_HWIN hWinMenu;     // Menu window moveable within viewport window

	  xSize = LCD_GetXSize();
	  ySize = LCD_GetYSize();
	  WM_SetSize(WM_HBKWIN, xSize, ySize);
	  WM_HWIN hButton;

	  hWinBase     = WM_CreateWindow       (0,  0, xSize, ySize ,                                  WM_CF_SHOW, _cbDummy, 0);
	  hWinViewport = WM_CreateWindowAsChild(0, 60, xSize, ySize - 60,               hWinBase,     WM_CF_SHOW, _cbDummy, 0);
	  hWinMenu     = WM_CreateWindowAsChild(0,  0, xSize, 60 * GUI_COUNTOF(_aMenu), hWinViewport, WM_CF_SHOW | WM_CF_MOTION_Y, _cbMenu, 0);
//	  CreateBar();

	while(1)
	{
		GUI_Exec();

//		hButton = WM_GetDialogItem(hWinMenu, GUI_ID_BUTTON0 + 9);
//
//		if(BUTTON_IsPressed(hButton))
//		{
//
//
//		}
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

	  f=f_mount(&fs,"",0);
	  f=f_opendir(&dir,"0:music");
	  f=f_open(&fsrc,"music/fink.mp3", FA_READ | FA_OPEN_EXISTING );

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

	vTaskDelete(Menu_Handle);

	while(1)
	{

		vTaskDelayUntil(&xLastFlashTime, 1000/portTICK_RATE_MS);

//		hText1 = WM_GetDialogItem(hWin, ID_TEXT_1);
		hText = WM_GetDialogItem(hWin, ID_TEXT_0);
		hSlider = WM_GetDialogItem(hWin, ID_SLIDER_0);
//		hSlider1 = WM_GetDialogItem(hWin, ID_SLIDER_1);
		hProgBar = WM_GetDialogItem(hWin, ID_PROGBAR_0);
		hButton = WM_GetDialogItem(hWin, ID_BUTTON_0);
		hButton1 = WM_GetDialogItem(hWin, ID_BUTTON_1);
		hButton2 = WM_GetDialogItem(hWin, ID_BUTTON_2);

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
//		if(BUTTON_IsPressed(hButton2))
//		{
//			while(BUTTON_IsPressed(hButton2)){};
//			xTaskCreate(Menu,(char const*)"Menu",2048,NULL,tskIDLE_PRIORITY + 2,&Menu_Handle);
//			vTaskDelete(MP3_Handle);
//		}
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
  {}
}
void vApplicationIdleHook(void)
{
	int i=0;

	while(1)
	{
		i++;

//		if(i>90000000)
//		{
			LCD_String_lc("in idle",5,10,RED,GREEN,2);
//			GUI_DispStringAt("IN IDLE",50,50);
			i=0;
//		}
//		LCD_box_mid_fill(0,0,5,5,RED);
//		GUI_DispStringAt("IN IDLE",50,50);
//		LCD_String_lc("in idle",5,10,RED,GREEN,2);
	}
}
//	  USBD_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
//	  while(1){};





