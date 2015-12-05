/* Includes ------------------------------------------------------------------*/
//#include "lcd.h"
#include "stm32f4xx_fsmc.h"
#include "LCD_6300.h"
#include "FONT8x8.h"

#include <stm32f4xx.h>
#include <system_stm32f4xx.h>
#include <core_cm4.h>

#include <stm32f4xx_fsmc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

#define CMD_AREA            (u32)(1<<16)
#define LCD_WRITE_COMMAND 	*(vu8 *)(Bank_NAND_ADDR | DATA_AREA)
#define LCD_WRITE_DATA 		*(vu8 *)(Bank_NAND_ADDR | CMD_AREA)

#define RST_1 GPIOA->BSRRL|=GPIO_BSRR_BS_2
#define RST_0 GPIOA->BSRRH|=GPIO_BSRR_BS_2

u8 vertical=0;


void LCD_hard_reset()
{
	RST_0;
	for(int i=0;i<0xffff;i++){};
	RST_1;
	for(int i=0;i<0xffff;i++){};
}

/*************************************************/

/*************************************************/

void LCD_init()
{
//	delay(250);//wait for stabilization lcd power
	for(int i=0;i<0xffff;i++){};
	LCD_hard_reset();
	LCD_WRITE_COMMAND = SWRESET; //send 1 byte
	for(int i=0;i<0xffff;i++){};
	LCD_WRITE_COMMAND = SLPOUT;//send 1 byte
	for(int i=0;i<0xffff;i++){};//delay 5ms	(or 120ms)
	LCD_WRITE_COMMAND = DISPON;//send 1 byte
	LCD_WRITE_COMMAND=(MADCTR);
	LCD_WRITE_DATA = (0xff);
//	LCD_WRITE_COMMAND=(COLMOD);
//	LCD_WRITE_DATA=(0x3d);

}

/*************************************************/

/*************************************************/

void FSMC_NAND_Init(void)
{
//  FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
//  FSMC_NANDInitTypeDef FSMC_NANDInitStructure2;
//  FSMC_NAND_PCCARDTimingInitTypeDef  p;
//  p.FSMC_SetupTime = 0;//1
//  p.FSMC_WaitSetupTime = 1;//2
//  p.FSMC_HoldSetupTime = 1;//1
//  p.FSMC_HiZSetupTime = 0;

  FSMC_Bank2->PCR2|=FSMC_ECCPageSize_8192Bytes;  //page 8192 bytres
  FSMC_Bank2->PCR2&=~FSMC_PCR2_PWID;             //bus width 8b
  FSMC_Bank2->PCR2|=FSMC_PCR2_PWAITEN; 			//pwait enable
//  FSMC_Bank2->PCR2|=FSMC_PCR2_PBKEN;           //do not turn on this bank

  FSMC_Bank3->PCR3|=FSMC_PCR3_PBKEN;            // turn on bank 3 instead

//  FSMC_Bank2->PMEM2=FSMC_PMEM2_MEMHIZ2_0;
//  FSMC_Bank2->PMEM2=FSMC_PMEM2_MEMHOLD2_0;
//  FSMC_Bank2->PMEM2=FSMC_PMEM2_MEMSET2_0;
//  FSMC_Bank2->PMEM2=FSMC_PMEM2_MEMWAIT2_0;
//
//  FSMC_Bank2->PATT2=FSMC_PMEM2_MEMHIZ2_0;
//  FSMC_Bank2->PATT2=FSMC_PMEM2_MEMHOLD2_0;
//  FSMC_Bank2->PATT2=FSMC_PMEM2_MEMSET2_0;
//  FSMC_Bank2->PATT2=FSMC_PMEM2_MEMWAIT2_0;


    FSMC_Bank3->PMEM3=FSMC_PMEM3_MEMHIZ3_0;
    FSMC_Bank3->PMEM3=FSMC_PMEM3_MEMHOLD3_2;
    FSMC_Bank3->PMEM3=FSMC_PMEM3_MEMSET3_0;
    FSMC_Bank3->PMEM3=FSMC_PMEM3_MEMWAIT3_1;

    FSMC_Bank3->PATT3=FSMC_PMEM3_MEMHIZ3_0;
    FSMC_Bank3->PATT3=FSMC_PMEM3_MEMHOLD3_2;
    FSMC_Bank3->PATT3=FSMC_PMEM3_MEMSET3_0;
    FSMC_Bank3->PATT3=FSMC_PMEM3_MEMWAIT3_1;

}

/*************************************************/

