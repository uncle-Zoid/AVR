/* 
* Owi.cpp
*
* Created: 18.06.2020 10:05:41
* Author: pappd
*/

#include "Owi.h"


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


Owi::Owi(const AvrPort &port, uint8_t pin)
	: port_ (port)
	, pin_	(pin)
{ }


byte_t Owi::reset()
{
	cli();
	setbit (*port_.ddr, pin_);  // nastav pin jako vystupni
	setbit (*port_.port, pin_); // nahod sbernici
	_delay_us(5);
	
	resbit (*port_.port, pin_); // stahni sbernici
	_delay_us(H);
	
	setbit (*port_.port, pin_); // uvolni sbernici
	_delay_us(I);
	
	resbit (*port_.ddr, pin_); // nastavit pin jako vstupni
	
	byte_t presence = !testbit (*port_.pin, pin_); // precti hodnotu
	_delay_us (J);
	
	sei();
	
	return presence;
}

void Owi::writeBit(byte_t bit)
{
	cli ();
	
	setbit (*port_.ddr, pin_);
	resbit (*port_.port, pin_);
	
	if (bit)
	{
		_delay_us(A);
		setbit (*port_.port, pin_);
		_delay_us(B);
	}
	else
	{
		_delay_us(C);
		setbit (*port_.port, pin_);
		_delay_us(D);
	}
	sei ();
}

void Owi::writeByte(byte_t byte)
{
	for (uint8_t i = 0; i < 8; ++i)
	{
		writeBit(byte & 0x01);
		byte >>= 1;
	}
}

byte_t Owi::readBit()
{
	byte_t res = 0;
	
	cli ();
	
	setbit (*port_.ddr, pin_);
	resbit (*port_.port, pin_);
	
	_delay_us (A);
	setbit (*port_.port, pin_);
	_delay_us (E);
	
	resbit (*port_.ddr, pin_); // nastav jako vstupni
	res = testbit (*port_.pin, pin_);
	_delay_us (F);
	
	sei ();
	
	return res;
}

byte_t Owi::readByte()
{
	byte_t result = 0x00;
	for (uint8_t i = 0; i < 8; ++i)
	{
		result >>= 1;
		result |= 0x80 * readBit();
	}
}

byte_t* Owi::readROM()
{
	reset();
	writeByte(OWI_READ_ROM);
	for (uint8_t i = 0; i < 8; ++i)
	{
		rom[i] = readByte();
	}
	return rom;
}

void Owi::skipRom()
{
	reset();
	writeByte(OWI_SKIP_ROM);
}

void Owi::matchROM(byte_t *ROM_64bit)
{
	reset();
	writeByte(OWI_MATCH_ROM);
	for(uint8_t i = 0; i < 8; ++i)
	{
		writeByte(ROM_64bit[i]);
	}
}
