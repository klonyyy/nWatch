/**
  ******************************************************************************
  * @file    bsp.h
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    28-January-2014
  * @brief   Target config file module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
#ifndef __BSP_H
#define __BSP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"
#include "stmpe811.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint32_t BSP_Init (void);
void BSP_Pointer_Update(void);
int APP_GetData(void * p, const U8 * * ppData, unsigned NumBytesReq, U32 Off);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
