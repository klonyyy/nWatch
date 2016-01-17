#include "heading.h"
#include "menu.h"

int aa=-1;

/*********************************************************************
*
*       _cbHeading
*
* Purpose:
*   Callback function of heading window containing logo and battery symbol.
*/
static void _cbHeading(WM_MESSAGE * pMsg)
{
  int xSize, xPos;
  const GUI_BITMAP * pBm;
  WM_HWIN hWin,hButton;
  static int Index,NCode;
  int a=-2;

  hWin = pMsg->hWin;
  switch (pMsg->MsgId)
  {
  case WM_CREATE:
    //
    // Create timer to be used to modify the battery symbol
    //
	hButton=BUTTON_CreateEx(10,15,50,40,hWin,WM_CF_SHOW,0,(GUI_ID_USER + 0x18));
    WM_CreateTimer(hWin, 0, 1000, 0);
    break;
  case WM_TIMER:
    //
    // Modify battery symbol on timer message
    //
    Index++;
    if (Index == GUI_COUNTOF(_apbmCharge)) {
      Index = 0;
    }
    WM_InvalidateWindow(hWin);
    WM_RestartTimer(pMsg->Data.v, 0);
    break;
  case WM_NOTIFY_PARENT:
  {
//     Id    = WM_GetId(pMsg->hWinSrc);      // Id of widget
     NCode = pMsg->Data.v;                 // Notification code
     switch (NCode)
     {
		 case WM_NOTIFICATION_CLICKED:
		 {
			 a=-1;

			if(aa!=-1)
			{
				 xTaskCreate(Menu,(char const*)"Menu",512,NULL,tskIDLE_PRIORITY + 6,&Menu_Handle);
				 if(aa==2)vTaskDelete(MP3_Handle);
				 if(aa==1)vTaskDelete(Manager_Handle);
				 aa=-1;
			}
			 break;
		 }
     }
     break;
   }
  case WM_PAINT:
  {
		xSize = WM_GetWindowSizeX(hWin);
		xPos = xSize;
		GUI_SetColor(0xf8f8f8);
		GUI_FillRect(0, 0, xSize - 1, 57);
		GUI_SetColor(0xb2b2b2);
		GUI_FillRect(0, 58, xSize - 1, 59);
		pBm = &_bmBatteryEmpty_27x14;
		xPos -= pBm->XSize + 3;
		GUI_DrawBitmap(pBm, xPos, 3);
		pBm = _apbmCharge[Index];
		GUI_DrawBitmap(pBm, xPos, 3);
		 GUI_SetFont(&GUI_Font24_ASCII);
		 GUI_SetColor(GUI_BLACK);
		 GUI_SetTextMode(GUI_TM_TRANS);

		 xQueueReceive(xQueue_men, &a, (portTickType) 10);

		 if(a==-2)a=aa;

			 switch(a)
			 {
				  case -1:
				  {
					  GUI_DispStringHCenterAt("Menu", xSize / 2, 20);
					  break;
				  }
				  case 1:
				  {
					  GUI_DispStringHCenterAt("Manager", xSize / 2, 20);
					  break;
				  }
				  case 2:
				  {
					  GUI_DispStringHCenterAt("MP3 Player", xSize / 2, 20);
					  break;
				  }
//			 	 default:
//				 GUI_DispStringHCenterAt(itoa(xPortGetFreeHeapSize(),t,10), xSize / 2, 20);
//			 		 GUI_DispDecAt(a,160,230,5);
			 }
			 aa=a;
		break;
  }
  default:
    WM_DefaultProc(pMsg);
  }
}


static void _cbDummy1(WM_MESSAGE * pMsg)
{
  switch (pMsg->MsgId) {
  default:
    WM_DefaultProc(pMsg);
  }
}

void Heading_Task( void * pvParameters)
{
	int l=0;

	portTickType xLastFlashTime;
	xLastFlashTime = xTaskGetTickCount();

	WM_HWIN hWinHeading;
	WM_HWIN hWinBase;     // Heading window
	hWinBase     = WM_CreateWindow(0,  0, 320, 60,  WM_CF_SHOW, _cbDummy1, 0);
	hWinHeading  = WM_CreateWindowAsChild(0,  0, 320, 60, hWinBase,     WM_CF_SHOW, _cbHeading, 0);

	WM_HWIN hItem,hText;
	WM_HWIN hButton3;

	while(1)
	{
		vTaskDelay(100);
		WM_Paint(hWinHeading);
//		if(l<100)mem[l++]=4000-xPortGetFreeHeapSize();
//		else l=0;
//
//	    ADC_SoftwareStartConv(ADC1);
//	    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
//	    int result= ADC_GetConversionValue(ADC1);
//	    result = result * 8059/10000;
//	    hItem = WM_GetDialogItem(hWinHeading, ID_PROGBAR_2);
//	    hText = WM_GetDialogItem(hWinHeading, ID_TEXT_2);
//	    PROGBAR_SetValue(hItem,result*100/3100);
//	    TEXT_SetText(hText, itoa(uxTaskGetNumberOfTasks(),t,10));

	}
}
