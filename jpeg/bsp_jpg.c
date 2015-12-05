
#include "bsp_jpg.h"

FIL fsrc;

UINT tjd_input (
	JDEC* jd,		/* Decompression object */
	BYTE* buff,		/* Pointer to the read buffer (NULL:skip) */
	UINT nd			/* Number of bytes to read/skip from input stream */
){

	UINT rb;
	if (buff)
	{
		/* Read nd bytes from the input strem */
		f_read(&fsrc, buff, nd, &rb);
		return rb;	/* Returns number of bytes could be read */

	} else
	{	/* Skip nd bytes on the input stream */
		return (f_lseek(&fsrc, f_tell(&fsrc) + nd) == FR_OK) ? nd : 0;
	}
}

/* User defined call-back function to output RGB bitmap */
UINT tjd_output (
	JDEC* jd,		/* Decompression object of current session */
	void* bitmap,	/* Bitmap data to be output */
	JRECT* rect		/* Rectangular region to output */
)
{
	jd = jd;	/* Suppress warning (device identifier is not needed in this appication) */

	LCD_area( rect->left ,rect->top,rect->right, rect->bottom);

	int yc = rect->bottom - rect->top + 1;			/* Vertical size */
	int xc = rect->right - rect->left + 1;

	DMA_Config(yc*xc*3,(uint8_t*)bitmap);

	while(!(DMA2->LISR & DMA_LISR_TCIF0)){};
	DMA2->LISR=0x00000000;

	return 1;	/* Continue to decompression */  ////////1
}

void load_jpg (
	FIL *fp,	/* File to open */
	char *fn,
	void *work,		/* Pointer to the working buffer (must be 4-byte aligned) */
	UINT sz_work	/* Size of the working buffer (must be power of 2) */
)
{
	JDEC jd;		/* Decompression object (70 bytes) */
	JRESULT rc;
	BYTE scale=0;

	if(*fn!='-')f_open(&fsrc,fn, FA_READ | FA_OPEN_EXISTING);

	rc = jd_prepare(&jd, tjd_input, (uint8_t*)work, sz_work, fp);

	if (rc == JDR_OK)
	{
		rc = jd_decomp(&jd, tjd_output, 0);
	}

}


//BYTE Buff[4096] __attribute__ ((aligned(4)));
