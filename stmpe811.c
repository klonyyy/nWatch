#include <stm32f4xx.h>
#include <system_stm32f4xx.h>
#include <core_cm4.h>
#include "itoa.h"
#include <misc.h>
#include <stm32f4xx_fsmc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_i2c.h>
#include "stmpe811.h"
#include "list1.h"
#include "textbox.h"

volatile u16 xr=0,yr=0;
u8 once=1;
int sek=0;
u16 lastyy,lastid,lasttb,scrl=0,lastbid,first;
char t[10];

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

uint8_t stmpe811_TestConnection(void)
{
    uint8_t tmp[3] = { 0 };
    uint16_t id=0;
    I2C_Read_Reg(0x00,(uint8_t*)&id,2);
    id=(id<<8)|(id>>8);
    return (id==0x811) ? 1 : 0;
}
void stmpe811_init(void)
{
	   I2C_Write_Byte (0x03,  0x02);
	   I2C_Write_Byte (0x04,  0x04);
	   I2C_Write_Byte (0x0A,  0x03);//0x02
	   I2C_Write_Byte(0x20,  0x49);
	   int var;
	   for ( var= 0; var < 0xfffff; ++var) {};
	   I2C_Write_Byte(0x21,  0x01);
	   I2C_Write_Byte (0x17,  0x00);
	   I2C_Write_Byte(0x41,  0x9a);
//	   I2C_Write_Byte(0x41,  0xa3);
	   I2C_Write_Byte(0x4A,  0x01);
	   I2C_Write_Byte (0x4B,  0x01);
	   I2C_Write_Byte (0x4B,  0x00);

	   I2C_Write_Byte(0x56,  0x07);////0x07
	   I2C_Write_Byte(0x58,  0x01);
	   I2C_Write_Byte(0x40,  0x03);////0x01
	   I2C_Write_Byte(0x0B,  0xFF);
	   I2C_Write_Byte(0x09,  0x03); //0x07
}
void I2C_Read_Reg(char addr,  char * Buffer, unsigned int N)
{
  //reads N registers values into buffer
  unsigned int i;

  //Check, if I2C is free
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));

  I2C_GenerateSTART(I2C2,ENABLE);
  //Test on EV5 and clear it
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
  //Send chip address, set I2C master in transmiter mode
  I2C_Send7bitAddress(I2C2, STMPE811_ADRR, I2C_Direction_Transmitter);
  //Test on EV6 and clear it
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  //Send base register address, set address autoincrement
  I2C_SendData(I2C2, addr);
  //Test on EV8 and clear it
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  //Re-generate START, transmition from slave beginning
  I2C_GenerateSTART(I2C2,ENABLE);
  //Test on EV5 and clear it
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
  //Send chip address, set I2C master in receiver mode
  I2C_Send7bitAddress(I2C2, STMPE811_ADRR, I2C_Direction_Receiver);
  //Test on EV6 and clear it
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

  i=0;               //Current read count
  while(N) {
    //Before receiving last byte, disable acknowledge and generate stop
    if(N == 1) {
      I2C_AcknowledgeConfig(I2C2, DISABLE);
      I2C_GenerateSTOP(I2C2, ENABLE);
    }
    //Test on EV7 and clear it
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED));
    //Read a byte from the LI35
    Buffer[i] = I2C_ReceiveData(I2C2);
    i++;
    N--;
  }

  I2C_AcknowledgeConfig(I2C2, ENABLE);

}

