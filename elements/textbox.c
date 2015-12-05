#include <stm32f4xx.h>
#include <system_stm32f4xx.h>
#include <core_cm4.h>
#include "itoa.h"
#include <misc.h>
#include <stm32f4xx_fsmc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_i2c.h>
#include "textbox.h"
#include "stmpe811.h"
#include "LCD_6300.h"

u16 lastx;
u8 reff=1;

u8 textbox_add(Textbox *box)
{
	u8 id=0;

	while(Textboxs[id].used==1)
	{
		id++;
	}
	if(id>MAX_TEXTBOX)return -1;


	Textboxs[id].x=box->x;
	Textboxs[id].y=box->y;
	Textboxs[id].text=box->text;
	Textboxs[id].height=box->height;
	Textboxs[id].width=box->width;
	Textboxs[id].used=box->used;
	Textboxs[id].id=box->id;
	Textboxs[id].vis=box->vis;
	Textboxs[id].on=box->on;
	Textboxs[id].count=box->count;
	Textboxs[id].color=box->color;
	Textboxs[id].colorrev=~box->color;
	Textboxs[id].count=strlen(Textboxs[id].text);

	return id;
}
void textbox_destroy(Textbox *box)
{
	u8 id=box->id;
	Textboxs[id].used=0;
}
void textbox_show(u8 id)
{
//		u16 len=test_str_len(Textboxs[id].text,1);
		u16 x_t=Textboxs[id].x-(Textboxs[id].width/2-4);
		u16 y_t=Textboxs[id].y-8;
//		LCD_box_mid(Textboxs[id].x,Textboxs[id].y,Textboxs[id].width,Textboxs[id].height,Textboxs[id].colorrev);
//		LCD_box_mid_fill(Textboxs[id].x,Textboxs[id].y,Textboxs[id].width-4,Textboxs[id].height-4,~Textboxs[id].colorrev);
		LCD_box_mid_round(Textboxs[id].x,Textboxs[id].y,Textboxs[id].width,Textboxs[id].height,Textboxs[id].colorrev,9,0);
		LCD_box_mid_round(Textboxs[id].x,Textboxs[id].y,Textboxs[id].width-4,Textboxs[id].height-4,~Textboxs[id].colorrev,9,1);
//		LCD_String(Textboxs[id].text,x_t,y_t,2,~Textboxs[id].color,Textboxs[id].color,1);
		LCD_String_tb(Textboxs[id].text,Textboxs[id].x-(Textboxs[id].width/2-4),Textboxs[id].y-8,~Textboxs[id].color,Textboxs[id].color,1,Textboxs[id].vis,Textboxs[id].first);
}
void textbox_text(u8 id, char *text)
{
	u16 x_t=Textboxs[id].x-(Textboxs[id].width/2-4);
	u16 y_t=Textboxs[id].y-8;
	LCD_box_mid_round(Textboxs[id].x,Textboxs[id].y,Textboxs[id].width,Textboxs[id].height,Textboxs[id].colorrev,9,0);
	Textboxs[id].text=text;
	LCD_String_tb(Textboxs[id].text,Textboxs[id].x-(Textboxs[id].width/2-4),Textboxs[id].y-8,~Textboxs[id].color,Textboxs[id].color,1,Textboxs[id].vis,Textboxs[id].first);

}
void textbox_on(u8 id, u8 on)
{
	Textboxs[id].on=on;
}
void LCD_String_tb(char *text, u16 x, u16 y, int color, int bcolor, u8 resize, int cnt, u8 first)
{
	int k=0;
	u8 add=0;
	int j=0;

	if(resize==1)add=8;
	else if(resize==2 || resize==3)add=16;
	else if(resize==4)add=32;

	k=test_str_len(text,resize,0);

	if(cnt<k)
	{
		k=k-cnt;
	}
	else
	{
		cnt=k;
		k=0;
	}

	text+=k;
	text-=first;

//	while(*text != 0)
	while(j<cnt)
	{
		LCD_char(*(text++), x, y,2, color, bcolor, resize);
		x+=add;
		j++;
	}
}

u8 textbox_clicked(touch *pressed)
{

	u8 id=0,ok=0;
	u16 x=pressed->x,y=pressed->y,z=pressed->z;

	while(Textboxs[id].used==1 )
	{
		if((pressed->x >=(Textboxs[id].x-(Textboxs[id].width/2))) && (pressed->x <= (Textboxs[id].x+(Textboxs[id].width/2))) && (pressed->y >= (Textboxs[id].y-(Textboxs[id].height/2))) && (pressed->y <= (Textboxs[id].y+(Textboxs[id].height/2))) && Textboxs[id].on)
		{
				if(pressed->last_box==2)
				{
					Textboxs[id].color=~Textboxs[id].colorrev;
					textbox_show(id);
				}
				else
				{
					Textboxs[id].color=Textboxs[id].colorrev;
					textbox_show(id);
				}
				return id;
		}
		if(pressed->last_box==2 && Textboxs[id].on)
		{
			Textboxs[id].color=~Textboxs[id].colorrev;
			textbox_show(id);
		}
		id++;
	}

	return 250;
}

void textbox_scroll(touch *pressed,u8 id)
{
  	if((lastx-pressed->x>10) && pressed->x!=400  )
  	{
  		if(lastx!=0 && Textboxs[id].first<(Textboxs[id].count-Textboxs[id].vis))
  		{
  			Textboxs[id].first++;
  			reff=1;
  		}

		lastx=pressed->x;
  	}
  	else if((pressed->x-lastx>10) && pressed->x!=400)
  	{
  		if(lastx!=0 && Textboxs[id].first>0)
  		{
  			Textboxs[id].first--;
  			reff=1;
  		}

  		lastx=pressed->x;
  	}
  	if(reff)
  	{
//  		list_show(my_list.first);
  		if(Textboxs[id].on)textbox_show(id);
  		reff=0;
  	}

  	if(pressed->x==400)
  	{
  		lastx=0;
  	}
}
