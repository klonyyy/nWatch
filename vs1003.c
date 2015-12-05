/* Includes ------------------------------------------------------------------*/
#include "vs1003.h"

/* Const define  -------------------------------------------------------------*/
#define RXNE    0x01
#define TXE     0x02
#define BSY     0x80



void ControlReset(uint8_t State)
{
	GPIO_WriteBit(XRESET_PORT,XRESET_PIN,!State);
}

void SCI_ChipSelect(uint8_t State)
{
	GPIO_WriteBit(CS_PORT,CS_PIN,!State);
}

void SDI_ChipSelect(uint8_t State)
{
	GPIO_WriteBit(XDCS_PORT,XDCS_PIN,!State);
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Delay
* Input          : nTime--delay time
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(u32 nTime)
{
	unsigned int i;
	unsigned long j;
	for(i = nTime;i > 0;i--)
		for(j = 1000;j > 0;j--);
}

/*******************************************************************************
* Function Name  : SPIPutChar
* Description    : Send one byte by SPI1
* Input          : outb--the byte to be sended
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char  SPIPutChar(unsigned char outB)
{
	  /* Wait if TXE cleared, Tx FIFO is full. */
	   while ((SPI1->SR & TXE) == 0);
	   SPI1->DR = outB;
	   /* Wait if RNE cleared, Rx FIFO is empty. */
	   while ((SPI1->SR & RXNE) == 0);
	   return SPI_I2S_ReceiveData(SPI1);
}

/*******************************************************************************
* Function Name  : SPIGetChar
* Description    : Read a byte from the SPI.
* Input          : None.
* Output         : None
* Return         : The received byte.
*******************************************************************************/
u8 SPIGetChar(void)
{
  u8 Data = 0;

  /* Wait until the transmit buffer is empty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  /* Send the byte */
  SPI_I2S_SendData(SPI1, 0xFF);

  /* Wait until a data is received */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  /* Get the received data */
  Data = SPI_I2S_ReceiveData(SPI1);
  /* Return the shifted data */
  return Data;
}

/*******************************************************************************
* Function Name  : Mp3SoftReset
* Description    : Soft reset the VS1003 chip
* Input          : None.
* Output         : None
* Return         : None
*******************************************************************************/
void Mp3SoftReset(void)
{

	Mp3WriteRegister (SPI_MODE, 0x08, 0x20);
	while(DREQ);
	Mp3WriteRegister(SPI_CLOCKF, 0x8b, 0xe8);
	Mp3WriteRegister(SPI_BASS, 0x00, 0x55);
	Mp3WriteRegister(SPI_AUDATA,0xAC,0x45);
	Mp3SetVolume(0x7a,0x7a);
//
    SDI_ChipSelect(SET);
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	SDI_ChipSelect(RESET);

//	Mp3WriteRegister(SPI_MODE,0x00,0x04);
}

/*******************************************************************************
* Function Name  : Mp3Reset
* Description    : Reset the VS1003 chip
* Input          : None.
* Output         : None
* Return         : None
*******************************************************************************/
void Mp3Reset(void)
{
	ControlReset(SET);									//xReset = 0   复螡vs1003
	Delay(10);							//、送覞个字节灸无效数炤启湳SPI磵输
	SCI_ChipSelect(RESET); 							//xCS = 1
	SDI_ChipSelect(RESET);     								//xDCS = 1
	ControlReset(RESET);								//xRESET = 1
	Delay(10);	           								//延使100ms
	while(DREQ);	//救待DREQ为高
    Delay(10);
    Mp3SoftReset();										//vs1003软复螡

}

void VS1053_Start()
{
        ControlReset(SET);
        Delay(100);
        SPIPutChar(0xFF);
        SCI_ChipSelect(RESET);
        SDI_ChipSelect(RESET);
        ControlReset(RESET);
        Mp3SoftReset();
        Delay(100);

        while(GPIO_ReadInputDataBit(DREQ_PORT,DREQ_PIN) == 0);

        Mp3WriteRegister(SPI_MODE,0x08,0x00);
        Mp3WriteRegister(SPI_CLOCKF,0x98,0x00);
        Mp3WriteRegister(SPI_AUDATA,0xAC,0x45);
        Mp3WriteRegister(SPI_BASS,0x08,0x00);
        Mp3WriteRegister(SPI_VOL,0x0B,0x0B);
        Mp3WriteRegister(SPI_STATUS,0,0b00110011);

        while(GPIO_ReadInputDataBit(DREQ_PORT,DREQ_PIN) == 0);
}

