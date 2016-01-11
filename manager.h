#ifndef _MANAGER_H
#define _MANAGER_H

#include "global_inc.h"

#define ID_FRAMEWIN_1  (GUI_ID_USER + 0x20)
#define ID_GRAPH_0  (GUI_ID_USER + 0x21)
#define ID_GRAPH_1  (GUI_ID_USER + 0x22)

static void _cbDialog(WM_MESSAGE * pMsg);
WM_HWIN CreateManager(void);
void Manager(void *pvParameters);

#endif
