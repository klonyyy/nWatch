/** \file itoa.c
 * \brief itoa() function.
 * \details itoa() function to convert integer to string.
 * \author Freddie Chopin
 * \date 2009-07-13
 */

/******************************************************************************
* project: 750-750
* chip: STM32F103VB
* compiler: arm-none-eabi-gcc 4.4.1
*
* prefix: (none)
*
* available global functions:
* 	char* itoa(int value, char* buffer, int base)
*
* available local functions:
* 	static void __reverse(char* begin,char* end);
******************************************************************************/

/*
+=============================================================================+
| includes
+=============================================================================+
*/

#include <stdint.h>

#include "itoa.h"

typedef enum {true = 1, false = 0} bool;

void reverse(char str[], int length)
{
	    int start = 0;
	    int end = length -1;
	    while (start < end)
	    {
	    	char c=*(str+start);
	        *(str+start)= *(str+end);
	        *(str+end)=c;

	        start++;
	        end--;
	    }
}
char* itoa(int num, char* str, int base)
{
    int h = 0;
    bool isNegative = false;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[h++] = '0';
        str[h] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[h++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[h++] = '-';

    str[h] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, h);

    return str;
}
///*
//+=============================================================================+
//| module variables
//+=============================================================================+
//*/
//
///*
//+=============================================================================+
//| local functions' declarations
//+=============================================================================+
//*/
//
//static void __reverse(char* begin,char* end);
//
///*
//+=============================================================================+
//| global functions
//+=============================================================================+
//*/
//
///*------------------------------------------------------------------------*//**
//* \brief itoa() function.
//* \details Converts signed integer to an char array. Valid 'base' in [2;16].
//* Only base == 10 values are treated as signed
//*
//* \param [in] value the value which will be converted
//* \param [out] buffer the output buffer
//* \param [in] base the base of conversion
//* \return pointer to \em buffer
//*//*-------------------------------------------------------------------------*/
//
//char* itoa(int value, char* buffer, int base)
//{
//	static const char digits[]="0123456789abcdef";
//
//	char* buffer_copy=buffer;
//	int32_t sign=0;
//	int32_t quot,rem;
//
//	if ((base>=2)&&(base<=16))				// is the base valid?
//	{
//		if (base == 10 && (sign = value) < 0)// negative value and base == 10? store the copy (sign)
//			value = -value;					// make it positive
//
//		do
//		{
//			quot=value/base;				// calculate quotient and remainder
//			rem=value%base;
//			*buffer++ = digits[rem];		// append the remainder to the string
//		} while ((value=quot));				// loop while there is something to convert
//
//		if (sign<0)							// was the value negative?
//			*buffer++='-';					// append the sign
//
//		__reverse(buffer_copy,buffer-1);		// reverse the string
//	}
//
//	*buffer='\0';
//	return buffer_copy;
//}
//
///*
//+=============================================================================+
//| local functions
//+=============================================================================+
//*/
//
///*------------------------------------------------------------------------*//**
//* \brief Reverse order in string.
//* \details Reverses the order of the elements in string.
//*
//* \param [in,out] begin pointer to the beginning of the string
//* \param [in,out] end pointer to the end of the string
//*//*-------------------------------------------------------------------------*/
//
//static void __reverse(char* begin,char* end)
//{
//	char temp;
//
//	while (end>begin)
//	{
//		temp=*end;
//		*end--=*begin;
//		*begin++=temp;
//	}
//}
//
///******************************************************************************
//* END OF FILE
//******************************************************************************/