/*******************************************************************************
* Function Name  : VsSineTest
* Description    : VS1003 sine test
* Input          : None.
* Output         : None
* Return         : None
*******************************************************************************/
void VsSineTest(void)
{
	Mp3PutInReset();  //xReset = 0   复螡vs1003
	Mp3Reset();
	Delay(1000);//wait(100);        //延使100ms
	SPIPutChar(0xff);//、送覞个字节灸无效数炤启湳SPI磵输
	SCI_ChipSelect(RESET);
	SDI_ChipSelect(RESET);
	Mp3ReleaseFromReset();

	Mp3Reset();
	Delay(500);//wait(100);

	Mp3SetVolume(50,50);//设置音量

 	Mp3WriteRegister(SPI_MODE,0x08,0x60);//进入vs1003灸测试模式
	Delay(500);
	while(DREQ);     //救待DREQ为高

 	SDI_ChipSelect(SET);       //xDCS = 1靴择vs1003灸数炤接口

 	//向vs1003、送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//其中n = 0x24, 设湪vs1003所产生灸正弦波灸凭率志炦体熎算〗〃燐vs1003灸datasheet
    SPIPutChar(0x53);
	SPIPutChar(0xef);
	SPIPutChar(0x6e);
	SPIPutChar(0x24);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	Delay(1000);
	SDI_ChipSelect(RESET);//程序执行窘这里后訉该能从滫濟听窘覞个炯覞凭率灸声音

    //退出正弦测试
	SDI_ChipSelect(SET);
	SPIPutChar(0x45);
	SPIPutChar(0x78);
	SPIPutChar(0x69);
	SPIPutChar(0x74);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	Delay(1000);
	SDI_ChipSelect(RESET);

    //再次进入正弦测试并设置n志为0x44煷綅正弦波灸凭率设置为另外灸志
    SDI_ChipSelect(SET);
	SPIPutChar(0x53);
	SPIPutChar(0xef);
	SPIPutChar(0x6e);
	SPIPutChar(0x44);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	Delay(1000);
	SDI_ChipSelect(RESET);

	//退出正弦测试
	SDI_ChipSelect(SET);
	SPIPutChar(0x45);
	SPIPutChar(0x78);
	SPIPutChar(0x69);
	SPIPutChar(0x74);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	Delay(1000);
	SDI_ChipSelect(RESET);

 }

/*******************************************************************************
* Function Name  : VsRamTest
* Description    : Test VS1003's register,if received value is 0x807F,then
				   indicate the VS1003 is OK.
* Input          : None.
* Output         : None
* Return         : None
*******************************************************************************/
void VsRamTest(void)
{
	u16 regvalue ;

	regvalue = 0;
	ControlReset(SET);
	Delay(100);
	SPIPutChar(0xff);										//、送覞个字节灸无效数炤启湳SPI磵输
	SCI_ChipSelect(RESET);
	SDI_ChipSelect(RESET);
	ControlReset(RESET);
	Delay(100);
 	Mp3WriteRegister(SPI_MODE,0x09,0x00);					// 进入vs1003灸测试模式
	while(DREQ);     	// 救待DREQ为高
 	SDI_ChipSelect(SET);       									// xDCS = 1靴择vs1003灸数炤接口

    SPIPutChar(0x4d);
	SPIPutChar(0xea);
	SPIPutChar(0x6d);
	SPIPutChar(0x54);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	Delay(500);
	SDI_ChipSelect(RESET);

	regvalue = Mp3ReadRegister(SPI_HDAT0);      				// 如汒久窘灸志为0x807F则鬼明完好ィ

}

