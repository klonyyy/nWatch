#include <stm32f4xx.h>
#include <system_stm32f4xx.h>
#include <core_cm4.h>
#include "itoa.h"
#include <misc.h>
#include <stm32f4xx_fsmc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_i2c.h>
#include "button.h"
#include "stmpe811.h"
#include "LCD_6300.h"


u8 button_add(Button *but)
{
	u8 id=0;

	while(Buttons[id].used==1)
	{
		id++;
	}
	if(id>MAX_BUTTONS)return -1;


	Buttons[id].x=but->x;
	Buttons[id].y=but->y;
	Buttons[id].text=but->text;
	Buttons[id].height=but->height;
	Buttons[id].width=but->width;
	Buttons[id].wrap=but->wrap;
	Buttons[id].used=but->used;
	Buttons[id].id=but->id;
	Buttons[id].on=but->on;
	Buttons[id].color=but->color;
	Buttons[id].colorrev=~but->color;

	return id;
}
void button_destroy(Button *but)
{
	u8 id=but->id;
	Buttons[id].used=0;

}
void update(Button *but)
{
	u8 id= but->id;
	Buttons[id].x=but->x;
	Buttons[id].y=but->y;
	Buttons[id].text=but->text;
	Buttons[id].height=but->height;
	Buttons[id].width=but->width;
	Buttons[id].wrap=but->wrap;
	Buttons[id].used=but->used;
	Buttons[id].on=but->on;
	Buttons[id].color=but->color;
	Buttons[id].colorrev=~but->color;
}
void button_show(u8 id)
{
	Buttons[id].on=1;

	if(Buttons[id].wrap==1)
	{
		u16 len=test_str_len(Buttons[id].text,1,1);
		u16 x_t=Buttons[id].x-(len/2);
		u16 y_t=Buttons[id].y-8;
		Buttons[id].width=len;
		Buttons[id].height=19;
//		LCD_box_mid(Buttons[id].x,Buttons[id].y,len+4,20,BLACK);
//		LCD_box_mid(Buttons[id].x,Buttons[id].y,len+8,24,BLACK);
//		LCD_box_mid_fill(Buttons[id].x,Buttons[id].y,len+3,19,~Buttons[id].color);
		LCD_box_mid_round(Buttons[id].x,Buttons[id].y,len+3,19,~Buttons[id].color,12,1);
		LCD_String(Buttons[id].text,x_t,y_t,2,Buttons[id].color,~Buttons[id].color,1);
		LCD_box_mid_round(Buttons[id].x,Buttons[id].y,len,18,0xE4E4E4,12,0);
		LCD_box_mid_round(Buttons[id].x,Buttons[id].y,len+2,20,0xC7C7C7,12,0);
		LCD_box_mid_round(Buttons[id].x,Buttons[id].y,len+4,22,0x888888,12,0);
		LCD_box_mid_round(Buttons[id].x,Buttons[id].y,len+6,24,0x474747,12,0);
	}
	else
	{
		u16 len=test_str_len(Buttons[id].text,1,1);
		u16 x_t=Buttons[id].x-(len/2);
		u16 y_t=Buttons[id].y-8;
//		LCD_box_mid(Buttons[id].x,Buttons[id].y,Buttons[id].width+4,Buttons[id].height,BLACK);
//		LCD_box_mid(Buttons[id].x,Buttons[id].y,Buttons[id].width+8,Buttons[id].height+4,BLACK);
//		LCD_box_mid_fill(Buttons[id].x,Buttons[id].y,Buttons[id].width+3,Buttons[id].height-1,~Buttons[id].color);
		LCD_box_mid_round(Buttons[id].x,Buttons[id].y,Buttons[id].width+3,Buttons[id].height-1,~Buttons[id].color,10,1);
		LCD_String(Buttons[id].text,x_t,y_t,2,Buttons[id].color,~Buttons[id].color,1);
		LCD_box_mid_round(Buttons[id].x,Buttons[id].y,Buttons[id].width-2,Buttons[id].height-2,~Buttons[id].color & 0xf0f0f0,10,0);
		LCD_box_mid_round(Buttons[id].x,Buttons[id].y,Buttons[id].width,Buttons[id].height,~Buttons[id].color & 0xd0d0d0,10,0);
		LCD_box_mid_round(Buttons[id].x,Buttons[id].y,Buttons[id].width+2,Buttons[id].height+2,~Buttons[id].color & 0xb5b5b5,10,0);
		LCD_box_mid_round(Buttons[id].x,Buttons[id].y,Buttons[id].width+4,Buttons[id].height+4,~Buttons[id].color & 0xa0a0a0,10,0);
	}
}

void button_toogle(u8 id)
{
	Buttons[id].color=~Buttons[id].color;
	button_show(id);
}
void button_on(u8 id, u8 on)
{
	if(id==250)
	{
		id=0;

		while(Buttons[id].used==1 )
		{
			Buttons[id].on=on;

			id++;
		}

	}
	else Buttons[id].on=on;
}

u8 button_clicked(touch *pressed)
{

	u8 id=0,ok=0;
	u16 x=pressed->x,y=pressed->y,z=pressed->z;

	while(Buttons[id].used==1 )
	{
		if((pressed->x >=(Buttons[id].x-(Buttons[id].width/2))) && (pressed->x <= (Buttons[id].x+(Buttons[id].width/2))) && (pressed->y >= (Buttons[id].y-(Buttons[id].height/2))) && (pressed->y <= (Buttons[id].y+(Buttons[id].height/2))) && Buttons[id].on)
		{
				if(pressed->last_but==2)
				{
					Buttons[id].color=~Buttons[id].colorrev;
					button_show(id);
				}
				else
				{
					Buttons[id].color=Buttons[id].colorrev;
					button_show(id);
				}
				return id;
		}
		if(pressed->last_but==2 && Buttons[id].on)
		{
			Buttons[id].color=~Buttons[id].colorrev;
			button_show(id);
		}
		id++;
	}

	return 250;
}


