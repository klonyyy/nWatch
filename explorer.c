
#include <stm32f4xx.h>
#include <system_stm32f4xx.h>
#include <core_cm4.h>
#include "itoa.h"
#include <misc.h>
#include "list1.h"
#include "stmpe811.h"
#include "LCD_6300.h"
#include "explorer.h"
#include "ff.h"
#include "textbox.h"
//////////USB//////////////////
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"

char tab[6*siz][siz];
char dir_path[100];
char fols[2187]={0};
char files[2187]={0};
char fileb[2187]={0};

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_Core __ALIGN_END;
char t[10];
static TCHAR lfname[_MAX_LFN];
uint32_t s1;
DIR dir;
FILINFO fno;
FATFS fs;
FIL fil;
List list;
FRESULT fres;


char* explorer(void)
{
	 LCD_paint(0x000000);

	  touch pressed={0};

	  Textbox path;
	  path.color=0x000000;
	  path.colorrev=0xffff00;
	  path.height=25;
	  path.width=300;
	  path.used=1;
	  path.vis=35;
	  path.text="0:";
	  path.x=160;
	  path.y=16;
	  path.on=1;
	  path.id=textbox_add(&path);
	  textbox_show(path.id);

	  u8 ik=250;
	  u8 tx=250;
	  for(int i=0;i<100;i++)
	  {
		  dir_path[i]=0;
	  }
	  strcpy(dir_path,"0:");

	  int last_it=0;

	  f_open(&fil,"system/fols.rgb", FA_READ | FA_OPEN_EXISTING );
	  f_read(&fil, fols, 2187, &s1);
	  f_close(&fil);

	  f_open(&fil,"system/files.rgb", FA_READ | FA_OPEN_EXISTING );
	   f_read(&fil, files, 2187, &s1);
	   f_close(&fil);

	  f_open(&fil,"system/fileb.rgb", FA_READ | FA_OPEN_EXISTING );
	  f_read(&fil, fileb, 2187, &s1);
	  f_close(&fil);

	  open_dir(dir_path);

	  while(1)
	  {
//		  tx=read_touch_textbox(&pressed);
		  ik=read_touch_list(&pressed);

		  	if(ik!=250 && !pressed.hold_it)
		  	{
		  		u8 l=strlen(dir_path)-1;
		  		u8 fl=0;
		  		u8 flen=0;
		  		u8 b=l;
		  		u8 back=0;
		  		u8 file=0;
		  		//  if there's two dots                                   or it was a file recenty opened
		  		if((*list.it[ik].text=='.' && *list.it[ik].text++=='.' ) || last_it==1)
		  		{
		  			while(dir_path[b]!='/')
		  			{
		  				dir_path[b--]=0;
		  				l--;
		  			}
		  			dir_path[b--]=0;
		  			l--;
		  			back=1;
		  		}

		  		if(dir_path[l]!='/' && back==0 && last_it==0)dir_path[++l]='/';

		  		while(*list.it[ik].text != 0 && back==0 && last_it==0)
		  		{
		  			if(*list.it[ik].text=='.')
		  			{
		  				file=1;
		  			}
		  			list.it[ik].text++;
		  			flen++;
		  		}
		  		list.it[ik].text-=flen;
		  		l++;
				while(*list.it[ik].text != 0 && back==0 && last_it==0)
				{
					dir_path[l++]=*list.it[ik].text;
					list.it[ik].text++;
				}

				if(file==0)
				{
					open_dir(dir_path);
					last_it=0;
				}
				else if(file==1 && last_it==0)
		  		{
		  			list.it[ik].text-=flen;
//		  			if(my_list.it[ik].roz=='b')bitmap_BMP(dir_path,0,0);
//		  			else if(my_list.it[ik].roz=='j')load_jpg(&fil,dir_path,Buff,sizeof(Buff));
//		  			else if(my_list.it[ik].roz=='a')play_video(dir_path);
		  			while(pressed.y==0){};
		  			last_it=1;
		  			return dir_path;
		  		}

//		  		LCD_String_lc(dir_path,0,0,GREEN,~GREEN,1);
		  	}
		  	else if(ik !=250 && pressed.hold_it)
		  	{
//		  		while(read_touch_list(&pressed)!=250)
//		  		{
//
//		  		}
		  		pressed.hold_it=0;

		  		char *s1;
		  		s1=list.it[ik].text;   // my_list
		  		char *k;
				textbox_on(path.id, 0);
				LCD_paint(0x000000);
		  		//k=keyboard(s1);
		  		textbox_on(path.id, 1);
		  		u8 a=strlen(dir_path);
		  		char dir_path_new[a+30];
		  		strcpy(dir_path_new,dir_path);
		  		u8 b=a;
		  		dir_path[a++]='/';
		  		while(*s1!=0)
		  		{
		  			dir_path[a++]=*s1++;
		  		}
		  		dir_path_new[b++]='/';
		  		while(*k!=0)
		  		{
		  			dir_path_new[b++]=*k++;
		  		}
		  		dir_path_new[b]=0;
//		  		LCD_String_lc(dir_path,0,0,GREEN,~GREEN,1);
//		  		LCD_String_lc(dir_path_new,1,1,GREEN,~GREEN,1);
		  		NVIC_DisableIRQ(EXTI9_5_IRQn);
		  		f_close(&fil);
		  		fres=f_rename(dir_path,dir_path_new);
//		  		LCD_String_lc(itoa(fres,t,10),1,2,BLUE,~GREEN,1);
		  		while(fres!= FR_OK){};
		  		NVIC_EnableIRQ(EXTI9_5_IRQn);
		  		list_show(list.first);
//		  		last_it=1;
		  		b=strlen(dir_path);
//
		  		dir_path[b--]=0;
		  		while(dir_path[b]!='/')
		  		{
		  			dir_path[b--]=0;
		  		}
		  		dir_path[b--]=0;
		  		open_dir(dir_path);

		  		while(pressed.hold_but)
		  		{
		  			read_touch_but(&pressed);
		  		};
		  	}

//		  	if(button)
//		  	{
//				  USBD_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
//				  while(1){};
//		  	}

//		  	textbox_text(path.id,dir_path);
	  }
}