void I2C_Write_Byte( uint8_t WriteAddr,uint8_t pBuffer )
{
    I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C2, STMPE811_ADRR, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C2, WriteAddr);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C2, pBuffer);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C2, ENABLE);
}
u8 read_touch_but(touch *pressed)
{
	u8 id=250;

	if(down==1)
	{
		pressed->hold_but=1;

		pressed->y=((yr-450)*240)/(3000); ///TP_Y=((TP_Y-CAL_Y_0)*320)/(CAL_Y-CAL_Y_0); ////y
		pressed->x=((xr-600)*320)/(3200); ///TP_X=((TP_X-511)*240)/(CAL_X_0-CAL_X);
		pressed->x=320-pressed->x;



//		LCD_box_mid_fill(pressed->x,pressed->y,3,3,0xff00ff);

		if(pressed->last_but==0)
		{
			id=button_clicked(pressed);
			pressed->last_but=2;
			lastbid=id;
		}
		if(id!=250)return id;

	}
	if(down==0)
	{
		id=button_clicked(pressed);
		pressed->last_but=0;
//		pressed->last_down=250;
		pressed->hold_but=0;
		id=250;
		pressed->x = 400;
		pressed->y = 400;
		pressed->z= 0;
	}
//	return pressed->last_down;
	return id;
}
u8 read_touch_list(touch *pressed)
{
	u8 id=250;

	if(down==1)
	{
		if(pressed->y==400)first=400;        ///for scrolling

		pressed->y=((yr-450)*240)/(3000); ///TP_Y=((TP_Y-CAL_Y_0)*320)/(CAL_Y-CAL_Y_0);
		pressed->x=((xr-600)*320)/(3200); ///TP_X=((TP_X-511)*240)/(CAL_X_0-CAL_X);
		pressed->x=320-pressed->x;

		if(first==400)
		{
			first=pressed->y;                 ////for scrolling
			sek=0;
		}

//		LCD_box_mid_fill(pressed->x,pressed->y,3,3,0x0000ff);

		if(!scrl)scrl=list_scroll(pressed,0);
		else list_scroll(pressed,0);

		id=list_it_clicked(pressed);

		if(id!=250)
		{
			pressed->last_list=3;
			lastid=id;
		}

		if(pressed->last_list==3 && pressed->last_item==0)
		{
			if(list_ex_it_clicked(pressed,lastid)==0)pressed->last_item=1;
		}

		if(hold>180 && !scrl)
		{
			down=0;
			hold=0;
			pressed->hold_it=1;
//			return id;
		}

		if(sek>100000)                              ////for scrolling
		{
			first=pressed->y;
			sek=0;
		}

	}
	if(down==0)
	{


		pressed->last_list=2;
		list_scroll(pressed,0);
		id=list_it_clicked(pressed);

		if(pressed->y-first>30 && pressed->y!=400)
		{
			int del=1000;
			u8 az=(pressed->y-first)/20;
			while(az--)
			{
				list_scroll(pressed,2);
				delay(del);
				del/=az;
				del+=(del*az);
			}
		}
		else if(first-pressed->y>30 && pressed->y!=400)
		{
			int del=1000;
			u8 az=(first-pressed->y)/20;
//			LCD_String(itoa(az,t,10),230,300,2,0xff0000,0x00ff00,1);
			while(az--)
			{
				list_scroll(pressed,1);
				delay(del);
				del/=az;
				del+=(del*az);
			}
		}

		pressed->last_list=0;
		pressed->x = 400;
		pressed->y = 400;
		pressed->z= 0;
		pressed->last_item=0;
		if(lastid==id && !scrl)return id;
		scrl=0;
	}

	return 250;
}
u8 read_touch_textbox(touch *pressed)
{
	u8 id=250;

	if(down==1)
	{
		pressed->y=((yr-450)*240)/(3000); ///TP_Y=((TP_Y-CAL_Y_0)*320)/(CAL_Y-CAL_Y_0);
		pressed->x=((xr-600)*320)/(3200); ///TP_X=((TP_X-511)*240)/(CAL_X_0-CAL_X);
		pressed->x=320-pressed->x;

		LCD_box_mid_fill(pressed->x,pressed->y,3,3,0x0000ff);

		if(pressed->last_box==0)
		{
			id=textbox_clicked(pressed);
			lasttb=id;
			pressed->last_box=2;
		}

		if(lasttb!=250)
		{
//			id=textbox_clicked(pressed);
			textbox_scroll(pressed,lasttb);
		}

		if(hold>180 && id!=250)
		{
			hold=0;
			pressed->hold_tb=1;
//			return id;
		}

		return lasttb;
//		return id;
	}
	if(down==0)
	{
		id=textbox_clicked(pressed);
		pressed->last_box=0;
		pressed->hold_tb=0;
//		id=250;
		pressed->x = 400;
		pressed->y = 400;
		pressed->z= 0;
	}

	return id;

}

u8 read_touch(touch *pressed)
{
	u8 id=250;

	if(down==1)
	{
		pressed->y=((yr-450)*240)/(3000); ///TP_Y=((TP_Y-CAL_Y_0)*320)/(CAL_Y-CAL_Y_0);
		pressed->x=((xr-600)*320)/(3200); ///TP_X=((TP_X-511)*240)/(CAL_X_0-CAL_X);
		pressed->x=320-pressed->x;
	}
	else if(down==0)
	{
		pressed->x = 400;
		pressed->y = 400;
		pressed->z= 0;
	}

	return down;

}
u8 touch_reg(touch *pressed)
{
	u8 zk=0;
	int xa=-1,ya=-1;
	u8 x[1]={0},y[1]={0};

	I2C_Read_Reg( 0x40,&zk,1);

	if((zk & 0x80))
	{
		I2C_Read_Reg( 0x4f, x,2 );
		I2C_Read_Reg( 0x4d, y,2 );
		ya = ((x[0]<<8)|x[1]);
		xa = ((y[0]<<8)|y[1]);
		I2C_Write_Byte(0x0b, 0xff);
		I2C_Write_Byte(0x4b, 0x01);
		I2C_Write_Byte(0x4b, 0x00);

		pressed->y=((ya-450)*240)/(3000); ///TP_Y=((TP_Y-CAL_Y_0)*320)/(CAL_Y-CAL_Y_0);
		pressed->x=((xa-600)*320)/(3200); ///TP_X=((TP_X-511)*240)/(CAL_X_0-CAL_X);
//		pressed->x=320-pressed->x;
		return 1;
	}
	pressed->y=400;
	pressed->x=400;
	I2C_Write_Byte(0x0b, 0xff);
	I2C_Write_Byte(0x4b, 0x01);
	I2C_Write_Byte(0x4b, 0x00);
	return 0;
}


void EXTI9_5_IRQHandler(void)
{
	u8 zk=0;
	u8 x[1]={0},y[1]={0};

	I2C_Read_Reg( 0x0b,&zk,1);

	if((zk & 0x02))
	{
		down=1;
		I2C_Read_Reg( 0x4f, x,2 );
		I2C_Read_Reg( 0x4d, y,2 );
		yr = ((x[0]<<8)|x[1]);
		xr = ((y[0]<<8)|y[1]);
		hold++;
	}
	else if((zk & 0x01))
	{
			down=0;
			hold=0;

	}

	I2C_Write_Byte(0x0b, 0xff);
	I2C_Write_Byte(0x4b, 0x01);
	I2C_Write_Byte(0x4b, 0x00);

	EXTI->PR |= EXTI_PR_PR5;
}
void stmpe(void)
{
	sek++;
}

