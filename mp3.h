#ifndef _MP3_H
#define _MP3_H

#include "DIALOG.h"
#include "global_inc.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x04)
#define ID_BUTTON_0 (GUI_ID_USER + 0x08)
#define ID_BUTTON_1 (GUI_ID_USER + 0x09)
#define ID_PROGBAR_0 (GUI_ID_USER + 0x0A)
#define ID_TEXT_0 (GUI_ID_USER + 0x0B)
#define ID_TEXT_1 (GUI_ID_USER + 0x0E)
#define ID_BUTTON_2 (GUI_ID_USER + 0x11)
#define ID_SLIDER_0 (GUI_ID_USER + 0x12)
#define ID_SLIDER_1 (GUI_ID_USER + 0x13)

WM_HWIN CreateWindow(void);
static void _cbDialog(WM_MESSAGE * pMsg);
void MP3_player(void *pvParameters);

#endif
