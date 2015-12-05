/**
  ******************************************************************************
  * @file    LibJPEG/LibJPEG_Decoding/Inc/decode.h 
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    14-August-2015
  * @brief   header of decode.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DECODE_H
#define __DECODE_H

/* Includes ------------------------------------------------------------------*/
#include "ff.h"
#include <stm32f4xx.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
struct jpeg_decompress_struct cinfo;
  /* This struct represents a JPEG error handler */
struct jpeg_error_mgr jerr;

void jpeg_decode(FIL *file, uint32_t width, uint8_t * buff, uint8_t (*callback)(uint8_t*, uint32_t));
static uint8_t Jpeg_CallbackFunction(uint8_t* Row, uint32_t DataLength);
u8 play_video(char *sc);

#endif /* __DECODE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
