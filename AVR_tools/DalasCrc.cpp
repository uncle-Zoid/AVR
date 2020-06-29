/* 
* DalasCrc.cpp
*
* Created: 26.06.2020 14:49:07
* Author: pappd
*/


#include "DalasCrc.h"

/*

https://www.youtube.com/watch?v=RAZZWnkSwX4

https://stackoverflow.com/questions/29214301/ios-how-to-calculate-crc-8-dallas-maxim-of-nsdata

https://www.maximintegrated.com/en/design/technical-documents/app-notes/2/27.html


uint8_t crc8(uint8_t *data, uint16_t size)
{
	uint8_t crc, i;
	crc = 0;

	while (size--)
	{
		crc ^= *data++;

		for (i = 0; i < 8; i++)
		{
			if (crc & 0x01) crc = (crc >> 1) ^ 0x8C;
			else crc >>= 1;
		}
	}

	return crc;
}
*/

DalasCrc::DalasCrc()
{
	init();
} 

void DalasCrc::init()
{
	byte_t  remainder;
	for (int i = 0; i < 256; ++i)
	{
	    remainder = i;
	    for (uint8_t i = 8; i > 0; i--)
	    {
		    if (remainder & 0x01) remainder = (remainder >> 1) ^ 0x8C;
		    else remainder >>= 1;
	    }
	    
	    crcTable[i] = remainder;
	}	
}

byte_t DalasCrc::compute8(const byte_t *message, int size)
{
	byte_t remainder = 0;

	for (int byte = 0; byte < size; ++byte)
	{
		uint8_t data = message[byte] ^ remainder;

		remainder = crcTable[data];
	}

	return remainder;
}
