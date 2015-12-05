/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_E51_H
#define __LCD_E51_H

#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define SWRESET ((u8)0x01)
#define SLPOUT ((u8)0x11)
#define DISPON ((u8)0x29)
#define RAMWR ((u8)0x2C)
#define CASET ((u8)0x2A)
#define RASET ((u8)0x2B)
#define MADCTR ((u8)0x36)
#define IFMODE ((u8)0xB0)
#define NOP					0x00
//#define SWRESET			0x01
#define RDDID				0x04
#define RDDST				0x09
#define RDDPM				0x0A
#define RDDMADCTR			0x0B
#define RDDCOLMOD			0x0C
#define RDDIM				0x0D
#define RDDSM				0x0E/////
#define RDDSDR				0x0F
#define SLPIN				0x10
//#define SLPOUT			0x11
#define PTLON				0x12
#define NORON				0x13
#define INVOFF				0x20
#define INVON				0x21
#define GAMSET				0x26
#define DISPOFF				0x28
//#define DISPON			0x29
//#define CASET				0x2A
//#define RASET				0x2B
//#define RAMWR				0x2C
#define RAMRD				0x2E
#define PTLAR				0x30
#define TEOFF				0x34
#define TEON				0x35
//#define MADCTR				0x36
#define IDMOFF				0x38
#define IDMON				0x39
#define COLMOD				0x3A
#define WRDISBV				0x51
#define RDDISBV				0x52
#define WRCTRLD				0x53
#define RDCTRLD				0x54
#define WRCABC				0x55
#define RDCABC				0x56
#define RDID1				0xDA
#define RDID2				0xDB
#define RDID3				0xDC
//#define IFMODE				0xB0
#define DISCLK				0xB1
#define INVCTR				0xB2
#define REGCTR				0xC0
#define VCOMCTR				0xC1
#define GAMCTR1				0xC8
#define GAMCTR2				0xC9
#define GAMCTR3				0xCA
#define GAMCTR4				0xCB
#define EPPGMDB				0xD0
#define EPERASE				0xD1
#define EPPROG				0xD2
#define EPRDVRF				0xD3
#define RDVCOF				0xD9
#define LEDCTRL				0xEF


#define BLACK          0x000000
#define WHITE          0xFFFFFF
#define RED            0x0000FF
#define BLUE           0xFF0000
#define YELLOW         0x00FFFF
#define GREEN          0x00FF00

#define LCD_DC 11
#define LCD_WR 4
#define LCD_RD 5
#define LCD_CS 6
#define LCD_RST 7

#define RST_1 GPIOA->BSRRL|=GPIO_BSRR_BS_2
#define RST_0 GPIOA->BSRRH|=GPIO_BSRR_BS_2

#define FSMC_Bank_NAND     FSMC_Bank2_NAND
#define Bank_NAND_ADDR     Bank2_NAND_ADDR

//#define Bank2_NAND_ADDR    ((uint32_t)0x70000000)
#define Bank2_NAND_ADDR    ((uint32_t)0x80000000)
#define DATA_AREA          ((u32)0x00000000)
#define CMD_AREA            (u32)(1<<16)

#define LCD_WRITE_COMMAND 	*(vu8 *)(Bank_NAND_ADDR | DATA_AREA)
#define LCD_WRITE_DATA 		*(vu8 *)(Bank_NAND_ADDR | CMD_AREA)
/* Includes ------------------------------------------------------------------*/
/*

/* Exported functions ------------------------------------------------------- */
//void set_LCD(unsigned char pin);
void delay5(void);
void clear_LCD(unsigned char pin);
//void delay_ms(unsigned int ms);
void LCD_hard_reset();
void LCD_init();
void FSMC_NAND_Init(void);
void LCD_paint(unsigned int color);
void LCD_pixel(unsigned int x, unsigned int y, unsigned int color);
void LCD_String_nl(unsigned int x, unsigned int y, unsigned int color, unsigned char *napis);
void LCD_circle(int x0, int y0, unsigned int radius, unsigned int color,u8 fill);
void LCD_circle_but(int x0, int y0, unsigned int radius, unsigned int color, u8 type);
void LCD_part_paint(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
//void LCD_line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color);
void LCD_box(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color,u8 fill);
void LCD_char(char c, int x, int y, int size, int fColor, int bColor, int resize);
void LCD_String( char *pString, int x, int  y, int Size, int fColor, int bColor, int resize);
void LCD_box_mid(unsigned int x, unsigned int y, unsigned int lx, unsigned int ly, unsigned int color);
void LCD_box_mid_fill(unsigned int x, unsigned int y, unsigned int lx, unsigned int ly, unsigned int color);
//void putc(unsigned char x, unsigned char y, unsigned int color, unsigned char *napis);
void LCD_String_lc( char *pString, int col, int  line, int fColor, int bColor, int resize);
void LCD_line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color, int resize);
void LCD_char_lc(char c, int col, int line, int size, int fColor, int bColor, int resize);
//void LCD_box_mid_round(unsigned int x, unsigned int y, unsigned int lx, unsigned int ly, unsigned int color, u8 rad);
void LCD_box_mid_round(unsigned int x, unsigned int y, unsigned int lx, unsigned int ly, unsigned int color, u8 rad, u8 fill);
void LCD_goto(unsigned int x, unsigned int y);
int test_str_len(char* text, uint8_t size, u8 ret_typ);
u8 test_resize(u8 resize);

#endif /* __LCD_E51_H */



