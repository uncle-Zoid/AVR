/* 
* DS18B20.cpp
*
* Created: 22.06.2020 10:02:50
* Author: pappd
*/

#include <string.h>

#include "DS18B20.h"

// default constructor
DS18B20::DS18B20(const AvrPort &port, uint8_t pin)
	: owi_   (port, pin)
	, power_ (noSensor)
{
	memset(scratchpad_, 0, SCRATCHPAD_SIZE);
	memset(rom_, 0, ROM_SIZE);
	
	readPowerSupply();
	readRom();
	readScratchpad();
} 

void DS18B20::startConversion()
{
	if(owi_.reset())
	{
		owi_.skipRom();
		owi_.writeByte(CONVERT);
	}
	else
	{
		power_ = noSensor;
	}
}

void DS18B20::readScratchpad()
{
	if (owi_.reset())
	{
		
		owi_.skipRom();
		owi_.writeByte(READ);
		
		for (byte_t i = 0; i < SCRATCHPAD_SIZE; ++i)
		{
			scratchpad_[i] = owi_.readByte();
		}
	}
	else
	{
		power_ = noSensor;
	}
}

void DS18B20::readRom()
{
	owi_.readROM(rom_, ROM_SIZE);
}

void DS18B20::readPowerSupply()
{
	if (owi_.reset())
	{		
		owi_.skipRom();
		owi_.writeByte(POWER);
		
		// power supply: 0=parasitic, 1=external
		power_ = PowerSupply(owi_.readBit());
	}
	else
	{
		power_ = noSensor;
	}
}

void DS18B20::writeConfigRegister(const byte_t (&config)[3], bool writeToEEPROM)
{
	if (owi_.reset())
	{
		owi_.skipRom();
		owi_.writeByte(WRITE);
		for (byte_t i = 0; i < 3; ++i)
		{
			owi_.writeByte(config[i]);
		}
		if (writeToEEPROM)
		{
			owi_.writeByte(COPY);
		}
	}
}

int DS18B20::temperature()
{
	int sign = 1; // kladne znamenko
	int t = (scratchpad_[TEMPERATURE_MSB] << 8) | scratchpad_[TEMPERATURE_LSB];
	// je teplota zaporna?
	if (t & 0x8000)
	{
		t = (t ^ 0xFFFF) + 1; // dvojkovy doplnek
		sign = -1;
	}
	switch (scratchpad_[ConfigRegister])
	{
		// vsechny bity platne ... vysledna teplota se ziska nasobenim prirustkem = teplota * 0,0625
		// -> nasobim 6,25 ... tj nasob 6x a pricti vydeleni 4 (1/4=0,25) ... 6x = 4x + 2x = posun2x + posun1x
		case RESOLUTION_12B:
		t = (t << 2) + (t << 1) + (t >> 2);
		break;
		
		// bit 0 neni platny ... rozliseni je 0,125
		// nasobim 12,5 ... teplota*12 + teplota/2 ... 12x = 8x + 4x = posun3x + posun2x
		case RESOLUTION_11B:
		t >>= 1;
		t = (t << 3) + (t << 2) + (t >> 1);
		break;
		
		// bity 0 a 1 nejsou platne ... rozliseni 0,25
		// nasobim 25 ... teplota*25 ... 25x = 16x + 8x + 1x
		case RESOLUTION_10B:
		t >>= 2;
		t = (t << 4) + (t << 3) + t;
		break;
		
		// bity 0, 1 a 2 nejsou platne
		// rozliseni 0,5 ... nasobim 50x = 32x + 16x + 2x
		case RESOLUTION_09B:
		t >>= 3;
		t = (t << 5) + (t << 4) + (t << 1);
		break;
	}
	return sign * t;
}