/*******************************************************************************
* Function Name  : Mp3WriteRegister
* Description    : Write VS1003 register
* Input          : addressbyte--the vs1003 register address
				   highbyte--the hight 8 bits
				   lowbyte--the low 8 bits
* Output         : None
* Return         : None
*******************************************************************************/
void Mp3WriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte)
{
	SDI_ChipSelect(RESET);
	while(GPIO_ReadInputDataBit(DREQ_PORT,DREQ_PIN) == 0);
	SCI_ChipSelect(SET);
	SPIPutChar(VS_WRITE_COMMAND);
	SPIPutChar(addressbyte);
	SPIPutChar(highbyte);
	SPIPutChar(lowbyte);
	while(GPIO_ReadInputDataBit(DREQ_PORT,DREQ_PIN) == 0);
	SCI_ChipSelect(RESET);

}

/*******************************************************************************
* Function Name  : Mp3ReadRegister
* Description    : Read VS1003 register
* Input          : addressbyte--the vs1003 register address
* Output         : None
* Return         : The register value
*******************************************************************************/
u16 Mp3ReadRegister(unsigned char addressbyte)
{
	u16 resultvalue = 0;
	SDI_ChipSelect(RESET);
	while(DREQ);
	SCI_ChipSelect(SET);				//XCS = 0
	SPIPutChar(VS_READ_COMMAND); 	//、送溋熌存器命令
	SPIPutChar(addressbyte);	 	//、送熌存器灸矩帧
	resultvalue = SPIGetChar() << 8;//溋去高8螡数炤
	resultvalue |= SPIGetChar();  	//溋去就8螡数炤
	while(DREQ);
	SCI_ChipSelect(RESET);
	return resultvalue;           	//【澵16螡熌存器灸志

}

/*******************************************************************************
* Function Name  : VS1003_Config
* Description    : Configures the GPIO ports and SPI1
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void VS1003_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;

	RCC->APB2ENR|= RCC_APB2ENR_SPI1EN;

	GPIO_InitStructure.GPIO_Pin = CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(CS_PORT, &GPIO_InitStructure);

//	GPIO_WriteBit(CS_PORT,CS_PIN,1);//

	GPIO_InitStructure.GPIO_Pin = XDCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(XDCS_PORT, &GPIO_InitStructure);

//	GPIO_WriteBit(XDCS_PORT,XDCS_PIN,1);//

	GPIO_InitStructure.GPIO_Pin = XRESET_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(XRESET_PORT, &GPIO_InitStructure);

	GPIO_WriteBit(XRESET_PORT, XRESET_PIN,1);

  /* Disable all pins */
//  GPIOA->ODR = 0xFF;

	SCI_ChipSelect(SET);
	SDI_ChipSelect(RESET);
	SCI_ChipSelect(RESET);

//	for(int i=0;i<0xff;i++)
//	{
//		SCI_ChipSelect(RESET);
//	}

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	GPIO_WriteBit(GPIOA,GPIO_PinSource5,1);

	// connect SPI1 pins to SPI alternate function
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

  /* SPI1 Config --------------------------------------------------*/
//  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
//  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//  SPI_InitStructure.SPI_CRCPolynomial = 7;
//  SPI_Init(SPI1, &SPI_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
//  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  	SPI_InitStructure.SPI_CRCPolynomial = 7;
  	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE); // enable SPI1

//  SPI_SSOutputCmd(SPI1, ENABLE);

  /* Enable SPI1 */
  SPI_Cmd(SPI1, ENABLE);


//  GPIOB->BSRRL|=GPIO_BSRR_BR_3; /////w砤cznie uk彻du

}

uint16_t VS1003_GetBitrate(void)
{
	uint16_t bitrate = (Mp3ReadRegister(SPI_HDAT0) & 0b1111000000000000) >> 12;
	uint8_t ID = (Mp3ReadRegister(SPI_HDAT1) & 0b0000000000011000) >> 3;
	uint16_t res;
	if (ID == 3)
	{	res = 32;
		while(bitrate>13)
		{
			res+=64;
			bitrate--;
		}
		while (bitrate>9)
		{
			res+=32;
			bitrate--;
		}
		while (bitrate>5)
		{
			res+=16;
			bitrate--;
		}
		while (bitrate>1)
		{
			res+=8;
			bitrate--;
		}
	}
	else
	{	res = 8;

		while (bitrate>8)
		{
			res+=16;
			bitrate--;
		}
		while (bitrate>1)
		{
			res+=8;
			bitrate--;
		}
	}
	return res;
}
