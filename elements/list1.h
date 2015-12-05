#ifndef _LIST1_H
#define _LIST1_H

#include "stmpe811.h"


//#define MAX_BUTTONS 10

//typedef enum { false, true } bool;
typedef struct
{
	char* text;
	int color;
	int colorrev;
//	char* pict;
	char roz;

}Item;

typedef struct
{
	u16 x;
	u16 y;
	u16 ele;
	u8 vis;
	u8 resize;
	u8 first;
	Item it[50];

}List;

List my_list;

void list_it_toggle(u8 ele);
void list_add(List *lis);
void list_show(u8 first);
u8 list_it_clicked(touch *pressed);
u8 list_ex_it_clicked(touch *pressed, u8 id);
u8 list_scroll(touch *pressed, u8 ok);


#endif _LIST1_H_
