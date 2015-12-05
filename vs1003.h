/*
 * vs1003.h
 *
 *  Created on: 25-04-2011
 *      Author: Przemyslaw Stasiak
 */
#include "stm32f4xx.h"
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_spi.h>




#ifndef VS10XX_H
#define VS10XX_H



/* Defines ------------------------------------------------------------------*/
#define VS_WRITE_COMMAND        0x02
#define VS_READ_COMMAND         0x03
#define SPI_MODE                0x00
#define SPI_STATUS              0x01
#define SPI_BASS                0x02
#define SPI_CLOCKF              0x03
#define SPI_DECODE_TIME         0x04
#define SPI_AUDATA              0x05
#define SPI_WRAM                0x06
#define SPI_WRAMADDR            0x07
#define SPI_HDAT0               0x08
#define SPI_HDAT1               0x09
#define SPI_AIADDR              0x0a
#define SPI_VOL                 0x0b
#define SPI_AICTRL0             0x0c
#define SPI_AICTRL1             0x0d
#define SPI_AICTRL2             0x0e
#define SPI_AICTRL3             0x0f
#define SM_DIFF                 0x01
#define SM_JUMP                 0x02
#define SM_RESET                0x04
#define SM_OUTOFWAV             0x08
#define SM_PDOWN                0x10
#define SM_TESTS                0x20
#define SM_STREAM               0x40
#define SM_PLUSV                0x80
#define SM_DACT                 0x100
#define SM_SDIORD               0x200
#define SM_SDISHARE             0x400
#define SM_SDINEW               0x800
#define SM_ADPCM                0x1000
#define SM_ADPCM_HP             0x2000

#define DREQ !((GPIOA->IDR) & GPIO_IDR_IDR_8)


#define MP3CMD_InitVS1003               0x11
#define MP3CMD_Play                             0x12
#define MP3CMD_Pause                    0x13
#define MP3CMD_Stop                             0x14
#define MP3CMD_Next                             0x15
#define MP3CMD_TestVS1003               0x16


#define SCLK (1 << 5)
#define MOSI (1 << 7)
#define MISO (1 << 6)

//#define MP3_XRESET              ( 1 << 12 )
//#define Mp3PutInReset()         { GPIOB-> &= ~MP3_XRESET; }
//#define Mp3ReleaseFromReset()   { GPIOB->ODR |= MP3_XRESET; }
//#define MP3_XCS                 ( 1 << 4 )
//#define Mp3SelectControl()      { GPIOA->ODR &= ~MP3_XCS; }
//#define Mp3DeselectControl()    { GPIOA->ODR |= MP3_XCS; }
//#define MP3_XDCS                ( 1 << 15 )
//#define Mp3SelectData()         { GPIOA->ODR &= ~MP3_XDCS; }
//#define Mp3DeselectData()       { GPIOA->ODR |= MP3_XDCS; }
//#define MP3_DREQ                ( 1 << 8 )

#define DREQ_PORT	GPIOA
#define DREQ_PIN	GPIO_Pin_8
#define CS_PORT		GPIOA
#define CS_PIN		GPIO_Pin_4
#define XDCS_PORT   GPIOA
#define XDCS_PIN	GPIO_Pin_15
#define XRESET_PORT	GPIOB
#define XRESET_PIN	GPIO_Pin_12

#define VS_Start                0x01
#define VS_End                  0x02
#define Mp3SetVolume(leftchannel,rightchannel){Mp3WriteRegister(11,(leftchannel),(rightchannel));}             // Ä‚â€™Ä‚Â´Ä‚ďż˝Ă‚ĹĽÄ‚â€°Ä‚Â¨Ä‚â€“Ä‚ďż˝

void VS1003_Config(void);
unsigned char SPIPutChar(unsigned char c);                                    // Ä‚Ĺ¤Ă‚Â¨Ă‚Ä…Ä‚ËťSPIĂ‚Â·Ă‚Ë�Ä‚â€ąÄ‚Ĺ¤Ä‚â€™Ă‚Â»Ă‚Â¸Ä‚Â¶Ä‚â€”Ä‚â€“Ă‚ËťÄ‚ĹˇĂ‚ÂµÄ‚â€žÄ‚Ĺ Ä‚ËťĂ‚ÄľÄ‚ĹĄ
void Mp3SoftReset(void);                                                             // vs1003Ă‚ÂµÄ‚â€žÄ‚ďż˝Ä‚Â­Ă‚Ä˝Ä‚ÄľĂ‚Â¸Ă‚Â´Ä‚Ĺ˝Ă‚Â»
void Mp3Reset(void);
void VsRamTest(void);
void VS1053_Start();// vs1003Ă‚ÂµÄ‚â€žÄ‚â€śĂ‚Ë›Ă‚Ä˝Ä‚ÄľĂ‚Â¸Ă‚Â´Ä‚Ĺ˝Ă‚Â»
void VsSineTest(void);                                                               // vs1003Ă‚ÂµÄ‚â€žÄ‚â€˘Ä‚ËťÄ‚ĹąÄ‚â€™Ă‚Ë›Ä‚Ë�Ä‚Ĺ Ä‚â€ť
void Mp3WriteRegister(unsigned char addressbyte,unsigned char highbyte,unsigned char lowbyte); // Ä‚ďż˝Ă‚Â´vs1003Ă‚Ä˝Ä‚â€žĂ‚Â´Ä‚Â¦Ä‚â€ Ä‚Â·
unsigned short  Mp3ReadRegister(unsigned char addressbyte);             // Ă‚Â¶Ä‚ďż˝vs1003Ă‚Ä˝Ä‚â€žĂ‚Â´Ä‚Â¦Ä‚â€ Ä‚Â·
void  Mp3MusicTest(void);
uint16_t VS1003_GetBitrate(void);
#endif
