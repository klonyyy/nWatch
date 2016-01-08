#ifndef _BAR_H
#define _BAR_H

#include "DIALOG.h"

#define ID_WINDOW_2  (GUI_ID_USER + 0x14)
#define ID_PROGBAR_2  (GUI_ID_USER + 0x15)
#define ID_TEXT_2    (GUI_ID_USER + 0x16)
#define ID_BACK    (GUI_ID_USER + 0x19)

WM_HWIN CreateBar(void);
static void _cbDialog(WM_MESSAGE * pMsg);

#endif
