#ifndef _BSP_JPG_H
#define _BSP_JPG_H

#include "stm32f4xx_fsmc.h"
#include <stm32f4xx.h>
#include <system_stm32f4xx.h>
#include <core_cm4.h>
#include <stm32f4xx_fsmc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include "bsp_jpg.h"
#include "tjpgd.h"
#include "ff.h"

UINT tjd_input (JDEC* jd,BYTE* buff,UINT nd);
UINT tjd_output (JDEC* jd,void* bitmap,JRECT* rect);


#endif _BSP_JPG_H
