#include <stm32f4xx.h>
#include <system_stm32f4xx.h>
#include <core_cm4.h>
#include "itoa.h"
#include <misc.h>
#include <stm32f4xx_fsmc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_i2c.h>
#include "list1.h"
#include "stmpe811.h"
#include "LCD_6300.h"

int ref=0,lasty=0;

void list_add(List *lis)
{
	my_list.x=lis->x;
	my_list.y=lis->y;
	my_list.ele=lis->ele;
	my_list.vis=lis->vis;
	my_list.resize=lis->resize;
	my_list.first=lis->first;

	if(my_list.ele < my_list.vis)my_list.vis=my_list.ele;

	for(int i=0;i<my_list.ele;i++)
	{
		my_list.it[i]=lis->it[i];
//		my_list.it[i].pict="0:system/fols.rgb";
		my_list.it[i].color=0x000000;
		my_list.it[i].colorrev=0xffffff;
	}

}
void list_show(u8 first)
{
	char t[10];
//	LCD_paint(0x000000);
//	LCD_String(itoa(((my_list.first*160)/my_list.ele)+20,t,10),240,190,1,RED,GREEN,1);
//	LCD_String(itoa(my_list.first,t,10),240,200,1,RED,GREEN,1);
//	LCD_String(itoa(my_list.ele,t,10),240,220,1,RED,GREEN,1);
	u8 dod=2;

	for(int i=my_list.first;i<my_list.first+my_list.vis;i++)
	{
		u8 len=test_str_len(my_list.it[i].text,my_list.resize,0);

//		if(my_list.first == i)LCD_String_lc(my_list.it[i].text,my_list.x+2,my_list.y+(i-my_list.first)*3+2,YELLOW,my_list.it[i].color,1);
//		else if(my_list.first+my_list.vis-1==i)LCD_String_lc(my_list.it[i].text,my_list.x +2,my_list.y+(i-my_list.first)*3-2,YELLOW,my_list.it[i].color,1);
//		else LCD_String_lc(my_list.it[i].text,my_list.x,my_list.y+(i-my_list.first),YELLOW,my_list.it[i].color,4);

		LCD_String_lc(my_list.it[i].text,my_list.x,my_list.y+(i-my_list.first),YELLOW,my_list.it[i].color,my_list.resize);

		len++;
		u8 ad=test_resize(my_list.resize);

		dod+=2*ad;

		while(len<16)
		{
			LCD_char(' ',ad*(my_list.x+len)+2,dod,2,BLACK,BLACK,my_list.resize);
			len++;
		}
		dod+=2;
//		check_ext(my_list.it[i].text,i);
		if(my_list.it[i].roz>0 && my_list.it[i].roz!='u')icon(1,my_list.x+10,(my_list.y+(i-my_list.first))*35,27,27);
		else if(my_list.it[i].roz=='u')icon(2,my_list.x+10,(my_list.y+(i-my_list.first))*35,27,27);
		else icon(0,my_list.x+10,(my_list.y+(i-my_list.first))*35,27,27);
	}

	LCD_box_mid_round(300,123,20,169,BLACK,3,1);
	LCD_box_mid_round(300,123,10,170,WHITE,3,0);
	LCD_box_mid_round(300,(my_list.first*135/(my_list.ele-my_list.vis))+55,6,30,WHITE,2,1);

//	LCD_box_mid_fill(0,40,320,20,BLACK);

}

void list_it_toogle(u8 id)
{
//	list[]
}

u8 list_ex_it_clicked(touch *pressed, u8 id)
{
	int x=0;

	if(my_list.resize==1)x=16;
	else if(my_list.resize==2 || my_list.resize==3)x=35;
	else if(my_list.resize>4)x=64;

//	LCD_box(5,my_list.y*x+(id-(my_list.first))*x,320,my_list.y*x+x+(id-(my_list.first))*x,BLUE);

		if((pressed->x >= 3) && (pressed->x <= 320) && (pressed->y >= my_list.y*x+(id-(my_list.first))*x ) && (pressed->y <= my_list.y*x+x+(id-(my_list.first))*x ))
		{
			return 1;
		}
		else
		{
			my_list.it[id].color=~my_list.it[id].colorrev;
			list_show(my_list.first);
			return 0;
		}

}

u8 list_it_clicked(touch *pressed)
{

	u8 id=0,x=0;
	u8 idd=250;
	u8 ele=my_list.vis;

	if(my_list.resize==1)x=16;
	else if(my_list.resize==2 || my_list.resize==3)x=35;
	else if(my_list.resize>4)x=64;

	while(id<ele)
	{
//		LCD_box(5,my_list.y*x+id*x,320,my_list.y*x+x+id*x,RED,0);

		if((pressed->x >= 3) && (pressed->x <= 320) && (pressed->y >= my_list.y*x+id*x) && (pressed->y <= my_list.y*x+x+id*x) && pressed->last_list!=3)
		{
				if(pressed->last_list==2)
				{
						my_list.it[my_list.first + id].color=~my_list.it[my_list.first + id].colorrev;
						list_show(my_list.first);
				}
				else
				{
						my_list.it[my_list.first + id].color=my_list.it[my_list.first + id].colorrev;
						list_show(my_list.first);
				}

				idd=my_list.first+id;
		}
		else
		{
				my_list.it[my_list.first+id].color=~my_list.it[my_list.first +id].colorrev;

		}

		id++;
	}

//	if(idd!=250)list_show1(my_list.first);

	return idd;
}

u8 list_scroll(touch *pressed, u8 ok)
{
  	if(((lasty-pressed->y>30) && pressed->y!=400)  || ok==1 )
  	{
  		if(lasty!=0 && my_list.first<(my_list.ele-my_list.vis))
  		{
  			my_list.first++;
  			ref=1;
  		}

		lasty=pressed->y;
  	}
  	else if(((pressed->y-lasty>30) && pressed->y!=400)  || ok==2)
  	{
  		if(lasty!=0 && my_list.first>0)
  		{
  			my_list.first--;
  			ref=1;
  		}

  		lasty=pressed->y;
  	}
  	if(ref)
  	{
  		list_show(my_list.first);
  		ref=0;
  		return 1;
  	}
  	if(pressed->y==400)
  	{
  		lasty=0;
  	}

  	return 0;
}