/*************************************************/

void LCD_paint(unsigned int color)
{
  unsigned int i;

  LCD_WRITE_COMMAND = RASET;
  LCD_WRITE_DATA = ((unsigned char)((0>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(0 & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)((0>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(239 & 0x000000FF));	    //Each value represents one column line in the DDRAM

  LCD_WRITE_COMMAND = CASET;
  LCD_WRITE_DATA = ((unsigned char)((0>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(0 & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)((319>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(319 & 0x000000FF));	    //Each value represents one column line in the DDRAM

  LCD_WRITE_COMMAND = RAMWR;

  for(i=0; i<320*240; i++)
  {
    LCD_WRITE_DATA = ((unsigned char)color);
    LCD_WRITE_DATA = ((unsigned char)(color >> 8));
    LCD_WRITE_DATA = ((unsigned char)(color >> 16));
  }

}

/*************************************************/

/*************************************************/

void LCD_pixel(unsigned int x, unsigned int y, unsigned int color)
{


  LCD_WRITE_COMMAND = CASET;
  LCD_WRITE_DATA = ((unsigned char)((x>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(x & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)((x>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(x & 0x000000FF));	    //Each value represents one column line in the DDRAM


  LCD_WRITE_COMMAND = RASET;
  LCD_WRITE_DATA = ((unsigned char)((y>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(y & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)((y>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(y & 0x000000FF));	    //Each value represents one column line in the DDRAM

  LCD_WRITE_COMMAND = RAMWR;
  LCD_WRITE_DATA = ((unsigned char)(color >> 16));
  LCD_WRITE_DATA = ((unsigned char)(color >> 8));
  LCD_WRITE_DATA = ((unsigned char)(color));

}

/*************************************************/

/*********************************************************/
void LCD_goto(unsigned int x, unsigned int y)
{


  LCD_WRITE_COMMAND = CASET;
  LCD_WRITE_DATA = ((unsigned char)((x>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(x & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)((x>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(x & 0x000000FF));	    //Each value represents one column line in the DDRAM


  LCD_WRITE_COMMAND = RASET;
  LCD_WRITE_DATA = ((unsigned char)((y>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(y & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)((y>>8) & 0x000000FF));	    //Each value represents one column line in the DDRAM
  LCD_WRITE_DATA = ((unsigned char)(y & 0x000000FF));	    //Each value represents one column line in the DDRAM


}
/*********************************************************/


/*********************************************************/

void LCD_circle(unsigned char x, unsigned char y, unsigned int promien, unsigned int color){
	//np: 	kolo(60, 90, 100, niebieski);
	int i;
	int j;

	for(i=0;i<320;i++)
	{
		for(j=0;j<240;j++)
		{
			if(((i-x)*(i-x) + (j-y)*(j-y)) < promien*4)LCD_pixel(i, j, color);
		}
	}
}



//void LCD_line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color)
//{
//	  int dy = y1 - y0;
//	  int dx = x1 - x0;
//	  int stepx, stepy;
//		if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
//	  if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
//
//	  dy <<= 1;        // dy jest teraz 2*dy
//	  dx <<= 1;        // dx jest teraz 2*dx
//
//	  LCD_pixel(x0, y0, color);
//
//	  if (dx > dy)
//	  {
//	 		int fraction = dy - (dx >> 1);  // tak samo jak dla 2*dy - dx
//
//	 		while (x0 != x1)
//	 		{
//				if (fraction >= 0)
//				{
//						y0 += stepy;
//						fraction -= dx;    // tak samo jak frakcja -= 2*dx
//				}
//				   x0 += stepx;
//				  fraction += dy;    // tak samo jak frakcja -= 2*dy
//				  LCD_pixel(x0, y0, color);
//	    	}
//	 	}
//	 	else
//	 	{
//	 		int fraction = dx - (dy >> 1);
//			while (y0 != y1)
//			{
//					if (fraction >= 0)
//					{
//						x0 += stepx;
//						fraction -= dy;
//					}
//
//					y0 += stepy;
//					fraction += dx;
//					LCD_pixel(x0, y0, color);
//			}
//	 	}
//}

void LCD_line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color, int resize)
{
	  int dy = y1 - y0;
	  int dx = x1 - x0;
	  int stepx, stepy;
		if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
	  if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }

	  dy <<= 1;        // dy jest teraz 2*dy
	  dx <<= 1;        // dx jest teraz 2*dx

      if(resize!=1)
      {
      	LCD_box_mid_fill(x0,y0,resize+1,resize+1,color);
      }
      else LCD_box_mid_fill(x0,y0,resize,resize,color);

	  if (dx > dy)
	  {
	 		int fraction = dy - (dx >> 1);  // tak samo jak dla 2*dy - dx

	 		while (x0 != x1)
	 		{
				if (fraction >= 0)
				{
						y0 += stepy;
						fraction -= dx;    // tak samo jak frakcja -= 2*dx
				}
				   x0 += stepx;
				  fraction += dy;    // tak samo jak frakcja -= 2*dy

			        if(resize!=1)
			        {
			        	LCD_box_mid_fill(x0,y0,resize+1,resize+1,color);
			        }
			        else LCD_box_mid_fill(x0,y0,resize,resize,color);
	    	}
	 	}
	 	else
	 	{
	 		int fraction = dx - (dy >> 1);
			while (y0 != y1)
			{
					if (fraction >= 0)
					{
						x0 += stepx;
						fraction -= dy;
					}
					y0 += stepy;
					fraction += dx;

			        if(resize!=1)
			        {
			        	LCD_box_mid_fill(x0,y0,resize+1,resize+1,color);
			        }
			        else LCD_box_mid_fill(x0,y0,resize,resize,color);
			}
	 	}
}


void LCD_box(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color)
{
	 	LCD_line(x0, y0, x1, y0, color,1);
	 	LCD_line(x0, y1, x1, y1, color,1);
	 	LCD_line(x0, y0, x0, y1, color,1);
	 	LCD_line(x1, y0, x1, y1, color,1);
}
void LCD_box_mid(unsigned int x, unsigned int y, unsigned int lx, unsigned int ly, unsigned int color)
{
	 	LCD_line(x-(lx/2), y-(ly/2), x+(lx/2), y-(ly/2), color,1);
	 	LCD_line(x-(lx/2), y+(ly/2), x+(lx/2), y+(ly/2), color,1);
	 	LCD_line(x-(lx/2), y-(ly/2), x-(lx/2), y+(ly/2), color,1);
	 	LCD_line(x+(lx/2), y-(ly/2), x+(lx/2), y+(ly/2), color,1);
}

void LCD_box_mid_fill(unsigned int x, unsigned int y, unsigned int lx, unsigned int ly, unsigned int color)
{
	LCD_area(x-(lx/2),y-(ly/2),x+(lx/2),y+(ly/2));

	if(color==1)
	{
		u8 R[40],B[40],G[40];
		int i=0,k=0;
		u8 a=0;

		LCD_WRITE_COMMAND=(RAMRD);
		i=LCD_WRITE_DATA;
//		i=LCD_WRITE_DATA;
		for(i=0; i<lx*ly; i++)
		{
			  R[i]=LCD_WRITE_DATA;
			  a=0xff;
			  while(a--);
			  G[i]=LCD_WRITE_DATA;
			  a=0xff;
			  while(a--);
			  B[i]=LCD_WRITE_DATA;
			  a=0xff;
			  while(a--);
//			  k++;
		}
		LCD_area(x-(lx/2),y-(ly/2),x+(lx/2),y+(ly/2));

		for(i=0; i<lx*ly; i++)
		{
			  LCD_WRITE_DATA = ((unsigned char)R[i]);
			  LCD_WRITE_DATA = ((unsigned char)G[i]);
			  LCD_WRITE_DATA = ((unsigned char)B[i]);
		}
	}
	else
	{

		int i=0;
		for(i=0; i<(lx+1)*(ly+1); i++)
		{
		  LCD_WRITE_DATA = ((unsigned char)color);
		  LCD_WRITE_DATA = ((unsigned char)(color >> 8));
		  LCD_WRITE_DATA = ((unsigned char)(color >> 16));
		}
	}

}

void LCD_char_lc(char c, int col, int line, int size, int fColor, int bColor, int resize)
{

	 int i,j;
	 int x=0,y=0;

	 	if(resize==1)
		{
			x=col*8;
			y=line*16;
		}
		else if(resize==2 || resize==3)
		{
			x=col*16;
			y=line*32;
		}
		else if(resize>=4)
		{
			x=col*32;
			y=line*64;
		}

	 unsigned int nCols;
	 unsigned int nRows;
	 unsigned int nBytes;
	 unsigned char PixelRow;
	 unsigned char Mask;
	 unsigned int Word0;
	 unsigned int Word1;
	 unsigned char *pFont;

	 unsigned char *pChar;
	 unsigned char *FontTable[] =
	 {
		(unsigned char *)FONT6x8,
//		(unsigned char *)Times19x23,
		(unsigned char *)FONT6x8,
		(unsigned char *)FONT8x16
	 };
	pFont = (unsigned char *)FontTable[size];
	nCols = *pFont;
	nRows = *(pFont + 1);
	nBytes = *(pFont + 2);
	pChar = pFont + (nBytes * (c - 0x1F)) + nBytes - 1;

	if(resize!=1)
	{
		resize/=2;
		resize*=2;
	}

	for (i = resize*nRows ; i >=resize ; i-=resize)
	{

	// copy pixel row from font table and then decrement row
	 PixelRow = *pChar--;

	 Mask = 0x80;

	    for (j = 0; j < resize*nCols+1; j +=resize)
	    {
	        if ((PixelRow & Mask) == 0)Word0 = bColor;
	        else Word0 = fColor;

	        Mask = Mask >> 1;

	        if(resize!=1)
	        {
	        	LCD_box_mid_fill(x+j,y+i,resize+1,resize+1,Word0);
	        }
	        else LCD_box_mid_fill(x+j,y+i,resize,resize,Word0);
	    }
	}

}


void LCD_char(char c, int x, int y, int size, int fColor, int bColor, int resize)
{

	 int i,j;
	 unsigned int nCols;
	 unsigned int nRows;
	 unsigned int nBytes;
	 unsigned char PixelRow;
	 unsigned char Mask;
	 unsigned int Word0;
	 unsigned int Word1;
	 unsigned char *pFont;

	 unsigned char *pChar;
	 unsigned char *FontTable[] =
	 {
		(unsigned char *)FONT6x8,
//		(unsigned char *)Times19x23,
		(unsigned char *)FONT6x8,
		(unsigned char *)FONT8x16
	 };
	pFont = (unsigned char *)FontTable[size];
	nCols = *pFont;
	nRows = *(pFont + 1);
	nBytes = *(pFont + 2);
	pChar = pFont + (nBytes * (c - 0x1F)) + nBytes - 1;

	if(resize!=1)
	{
		resize/=2;
		resize*=2;
	}

	for (i = resize*nRows ; i >=resize ; i-=resize)
	{

	// copy pixel row from font table and then decrement row
	 PixelRow = *pChar--;

	 Mask = 0x80;

	    for (j = 0; j < resize*nCols+1; j +=resize)
	    {
	        if ((PixelRow & Mask) == 0)Word0 = bColor;
	        else Word0 = fColor;

	        Mask = Mask >> 1;

	        if(resize!=1)
	        {
	        	LCD_box_mid_fill(x+j,y+i,resize+1,resize+1,Word0);
	        }
	        else LCD_box_mid_fill(x+j,y+i,resize,resize,Word0);
	    }
	}

}

//void LCD_char(char c, int x, int y, int size, int fColor, int bColor) {
//
////extern const unsigned char FONT6x8[97][8];
////extern const unsigned char FONT8x8[97][8];
////extern const unsigned char FONT8x16[97][16];
// int i,j;
// unsigned int nCols;
// unsigned int nRows;
// unsigned int nBytes;
// unsigned char PixelRow;
// unsigned char Mask;
// unsigned int Word0;
// unsigned int Word1;
// unsigned char *pFont;
//
// unsigned char *pChar;
// unsigned char *FontTable[] =
// {
// 	(unsigned char *)FONT6x8,
// 	(unsigned char *)Times19x23,
// 	(unsigned char *)FONT8x16
// };
//
//
//// get pointer to the beginning of the selected font table
//pFont = (unsigned char *)FontTable[size];
//
//// get the nColumns, nRows and nBytes
//nCols = *pFont;
//nRows = *(pFont + 1);
//nBytes = *(pFont + 2);
//// get pointer to the last byte of the desired character
//pChar = pFont + (nBytes * (c - 0x1F)) + nBytes - 1;
//
//   //////E6,F6,
//LCD_WRITE_COMMAND=(CASET);
//LCD_WRITE_DATA = ((unsigned char)((x>>8) & 0x000000FF));
//LCD_WRITE_DATA = ((unsigned char)(x  & 0x000000FF));
//LCD_WRITE_DATA = ((unsigned char)(((x + nCols-1)>>8) & 0x000000FF));
//LCD_WRITE_DATA = ((unsigned char)((x + nCols-1)  & 0x000000FF));
//LCD_WRITE_COMMAND=(RASET);
//LCD_WRITE_DATA = ((unsigned char)((y>>8)  & 0x000000FF));
//LCD_WRITE_DATA = ((unsigned char)(y  & 0x000000FF));
//LCD_WRITE_DATA = ((unsigned char)(((y + nRows-1)>>8)  & 0x000000FF));
//LCD_WRITE_DATA = ((unsigned char)((y + nRows-1)  & 0x000000FF));
//LCD_WRITE_COMMAND=(RAMWR);
//
//
//// loop on each row, working backwards from the bottom to the top
//	for (i = nRows ; i >=0 ; i--)
//	{
//
//	// copy pixel row from font table and then decrement row
//	 PixelRow = *pChar--;
//
//	// loop on each pixel in the row (left to right)
//	// Note: we do two pixels each loop
//	 Mask = 0x80;
//
//	    for (j = 0; j < nCols; j ++)
//	    {
//	        if ((PixelRow & Mask) == 0)
//	        Word0 = bColor;
//
//	        else Word0 = fColor;
//
//	        Mask = Mask >> 1;
//
////	        LCD_WRITE_DATA = ((unsigned char)(Word0));
////	        LCD_WRITE_DATA = ((unsigned char)(Word0 >> 8));
////	        LCD_WRITE_DATA = ((unsigned char)(Word0 >> 16));
//	    }
//	}
//
//}
void LCD_String( char *pString, int x, int  y, int Size, int fColor, int bColor, int resize)
{
	while (*pString != 0x00)
	{
		LCD_char(*pString++, x, y,Size, fColor, bColor, resize);

		switch(resize)
		{
			case 1:
			{
				x=x+8;
				if(x>=316)
				{
					y=y-16;
					x=3;
				}
				break;
			}
			case 2:
			case 3:
			{
				x=x+16;
				if(x>=240)
				{
					y=y+34;
					x=5;
				}
				break;
			}
			case 4:
			{
				x=x+32;
				if(x>=240)
				{
					y=y+64;
					x=8;
				}
				break;
			}
		}
	}
}

void LCD_String_lc( char *pString, int col, int  line, int fColor, int bColor, int resize)
{
	int y=0,x=0;

	if(resize==1)x=col*8;
	else if(resize==2 || resize==3)x=col*16;
	else if(resize>4)x=col*32;

	while (*pString != 0x00)
	{
		switch(resize)
		{
			case 1:
			{
				x+=8;
				y=line*16;
				if(x>=316)
				{
					line++;
					x=3;
				}
				break;
			}
			case 2:
			case 3:
			{
				x+=16;
				y=line*34;
				if(x>=320)
				{
					line++;
					x=5;
				}
				break;
			}
			case 4:
			{
				x+=32;
				y=line*64;
				if(x>=320)
				{
					line++;
					x=8;
				}
				break;
			}
		}

		LCD_char(*pString++, x, y,2, fColor, bColor, resize);
	}
}
void LCD_pixel_color(int r,int g,int b)
{
	 LCD_WRITE_DATA = ((unsigned char)(r));
	 LCD_WRITE_DATA = ((unsigned char)(g));
	 LCD_WRITE_DATA = ((unsigned char)(b));
}

void LCD_area(int x, int y, int x1, int y1)
{

	LCD_WRITE_COMMAND=(CASET);
	 LCD_WRITE_DATA = ((unsigned char)((x>>8) & 0x000000FF));
	 LCD_WRITE_DATA = ((unsigned char)(x  & 0x000000FF));
	 LCD_WRITE_DATA = ((unsigned char)(((x1)>>8) & 0x000000FF));
	 LCD_WRITE_DATA = ((unsigned char)((x1)  & 0x000000FF));
	LCD_WRITE_COMMAND=(RASET);
	 LCD_WRITE_DATA = ((unsigned char)((y>>8)  & 0x000000FF));
	 LCD_WRITE_DATA = ((unsigned char)(y  & 0x000000FF));
	 LCD_WRITE_DATA = ((unsigned char)((y1>>8) & 0x000000FF));
	 LCD_WRITE_DATA = ((unsigned char)((y1) & 0x000000FF));
	LCD_WRITE_COMMAND=(RAMWR);

}


int test_str_len(char *text, uint8_t size, u8 ret_typ)
{
	int len=0;
	u8 add=0;

	if(size==1)add=8;
	else if(size==2 || size==3)add=16;
	else if(size==4)add=32;

	while(*(text++)!=0)
	{
//		text++;
		len+=add;
	}
	if(ret_typ)return len;
	else return (len/add);
}


