/* 
* DS18B20.h
*
* Created: 22.06.2020 10:02:51
* Author: pappd
*/


#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "Owi.h"

class DS18B20
{
//variables
public:
	enum Commands {
		CONVERT = 0x44, // spusti konverzi teploty.
		WRITE   = 0x4E, // zapis do pameti cidla, Th, Tl, config
		READ	= 0xBE, // cteni z cidla (az 9 Bajtu: temp_lsb, temp_msb, th, tl, config, 3x reserved, crc)
		COPY	= 0x48, // zkopiruje th,tl,config do EEPROM cidla
		RECALL	= 0xB8, // zkopiruje EEPROM do cidla
		POWER	= 0xB4  // nasleduje cteni jednoto bitu, 0=parazitni, 1=externi napajeni
		};
		
	enum Resolution {
		RESOLUTION_12B = 0x7F,
		RESOLUTION_11B = 0x5F,
		RESOLUTION_10B = 0x3F,
		RESOLUTION_09B = 0x1F		
		};

	enum Scratchpad {
		TEMPERATURE_LSB = 0, // spodni byte teploty
		TEMPERATURE_MSB,	 // horni byte teploty
		ThRegister,			 // horni hodnota alarmu
		TlRegister,			 // dolni hodnota alarmu
		ConfigRegister,		 // rozliseni senzoru
		Reserved1,			 // rezervovano (obsahuje hodnotu 0xff)
		Reserved2,			 // rezervovano
		Reserved3,			 // rezervovano (obsahuje hodnotu 0x10)
		Crc					 // crc
		};
	
	enum PowerSupply {		
		ParaziticSupply = 0,
		ExternalSupply,
		noSensor
		};
	static constexpr uint32_t CONVERSION_WAIT_US[4] = {750000, 375000, 187500, 93750};
	static constexpr byte_t SCRATCHPAD_SIZE = 9; 
	static constexpr byte_t ROM_SIZE = 8;
	
private:
	Owi owi_;
	byte_t scratchpad_[SCRATCHPAD_SIZE];
	byte_t rom_ [ROM_SIZE];
	PowerSupply power_;
	uint32_t delay_;

//functions
public:
	DS18B20(const AvrPort &port, uint8_t pin);
	~DS18B20() = default;
	
	
	/**
	 * \brief zahaji mereni teploty, pred prectenim vysledku je treba pockat dany cas
	 * pro 12b rozliseni je to 750 ms (viz CONVERSION_WAIT_US)
	 */
	void startConversion ();
	
	/**
	* \brief cte cely obsah dat z cidla
	*/
	void readScratchpad  ();
	
	/**
	* cte rom cidla
	*/
	void readRom	     ();
	
	/**
	* cte zdroj napajeni
	*/
	void readPowerSupply ();
	
	/**
	* zapise do konfiruracniho registru (TL, TH, rozliseni)
	*/
	void writeConfigRegister (const byte_t (&config)[3], bool writeToEEPROM);
	
	// teplota je ulozena s presnosti na dve desetinna mista 2550 = 25,5 °C
	int temperature();
	
	
protected:
private:
	DS18B20( const DS18B20 &c );
	DS18B20& operator=( const DS18B20 &c );

}; //DS18B20

#endif //__DS18B20_H__
