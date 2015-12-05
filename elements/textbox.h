#ifndef _TEXTBOX_H
#define _TEXTBOX_H

#include "stmpe811.h"

#define MAX_TEXTBOX 10

typedef struct
{
	u16 x;
	u16 y;
	u8 id;
	char *text;
	u8 used;
	u16 height;
	u16 width;
	int color;
	int colorrev;
	int count;
	int first;
	u16 vis;
	u8 on;

}Textbox;

Textbox Textboxs[MAX_TEXTBOX];

u8 textbox_add(Textbox *box);
void textbox_show(u8 id);
u8 textbox_clicked(touch *pressed);
void textbox_destroy(Textbox *but);
void textbox_on(u8 id, u8 on);
void LCD_String_tb(char *text, u16 x, u16 y, int color, int bcolor, u8 resize, int cnt, u8 first);
void textbox_scroll(touch *pressed,u8 id);

#endif
