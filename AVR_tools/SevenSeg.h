/* 
* SevenSeg.h
*
* Created: 13.08.2020 9:11:24
* Author: pappd
*/


#ifndef __SEVENSEG_H__
#define __SEVENSEG_H__

#include "defines.h"

class SevenSeg
{
	static const uint8_t NUMS[10];
public:
	SevenSeg(AvrPort &leds, AvrPort &control, int digits, int decimalPlaces = 0);

	void showValue(int value);
	
	void display();
private:
	const int digits_;
	int mask_;
	int counter_;
	int decimalPlaces_;
	
	AvrPort leds_;
	AvrPort control_;
	
	byte_t nums_[4];
	byte_t buffer[4];
	byte_t isChanged_;
}; //SevenSeg

#endif //__SEVENSEG_H__