u8 open_dir(char *path)
{
	f_opendir(&dir,path);
	int il=0,a=5;

	for(int i=0;i<6*siz;i++)
	{
		for(int j=0;j<siz;j++)
		{
			tab[i][j]=0;
		}
	}

	fno.lfname = lfname;
	fno.lfsize = _MAX_LFN;

	while(!f_readdir(&dir, &fno) && fno.fname[0])
	{
			int b=sizeof(fno.fname);
			int h=0;

			if(*fno.lfname!=0)
			{
				while(*fno.lfname != 0)
				{
					tab[il][h]=*fno.lfname;
					fno.lfname++;
					h++;
				}

			}
			else
			{
				for(int j=0;j<b;j++)
				{
					tab[il][j]=fno.fname[j];
				}

			}

			list.it[il].text=tab[il];
			check_ext(tab[il],il);

			il++;
	}

	list.ele=il;
	list.resize=2;
	list.vis=5;
	list.x=2;
	list.first=0;
	list.y=1;
	list_add(&list);
	list_show(my_list.first);

	return il;
}

void icon(u8 wh, u16 x, u16 y, u16 lx, u16 ly)
{
//	lx/=2;
//	ly/=2;
	int BUF=lx*ly*3;
	LCD_area(x,y,x+lx-1,y+ly-1);
	if(wh==0)DMA_Config(BUF,fols);
	else if(wh==1)DMA_Config(BUF,files);
	else if(wh==2)DMA_Config(BUF,fileb);
	while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
	DMA2->LISR=0x00000000;

}

void check_ext(char *f, u8 il)
{
	list.it[il].roz=0;
	while(*f++ != 0)
	{
		if(*(f-3)=='.' && *(f-2)=='b' && *(f-1)=='m' && *f=='p')list.it[il].roz='b';
		else if(*(f-3)=='.' && *(f-2)=='j' && *(f-1)=='p' && *f=='g')list.it[il].roz='j';
		else if(*(f-3)=='.' && *(f-2)=='a' && *(f-1)=='v' && *f=='i')list.it[il].roz='a';
		else if(*(f-3)>0 && *(f-2)=='.' && *(f-1)>0)list.it[il].roz='u';    //unknown
	}
}
