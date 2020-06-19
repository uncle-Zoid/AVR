/* 
* Owi.h
*
* Created: 18.06.2020 10:05:41
* Author: pappd
*/

#ifndef __OWI_H__
#define __OWI_H__

#ifndef F_PCU
#define F_CPU 16000000
#endif


#include <inttypes.h>

#define setbit (port, bit) (port |= (1<<bit));
#define resbit (port, bit) (port &= ~(1<<bit));
#define testbit(port, bit) (port & (1<<bit))


using byte_t = uint8_t;

struct AvrPort
{
	volatile uint8_t *ddr;
	volatile uint8_t *pin;
	volatile uint8_t *port;
};

class Owi
{
//variables
public:
protected:
private:

	static const uint16_t A = 6;
	static const uint16_t B = 64;
	static const uint16_t C = 60;
	static const uint16_t D = 10;
	static const uint16_t E = 9;
	static const uint16_t F = 55;
	static const uint16_t G = 0;
	static const uint16_t H = 480;
	static const uint16_t I = 70;
	static const uint16_t J = 410;
	
	/* Pouzitelne pouze pri jednom zarizeni na sbernici, zarizeni posle jako odpoved svoji ROM - 64 bitova */
	static const byte_t OWI_READ_ROM = 0x33;
	/* Komunikace se vsemi zarizenimi na sbernici, pri jednom zarizeni se bude komunikovat prave s timto, nemusi se oslovovat primo */
	static const byte_t OWI_SKIP_ROM = 0xCC;
	/* Po tomto prikazu zarizeni na sbernici ocekavaji ROM, nasledna komunikace bude probihat s odpovidajicim zarizenim */
	static const byte_t OWI_MATCH_ROM = 0x55;
	
	AvrPort port_;
	uint8_t pin_;
	
	byte_t rom[8];


//functions
public:
	Owi(const AvrPort &port, uint8_t pin);
	~Owi() = default;
	
	/**
		@return prectena hodnota -- 0=zadne zarizeni, 1=zarizeni na sbernici
	*/
	byte_t	reset		();	
	void	writeByte	(byte_t byte);
	byte_t	readByte	();
	byte_t*	readROM		();
	void	skipRom		();	
	void	matchROM	(byte_t *ROM_64bit);
	
protected:
private:
	Owi( const Owi &c );
	Owi& operator=( const Owi &c );

	byte_t readBit ();
	void writeBit (byte_t bit);
}; //Owi

#endif //__OWI_H__
