#ifndef __STMPE811_H
#define __STMPE811_H

#include "stm32f4xx.h"

#define STMPE811_ADRR 0x82

//typedef u8 uint8_t;
//typedef u16 uint16_t;


typedef struct
{
	u16 x;
	u16 y;
	u16 z;
	u8 pressed;
	u8 last_but;
	u8 last_box;
	u8 last_list;
	u8 last_down;
	u8 last_item;
	u8 hold_it;
	u8 hold_tb;
	u8 hold_but;

}touch;

u8 down;
u16 hold;

uint8_t stmpe811_TestConnection(void);
void stmpe811_init(void);
//void I2C_Read_Reg(  uint8_t reg_adres, uint8_t * dane, uint8_t len );
void I2C_Read_Reg(char addr,  char * Buffer, unsigned int N);
void I2C_Write_Byte( uint8_t WriteAddr,uint8_t pBuffer );
u8 read_touch_but(touch *pressed);
u8 read_touch_list(touch *pressed);
void EXTI9_5_IRQHandler(void);
u8 read_touch(touch *pressed);
u8 touch_reg(touch *pressed);
void stmpe(void);
void i2c_ini(void);
#endif
