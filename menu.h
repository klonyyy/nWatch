#ifndef _MENU_H
#define _MENU_H

#include "global_inc.h"

static void _cbSelected(WM_MESSAGE * pMsg);
static void _CreateSelected(int Index, WM_HWIN hWin);
static void _cbMenu(WM_MESSAGE * pMsg);
static void _cbDummy(WM_MESSAGE * pMsg);
void Menu(void *pvParameters);

#endif
