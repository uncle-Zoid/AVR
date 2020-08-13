/* 
* SevenSeg.cpp
*
* Created: 13.08.2020 9:11:24
* Author: pappd
*/


#include "SevenSeg.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

const uint8_t SevenSeg::NUMS[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,0x80, 0x90};

SevenSeg::SevenSeg(AvrPort &leds, AvrPort &control, int digits, int decimalPlaces)
	: digits_   (MIN(digits, 4))
	, counter_	(0)
	, leds_		(leds)
	, control_	(control)
	, decimalPlaces_ (decimalPlaces)
{
	showValue(0);
	
	mask_ = 0;
	for (int i = 0; i < digits_; ++i) 
	{
		mask_ = (mask_ << 1) | 0x01;
	}
	
	*leds.ddr = 0xFF;
	*control.ddr |= mask_;
	
	*leds.port = 0xFF;
	*control.port |= mask_;
}

void SevenSeg::showValue(int value)
{	
	int div = 1000;
	for (int i = 0; i < digits_; ++i)
	{
		buffer[i] = value / div;
		value %= div;
		
		div /= 10;
	}
	if (decimalPlaces_)
	{
		buffer[digits_ - decimalPlaces_ - 1] &= 0x7F;
	}
	
	for (int i = 0; i < digits_; ++i)
	{
		nums_[i] = buffer[i];
	}
	counter_ = 0;
}

void SevenSeg::display()
{	
	setbit (*control_.port, counter_);
	counter_ = (counter_ + 1) % digits_;
	*leds_.port = NUMS[nums_[counter_]];
	resbit (*control_.port, counter_);	
}
