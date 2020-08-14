/* 
* SevenSeg.cpp
*
* Created: 13.08.2020 9:11:24
* Author: pappd
*/


#include "SevenSeg.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

const uint8_t SevenSeg::NUMS[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,0x80, 0x90};

SevenSeg::SevenSeg(AvrPort &leds, AvrPort &control, int digits, int decimalPlaces)
	: digits_   (MIN(digits, 4))
	, counter_	(0)
	, decimalPlaces_ (decimalPlaces)
	, leds_		(leds)
	, control_	(control)	
	, isChanged_	(false)
{
	showValue(0);
	
	mask_ = 0;
	for (int i = 0; i < digits_; ++i) 
	{
		mask_ = (mask_ << 1) | 0x01;
	}
	
	decimalPlaces_ = MAX(0, digits_ - decimalPlaces_ - 1);
	
	
	*leds.ddr = 0xFF;
	*control.ddr |= mask_;
	
	*leds.port = 0xFF;
	*control.port |= mask_;
}

void SevenSeg::showValue(int value)
{	
	for (int i = digits_ - 1 ; i >= 0; --i)
	{
		buffer[i] = NUMS[value % 10];
		value /= 10;	
	}
	if (decimalPlaces_)
	{
		buffer[decimalPlaces_] &= 0x7F;
	}
	
	isChanged_ = true;
}

void SevenSeg::display()
{	
	*control_.port |= mask_;

	*leds_.port = nums_[counter_];
	resbit (*control_.port, counter_);	
	
	counter_ = (counter_ + 1) % digits_;
	
	if (counter_ == 0 && isChanged_)
	{
		for (int i = 0; i < digits_; ++i)
		{
			nums_[i] = buffer[i];
		}
		isChanged_ = false;
	}
}
