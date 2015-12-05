#ifndef _BUTTON_H
#define _BUTTON_H

#include "stmpe811.h"


#define MAX_BUTTONS 18

//typedef enum { false, true } bool;

typedef u8 uint8_t;

typedef struct
{
	u16 x;
	u16 y;
	u16 width;
	u16 height;
	char *text;
	u8 wrap;
	u16 id;
	int color;
	int colorrev;
	u8 used;
	u8 on;

}Button;

Button Buttons[MAX_BUTTONS];

void button_toogle(u8 id);
u8 button_add(Button *but);
void button_show(u8 id);
u8 button_clicked(touch *pressed);
void button_destroy(Button *but);
void button_on(u8 id, u8 on);
void update(Button *but);

#endif
