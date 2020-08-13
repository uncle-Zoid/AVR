/*
 * defines.h
 *
 * Created: 12.08.2020 9:32:26
 *  Author: pappd
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_

#define F_CPU 16000000UL

#define resbit(port, bit)  (port &= ~(1<<bit));
#define setbit(port, bit)  (port |= (1<<bit));
#define testbit(port, bit) (port & (1<<bit))



#include <inttypes.h>

using byte_t = uint8_t;

struct AvrPort
{
	volatile uint8_t *ddr;
	volatile uint8_t *pin;
	volatile uint8_t *port;
};


#endif /* DEFINES_H_ */